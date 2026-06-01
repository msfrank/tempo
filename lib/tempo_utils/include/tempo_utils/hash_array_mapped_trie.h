#ifndef TEMPO_UTILS_HASH_ARRAY_MAPPED_TRIE_H
#define TEMPO_UTILS_HASH_ARRAY_MAPPED_TRIE_H

#include "hamt_node.h"
#include "log_message.h"

namespace tempo_utils {

    template<
        class KeyType,
        class ValueType,
        class Hash = std::hash<KeyType>,
        class KeyEqual = std::equal_to<KeyType>
    >
    class HashArrayMappedTrie {

    public:
        HashArrayMappedTrie() = default;

        explicit HashArrayMappedTrie(std::shared_ptr<HamtNode<KeyType,ValueType,Hash,KeyEqual>> root)
            : m_root(std::move(root))
        {
            TU_NOTNULL (m_root);
        }

        explicit HashArrayMappedTrie(const std::vector<std::pair<KeyType,ValueType>> &entries)
        {
            auto root = HamtIndexNode<KeyType,ValueType,Hash,KeyEqual>::create();
            for (const auto &entry : entries) {
                KeyHash hash(Hash()(entry.first));
                root->insert(entry.first, entry.second, hash);
            }
            m_root = std::move(root);
        }

        HashArrayMappedTrie(std::initializer_list<std::pair<KeyType,ValueType>> init)
        {
            auto root = HamtIndexNode<KeyType,ValueType,Hash,KeyEqual>::create();
            for (const auto &entry : init) {
                KeyHash hash(Hash()(entry.first));
                root->insert(entry.first, entry.second, hash);
            }
            m_root = std::move(root);
        }

        template<class InputIt>
        static HashArrayMappedTrie<KeyType,ValueType> fromMap(InputIt begin, InputIt end)
        {
            auto root = HamtIndexNode<KeyType,ValueType,Hash,KeyEqual>::create();
            for (auto it = begin; it != end; ++it) {
                KeyHash hash(Hash()(it->first));
                root->insert(it->first, it->second, hash);
            }
            return HashArrayMappedTrie(root);
        }

        template<class InputIt>
        static HashArrayMappedTrie<KeyType,ValueType> fromPairs(InputIt begin, InputIt end)
        {
            auto root = HamtIndexNode<KeyType,ValueType,Hash,KeyEqual>::create();
            for (auto it = begin; it != end; ++it) {
                const auto &pair = *it;
                KeyHash hash(Hash()(pair.first));
                root->insert(pair.first, pair.second, hash);
            }
            return HashArrayMappedTrie(root);
        }

        static HashArrayMappedTrie<KeyType,ValueType> of(KeyType key, ValueType value)
        {
            auto hash = Hash()(key);
            auto root = HamtValueNode<KeyType,ValueType,Hash,KeyEqual>::create(key, value, hash);
            return HashArrayMappedTrie(root);
        }

        /**
         * Returns whether the trie is empty.
         *
         * @return true if the trie is empty, otherwise false.
         */
        bool isEmpty() const
        {
            return m_root == nullptr;
        }

        /**
         * Returns the total number of entries in the trie.
         *
         * @return The number of entries.
         */
        size_t numEntries() const
        {
            if (m_root == nullptr)
                return 0;
            return m_root->numEntries();
        }

        /**
         * Returns whether an entry with the specified `key` is present in the trie.
         *
         * @param key The entry key.
         * @return true if the trie contains the entry, otherwise false.
         */
        bool contains(const KeyType &key)
        {
            auto entry = get(key);
            return entry.isValid();
        }

        /**
         * Returns the entry with the specified `key` in the trie. If no such key is present in the trie then
         * the entry is invalid.
         *
         * @param key
         * @return The entry corresponding to the key in the trie, or an invalid HamtEntry if the key is not present.
         */
        HamtEntry<KeyType,ValueType> get(const KeyType &key)
        {
            if (m_root == nullptr)
                return {};
            KeyHash hash(Hash()(key));
            return m_root->find(key, hash);
        }

        /**
         *
         * @param key
         * @return
         */
        const ValueType& valueAt(const KeyType &key)
        {
            auto node = find(key);
            return node->entryValue();
        }


    private:
        std::shared_ptr<HamtNode<KeyType,ValueType,Hash,KeyEqual>> m_root;

    };
}

#endif // TEMPO_UTILS_HASH_ARRAY_MAPPED_TRIE_H