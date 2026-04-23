#ifndef TEMPO_UTILS_ROPE_H
#define TEMPO_UTILS_ROPE_H

#include <stack>

#include "rope_operations.h"

namespace tempo_utils {

    // forward declarations
    template<class ElementType> class RopeChunkIterator;
    template<class ElementType> class RopeElementIterator;

    /**
     *
     * @tparam ElementType
     */
    template<class ElementType>
    class Rope {
    public:
        Rope() = default;

        Rope(SharedRopeNode<ElementType> node) : m_rope(std::move(node)) {}

        template<class InputIt>
        Rope(InputIt begin, InputIt end)
        {
            std::vector<ElementType> elements(begin, end);
            if (!elements.empty()) {
                m_rope = make_rope(std::move(elements));
            }
        }

        Rope(std::initializer_list<ElementType> init) : Rope(init.begin(), init.end()) {}

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
        const ElementType &elementAt(size_t index)
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

        /**
         * Construct a new rope which is a subspan of the rope. The first element of the subspan is specified by
         * offset, and the maximum number of elements in the subspan is specified by count. If the count
         * specifies more elements than are available, the resulting subspan will contain the maximum number
         * of elements available.
         *
         * @param offset
         * @param count
         * @return
         */
        Rope<ElementType> subspan(size_t offset, size_t count = std::numeric_limits<size_t>::max())
        {
            if (count == 0)
                return {};
            std::vector<std::shared_ptr<LeafRopeNode<ElementType>>> leaves;
            auto end = offset + count;
            if (end < offset) {
                end = count;
            }
            collect_leaves(m_rope, offset, end, leaves);
            return Rope(merge_leaves(leaves));
        }

        /**
         * Iterate over the chunks of the rope.
         *
         * @return
         */
        RopeChunkIterator<ElementType> iterateChunks() const
        {
            return RopeChunkIterator<ElementType>(m_rope);
        }

        /**
         * Iterate over the elements of the rope.
         *
         * @return
         */
        RopeElementIterator<ElementType> iterateElements() const
        {
            return RopeElementIterator<ElementType>(m_rope);
        }

        size_t getDepth() const
        {
            if (m_rope == nullptr)
                return 0;
            return m_rope->getDepth();
        }

    private:
        SharedRopeNode<ElementType> m_rope;
    };

    /**
     *
     * @tparam ElementType
     */
    template<class ElementType>
    class RopeChunkIterator {
    public:
        explicit RopeChunkIterator(SharedRopeNode<ElementType> rope)
            : m_priv(std::make_shared<Priv>(std::move(rope)))
        {
            load(m_priv->root);
        }

        /**
         * Returns whether the iterator has another chunk available.
         *
         * @return true if another chunk is available, otherwise false.
         */
        bool hasNext() const
        {
            return !m_priv->stack.empty();
        }

        /**
         * Return the next chunk if one is available, otherwise nullptr;
         *
         * @return
         */
        bool getNext(RopeChunk<ElementType> &chunk)
        {
            if (m_priv->stack.empty())
                return false;

            auto curr = m_priv->stack.top();
            TU_ASSERT (curr->getType() == RopeNodeType::LEAF);
            m_priv->stack.pop();

            while (!m_priv->stack.empty()) {
                auto parent = m_priv->stack.top();
                m_priv->stack.pop();
                TU_ASSERT (parent->getType() == RopeNodeType::CONCAT);
                auto concat = std::static_pointer_cast<ConcatRopeNode<ElementType>>(parent);
                auto right = concat->getRightNode();
                if (right != nullptr) {
                    load(right);
                    TU_ASSERT (m_priv->stack.top()->getType() == RopeNodeType::LEAF);
                    break;
                }
            }

            auto leaf = std::static_pointer_cast<LeafRopeNode<ElementType>>(curr);
            chunk = leaf->getChunk();
            return true;
        }

    private:
        struct Priv {
            SharedRopeNode<ElementType> root;
            std::stack<SharedRopeNode<ElementType>> stack;
        };
        std::shared_ptr<Priv> m_priv;

        void load(SharedRopeNode<ElementType> node)
        {
            if (node == nullptr)
                return;
            while (node != nullptr) {
                switch (node->getType()) {
                    case RopeNodeType::LEAF: {
                        m_priv->stack.push(std::move(node));
                        return;
                    }
                    case RopeNodeType::CONCAT: {
                        auto concat = std::static_pointer_cast<ConcatRopeNode<ElementType>>(node);
                        m_priv->stack.push(node);
                        node = concat->getLeftNode();
                        break;
                    }
                    default:
                        TU_UNREACHABLE();
                }
            }
        }
    };

    template<class ElementType>
    class RopeElementIterator {
    public:
        explicit RopeElementIterator(SharedRopeNode<ElementType> rope)
            : m_priv(std::make_shared<Priv>(RopeChunkIterator(rope)))
        {
            m_priv->done = !m_priv->it.getNext(m_priv->chunk);
        }

        bool hasNext() const
        {
            return !m_priv->done;
        }

        bool getNext(ElementType &element)
        {
            if (m_priv->done)
                return false;

            auto chunk = m_priv->chunk;
            auto index = m_priv->index;

            if (chunk.numElements() <= ++m_priv->index) {
                m_priv->index = 0;
                m_priv->done = !m_priv->it.getNext(m_priv->chunk);
            }

            element = chunk.elementAt(index);
            return true;
        }

    private:
        struct Priv {
            RopeChunkIterator<ElementType> it;
            RopeChunk<ElementType> chunk;
            size_t index = 0;
            bool done = true;
        };
        std::shared_ptr<Priv> m_priv;
    };
}

#endif //TEMPO_UTILS_ROPE_H
