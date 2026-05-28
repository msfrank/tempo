#ifndef TEMPO_UTILS_GAP_BUFFER_H
#define TEMPO_UTILS_GAP_BUFFER_H

#include <cmath>
#include <vector>
#include <span>

#include "log_message.h"

namespace tempo_utils {

    template<class ElementType>
    class GapBuffer {
    public:

        GapBuffer() = default;

        explicit GapBuffer(size_t initialSize) : m_buf(initialSize), m_size(initialSize) {}

        GapBuffer(const ElementType *data, size_t size, size_t gap = 0)
        {
            TU_NOTNULL (data);
            m_buf.insert(m_buf.begin(), data, data + size);
            m_presize = size;
            m_postsize = 0;
            if (gap > 0) {
                m_buf.resize(size + gap);
            }
            m_size = m_buf.size();
        }

        explicit GapBuffer(std::span<ElementType> elements, size_t gap = 0)
        {
            if (!elements.empty()) {
                m_buf.insert(m_buf.begin(), elements.begin(), elements.end());
                m_presize = elements.size();
            }
            m_postsize = 0;
            if (gap > 0) {
                m_buf.resize(elements.size() + gap);
            }
            m_size = m_buf.size();
        }

        template<class InputIt>
        GapBuffer(InputIt begin, InputIt end, size_t gap = 0)
        {
            m_buf.insert(m_buf.begin(), begin, end);
            m_presize = m_buf.size();
            m_postsize = 0;
            if (gap > 0) {
                m_buf.resize(m_buf.size() + gap);
            }
            m_size = m_buf.size();
        }

        /*
         * accessors
         */

        bool isEmpty() const
        {
            return m_presize + m_postsize == 0;
        }

        size_t numElements() const
        {
            return m_presize + m_postsize;
        }

        size_t gapSize() const
        {
            return m_size - (m_presize + m_postsize);
        }

        size_t bufferSize() const
        {
            return m_size;
        }

        size_t getCursor() const
        {
            return m_presize;
        }

        size_t getCursorEnd() const
        {
            return m_size - m_postsize;
        }

        const ElementType &cursorElement() const
        {
            TU_ASSERT (!isEmpty());
            if (m_presize > 0)
                return m_buf[m_presize - 1];
            return m_buf[m_size - m_postsize];
        }

        ElementType &cursorElement()
        {
            TU_ASSERT (!isEmpty());
            if (m_presize > 0)
                return m_buf[m_presize - 1];
            return m_buf[m_size - m_postsize];
        }

        ElementType getElement(size_t index) const
        {
            TU_ASSERT (index < numElements());
            if (index < m_presize) {
                return m_buf.at(index);
            }
            return m_buf.at(index + gapSize());
        }

        const ElementType &elementAt(size_t index) const
        {
            TU_ASSERT (index < numElements());
            if (index < m_presize) {
                return m_buf.at(index);
            }
            return m_buf.at(index + gapSize());
        }

        ElementType &elementAt(size_t index)
        {
            TU_ASSERT (index < numElements());
            if (index < m_presize) {
                return m_buf.at(index);
            }
            return m_buf.at(index + gapSize());
        }

        /**
         * Move the cursor one step forward, i.e. towards the end of the buffer.
         *
         * @return Returns true if the cursor moved, otherwise false if the cursor is already at the end.
         */
        bool stepForward()
        {
            if (m_postsize == 0)
                return false;

            m_buf[m_presize] = m_buf[m_size - m_postsize];
            ++m_presize;
            --m_postsize;
            return true;
        }

        /**
         * Move the cursor one step backward, i.e. towards the start of the buffer.
         *
         * @return Returns true if the cursor moved, otherwise false if the cursor is already at the start.
         */
        bool stepBackward()
        {
            if (m_presize > 0) {
                auto element = m_buf[m_presize - 1];
                m_buf[m_size - m_postsize - 1] = element;
                ++m_postsize;
                --m_presize;
                return true;
            }
            return false;
        }

        /**
         * If `offset` is positive then move the cursor `offset` steps forward, i.e. towards the end of the
         * buffer. If `offset` is negative then move the cursor -`offset` steps backward, i.e. towards the start
         * of the buffer.
         *
         * @param offset The distance and direction of the movement.
         * @return The distance between the previous cursor position and the new position.
         */
        size_t step(ssize_t offset)
        {
            size_t distance = 0;
            if (offset > 0) {
                for (size_t i = 0; i < offset; ++i) {
                    if (!stepForward())
                        break;
                    ++distance;
                }
            } else {
                offset = std::abs(offset);
                for (size_t i = 0; i < offset; ++i) {
                    if (!stepBackward())
                        break;
                    ++distance;
                }
            }
            return distance;
        }

