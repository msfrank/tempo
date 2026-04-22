#ifndef TEMPO_UTILS_ROPE_H
#define TEMPO_UTILS_ROPE_H

#include "rope_operations.h"

namespace tempo_utils {

    /**
     *
     * @tparam ElementType
     */
    template<class ElementType>
    class Rope {
    public:
        Rope() = default;

        template<class InputIt>
        Rope(InputIt begin, InputIt end)
        {
            std::vector<ElementType> elements(begin, end);
            if (!elements.empty()) {
                m_rope = make_rope(std::move(elements));
            }
        }

        Rope(std::initializer_list<ElementType> init)
            : Rope(init.begin(), init.end())
        {
        }

        /**
         * Returns whether the rope is empty.
         *
         * @return true if the rope is empty, otherwise false.
         */
        bool isEmpty() const
        {
            return m_rope == nullptr;
        }

        /**
         * Returns the total number of elements in the rope.
         *
         * @return The number of elements.
         */
        size_t numElements() const
        {
            if (m_rope != nullptr)
                return m_rope->getSize();
            return 0;
        }

        /**
         * Get the element at the specified index.
         *
         * @param index
         * @return
         */
        ElementType getElement(size_t index)
        {
            return m_rope->elementAt(index);
        }

        /**
         * Return a mutable reference to the element at the specified index.
         *
         * @param index
         * @return
         */
        ElementType &elementAt(size_t index)
        {
            return m_rope->elementAt(index);
        }

        /**
         * Append `other` after this rope and return the new rope.
         *
         * @param other
         * @return
         */
        Rope<ElementType> append(Rope<ElementType> other)
        {
            if (isEmpty())
                return other;
            if (other.isEmpty())
                return Rope(m_rope);

            auto rope = concat_rope(m_rope, other.m_rope);
            if (!rope->isBalanced()) {
                rope = rebalance_rope(rope);
            }
            return Rope(rope);
        }

        /**
         * Prepend `other` before this rope and return the new rope.
         *
         * @param other
         * @return
         */
        Rope<ElementType> prepend(Rope<ElementType> other)
        {
            if (isEmpty())
                return other;
            if (other.isEmpty())
                return Rope(m_rope);

            auto rope = concat_rope(other.m_rope, m_rope);
            if (!rope->isBalanced()) {
                rope = rebalance_rope(rope);
            }
            return Rope(rope);
        }

        /**
         * Split the rope at the specified index and return a pair whose first member is the rope containing
         * elements 0 to index - 1, and whose second member is the rope containing elements index to size - 1.
         *
         * If rope is empty, then the pair contains two empty ropes. If index is 0 then the first member of
         * pair is an empty rope and the second member contains the entire rope. If index >= size then the
         * first member of pair is the entire rope and the second member contains an empty rope.
         *
         * @param index
         * @return
         */
        std::pair<Rope<ElementType>,Rope<ElementType>> split(size_t index)
        {
            if (isEmpty())
                return std::pair(Rope(), Rope());
            if (index == 0)
                return std::pair(Rope(), Rope(m_rope));

            auto pair = split_rope(m_rope, index);
            return std::pair(Rope(pair.first), Rope(pair.second));
        }

        size_t getDepth() const
        {
            if (m_rope == nullptr)
                return 0;
            return m_rope->getDepth();
        }

    private:
        SharedRopeNode<ElementType> m_rope;

        explicit Rope(SharedRopeNode<ElementType> rope) : m_rope(std::move(rope)) {}
    };

    // template<class ElementType>
    // class RopeIterator {
    // public:
    // };
}

#endif //TEMPO_UTILS_ROPE_H
