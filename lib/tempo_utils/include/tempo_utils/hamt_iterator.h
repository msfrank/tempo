#ifndef TEMPO_UTILS_HAMT_ITERATOR_H
#define TEMPO_UTILS_HAMT_ITERATOR_H

#include <vector>

#include "abstract_iterator.h"
#include "hamt_node.h"

namespace tempo_utils {

    /**
     * Iterates over the entries in the trie.
     *
     * @tparam K The key type.
     * @tparam V The value type.
     * @tparam H The hash functor type.
     * @tparam E The key equals functor type.
     */
    template<class K, class V, class H, class E>
    class HamtIterator : public AbstractIterator<HamtEntry<K,V>> {
    public:

        /**
         * Constructs a HamtIterator to iterate over the entries rooted at the `root` node.
         *
         * @param root The root node of the trie.
         */
        explicit HamtIterator(std::shared_ptr<HamtNode<K,V,H,E>> root)
            : m_priv(std::make_shared<Priv>(std::move(root)))
        {
            load(m_priv->root, 0);
        }

        /**
         * Returns whether the iterator has another entry available.
         *
         * @return true if another entry is available, otherwise false.
         */
        bool hasNext() const override
        {
            return !m_priv->stack.empty();
        }

        /**
         * Consumes the next entry from the iterator if one is available and places it in `entry`.
         *
         * @param entry Mutable reference which will contain the next entry if one is available.
         * @return true if an entry was consumed, otherwise false.
         */
        bool getNext(HamtEntry<K,V> &entry) override
        {
            TU_LOG_INFO << "-- iterator stack --";
            for (const auto &pair : m_priv->stack) {
                switch (pair.first->getType()) {
                    case HamtNodeType::VALUE: {
                        auto value = std::static_pointer_cast<HamtValueNode<K,V,H,E>>(pair.first);
                        auto e = value->getEntry();
                        TU_LOG_INFO << "  " << e.entryKey() << " -> " << e.entryValue();
                        break;
                    }
                    case HamtNodeType::INDEX: {
                        TU_LOG_INFO << "  index: " << pair.second;
                        break;
                    }
                    default:
                        TU_LOG_INFO << "  ???";
                        break;
                }
            }
            TU_LOG_INFO << "-- end stack --";

            if (m_priv->stack.empty())
                return false;

            auto &curr = m_priv->stack.back();
            auto currNode = std::move(curr.first);
            m_priv->stack.pop_back();

            // top of the stack is always a value node
            TU_ASSERT (currNode->getType() == HamtNodeType::VALUE);
            auto currValue = std::static_pointer_cast<HamtValueNode<K,V,H,E>>(currNode);
            entry = currValue->getEntry();

            // find the next value node
            while (!m_priv->stack.empty()) {
                auto &next = m_priv->stack.back();
                auto &nextNode = next.first;
                int &nextOffset = next.second;
                nextOffset++;

                TU_ASSERT (nextNode->getType() == HamtNodeType::INDEX);
                auto nextIndex = std::static_pointer_cast<HamtIndexNode<K,V,H,E>>(nextNode);
                auto child = findNextChild(nextIndex, nextOffset);

                if (child != nullptr) {
                    load(child, nextOffset);
                    break;
                }

                m_priv->stack.pop_back();
            }

            return true;
        }

    private:
        struct Priv {
            std::shared_ptr<HamtNode<K,V,H,E>> root;
            std::vector<
                std::pair<
                    std::shared_ptr<HamtNode<K,V,H,E>>,
                    int>>
            stack;
        };
        std::shared_ptr<Priv> m_priv;

        static std::shared_ptr<HamtNode<K,V,H,E>>
        findNextChild(std::shared_ptr<HamtIndexNode<K,V,H,E>> node, int &offset)
        {
            for (int i = offset; i < table_slots(kHamtBitsPerLevel); ++i) {
                auto child = node->at(i);
                if (child != nullptr) {
                    offset = i;
                    return child;
                }
            }
            return nullptr;
        }

        void load(std::shared_ptr<HamtNode<K,V,H,E>> node, int offset)
        {
            if (node == nullptr)
                return;
            while (node != nullptr) {
                switch (node->getType()) {
                    case HamtNodeType::VALUE: {
                        m_priv->stack.push_back(std::make_pair(std::move(node), -1));
                        return;
                    }
                    case HamtNodeType::INDEX: {
                        auto index = std::static_pointer_cast<HamtIndexNode<K,V,H,E>>(node);
                        node = findNextChild(index, offset);
                        TU_NOTNULL (node);
                        m_priv->stack.push_back(std::make_pair(index, offset));
                        break;
                    }
                    default:
                        TU_UNREACHABLE();
                }
            }
        }
    };
}

#endif // TEMPO_UTILS_HAMT_ITERATOR_H