        /**
         * Move the cursor to the position before the element at `index`. If the index is outside the range of
         * the buffer then the cursor is moved to the position after the last element.
         *
         * @param index The element index.
         * @return The distance between the previous cursor position and the new position.
         */
        size_t moveBefore(size_t index)
        {
            // we are moving toward the end (i.e. forward)
            if (index >= m_presize) {
                size_t distance = index - m_presize;
                for (size_t i = 0; i < distance; ++i) {
                    if (!stepForward())
                        return i;
                }
                return distance;
            }

            // we are moving toward the start (i.e. backward)
            size_t distance = m_presize - index;
            for (size_t i = 0; i < distance; ++i) {
                if (!stepBackward())
                    return i;
            }
            return distance;
        }

        /**
         * Move the cursor to the position after the element at `index`. If the index is outside the range of
         * the buffer then the cursor is moved to the position after the last element.
         *
         * @param index The element index.
         * @return The distance between the previous cursor position and the new position.
         */
        size_t moveAfter(size_t index)
        {
            return moveBefore(index + 1);
        }

        /**
         * Move the cursor to the position before the first element.
         *
         * @return The distance between the previous cursor position and the new position.
         */
        size_t moveToStart()
        {
            return moveBefore(0);
        }

        /**
         * Move the cursor to the position after the last element.
         *
         * @return The distance between the previous cursor position and the new position.
         */
        size_t moveToEnd()
        {
            return moveBefore(m_presize + m_postsize);
        }

        /**
         * Insert `element` at the current cursor position.
         *
         * @param element
         */
        void insert(ElementType element) {
            if (m_presize + m_postsize == m_size) {
                growBuffer();
            }
            m_buf[m_presize] = element;
            ++m_presize;
        }

        /**
         * Insert each element in the range between the iterators `begin` and `end`.
         *
         * @tparam InputIt The element iterator type.
         * @param begin The begin iterator.
         * @param end The end iterator.
         */
        template<class InputIt>
        void insert(InputIt begin, InputIt end)
        {
            for (auto it = begin; it != end; ++it) {
                insert(*it);
            }
        }

        /**
         * Remove up to `count` elements, starting at the current cursor position and proceeding backward, i.e.
         * toward the start of the buffer.
         *
         * @param count The number of elements to remove.
         * @return The number of elements removed, which may be less than `count` depending on the cursor position.
         */
        size_t removeBackward(size_t count)
        {
            if (count < m_presize) {
                m_presize -= count;
            } else {
                count = m_presize;
                m_presize = 0;
            }
            return count;
        }

        /**
         * Remove up to `count` elements, starting at the current cursor position and proceeding forward, i.e.
         * toward  the end of the buffer.
         *
         * @param count The number of elements to remove.
         * @return The number of elements removed, which may be less than `count` depending on the cursor position.
         */
        size_t removeForward(size_t count)
        {
            if (count < m_postsize) {
                m_postsize -= count;
            } else {
                count = m_postsize;
                m_postsize = 0;
            }
            return count;
        }

        /**
         * Grow the buffer to accomodate at least one more element.
         */
        void growBuffer() {
            size_t newsize = std::max(m_size * 2, static_cast<size_t>(64));
            std::vector<ElementType> newbuf(newsize);
            for (size_t i = 0; i < m_presize; ++i) {
                newbuf[i] = m_buf[i];
            }
            for (size_t i = 0; i < m_postsize; ++i) {
                newbuf[newsize - i - 1] = m_buf[m_size - i - 1];
            }
            m_size = newsize;
            m_buf = std::move(newbuf);
        }

        /**
         * Construct a vector containing the elements of the buffer.
         *
         * @return The vector.
         */
        std::vector<ElementType> toVector() const
        {
            std::vector<ElementType> vec(m_presize + m_postsize);
            for (size_t i = 0; i < m_presize; ++i) {
                vec[i] = m_buf[i];
            }
            size_t poststart = m_size - m_postsize;
            for (size_t i = 0; i < m_postsize; ++i) {
                vec[m_presize + i] = m_buf[poststart + i];
            }
            return vec;
        }

    private:
        std::vector<ElementType> m_buf;
        size_t m_size = 0;
        size_t m_presize = 0;
        size_t m_postsize = 0;
    };
}

#endif // TEMPO_UTILS_GAP_BUFFER_H
