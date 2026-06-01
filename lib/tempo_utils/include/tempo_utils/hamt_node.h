#ifndef TEMPO_UTILS_HAMT_NODE_H
#define TEMPO_UTILS_HAMT_NODE_H

#include <memory>

#include "hashing.h"
#include "integer_types.h"
#include "log_message.h"
#include "log_stream.h"
#include "log_helpers.h"

namespace tempo_utils {

    constexpr int kHamtBitsPerLevel = 4;

    constexpr int table_slots(int bitsPerLevel) {
        int numSlots = 1;
        for (int i = 0; i < bitsPerLevel; i++) {
            numSlots *= 2;
        }
        return numSlots;
    }

    // forward declarations
    template<class KeyType, class ValueType, class Hash, class KeyEqual>
    class HamtNode;
    template<class KeyType, class ValueType, class Hash, class KeyEqual>
    class HamtValueNode;
    template<class KeyType, class ValueType, class Hash, class KeyEqual>
    class HamtIndexNode;

    /**
     * KeyHash encapsulates the state used when searching and modifying the trie.
     */
    class KeyHash
    {
    public:
        explicit KeyHash(size_t init, tu_uint32 level = 0)
            : m_init(init), m_curr(init), m_level(level)
        {
            for (tu_uint32 i = 0; i < level; ++i) {
                TU_ASSERT (m_bits >= kHamtBitsPerLevel);
                m_bits -= kHamtBitsPerLevel;
                m_curr <<= kHamtBitsPerLevel;
            }
        }

        KeyHash(const KeyHash &other)
            : m_init(other.m_init), m_curr(other.m_curr), m_bits(other.m_bits), m_level(other.m_level)
        {
        }

        size_t getInit() const { return m_init; }
        size_t getHash() const { return m_curr; }
        tu_uint32 getIndex() const { return m_curr >> (kHashBits - kHamtBitsPerLevel); }
        tu_uint8 getBitsRemaining() const { return m_bits; }
        tu_uint32 getLevel() const { return m_level; }

        KeyHash next() const
        {
            TU_ASSERT (m_bits > 0);
            auto curr = m_curr << kHamtBitsPerLevel;
            auto bits = m_bits - kHamtBitsPerLevel;
            auto level = m_level + 1;
            return KeyHash(m_init, curr, bits, level);
        }

        bool needsRehash() const { return m_bits < kHamtBitsPerLevel; }

        KeyHash rehash(size_t init) const {
            TU_ASSERT (needsRehash());
            return KeyHash(init, init, kHashBits, m_level + 1);
        }

        std::string toString() const
        {
            std::bitset<kHashBits> bits(m_curr);
            return bits.to_string();
        }

    private:
        size_t m_init;
        size_t m_curr;
        tu_uint8 m_bits = kHashBits;
        tu_uint32 m_level;

        KeyHash(size_t init, size_t curr, tu_uint8 bits, tu_uint32 level)
            : m_init(init), m_curr(curr), m_bits(bits), m_level(level)
        {
        }
    };

    enum class HamtNodeType {
        VALUE,
        INDEX,
        CHAIN,
    };

    /**
     * Immutable value class containing an entry key-value pair.
     *
     * @tparam KeyType The key type.
     * @tparam ValueType The value type.
     */
    template<class KeyType, class ValueType>
    class HamtEntry {
    public:
        HamtEntry() = default;
        HamtEntry(const KeyType &key, const ValueType &value)
            : m_entry(std::make_shared<std::pair<KeyType,ValueType>>(std::make_pair(key, value)))
        {
        }

        bool isValid() const { return m_entry != nullptr; }
        const std::pair<KeyType,ValueType>& entryPair() const { return *m_entry; }
        const KeyType& entryKey() const { return m_entry->first; }
        const ValueType& entryValue() const { return m_entry->second; }

    private:
        std::shared_ptr<const std::pair<KeyType,ValueType>> m_entry;
    };

    /**
     * Fixed size array containing node pointers.
     */
    template<class KeyType, class ValueType, class Hash, class KeyEqual>
    using HamtIndexTable = std::array<
        std::shared_ptr<HamtNode<KeyType,ValueType,Hash,KeyEqual>>,
        table_slots(kHamtBitsPerLevel)>;

    /**
     * The HAMT base node.
     *
     * @tparam KeyType The key type.
     * @tparam ValueType The value type.
     * @tparam Hash The hash functor type.
     * @tparam Hash The key equals functor type.
     */
    template<class KeyType, class ValueType, class Hash, class KeyEqual>
    class HamtNode {
    public:
        explicit HamtNode(HamtNodeType type): m_type(type) {}
        virtual ~HamtNode() = default;

        HamtNodeType getType() const { return m_type; }

        virtual HamtEntry<KeyType,ValueType> find(const KeyType &key, const KeyHash &hash) const = 0;

        virtual size_t numEntries() const = 0;

    protected:
        static bool isEqual(const KeyType &lhs, const KeyType &rhs)
        {
            return KeyEqual()(lhs, rhs);
        }

    private:
        HamtNodeType m_type;
    };

    /**
     * A HAMT leaf node which contains the entry and hash.
     *
     * @tparam KeyType
     * @tparam ValueType
     */
    template<class KeyType, class ValueType, class Hash, class KeyEqual>
    class HamtValueNode : public HamtNode<KeyType,ValueType,Hash,KeyEqual> {

        struct Private { explicit Private() = default; };

    public:
        HamtValueNode(HamtEntry<KeyType,ValueType> entry, size_t hash, Private)
            : HamtNode<KeyType,ValueType,Hash,KeyEqual>(HamtNodeType::VALUE),
              m_entry(std::move(entry)),
              m_hash(hash)
        {
            TU_ASSERT (m_entry.isValid());
        }

        static std::shared_ptr<HamtValueNode> create(HamtEntry<KeyType,ValueType> entry, size_t hash)
        {
            return std::make_shared<HamtValueNode>(entry, hash, Private{});
        }

        static std::shared_ptr<HamtValueNode> create(KeyType key, ValueType value, size_t hash)
        {
            return std::make_shared<HamtValueNode>(HamtEntry(key, value), hash, Private{});
        }

        HamtEntry<KeyType,ValueType> find(const KeyType &key, const KeyHash &hash) const override
        {
            if (m_hash == hash.getInit() && KeyEqual()(key, m_entry.entryKey())) {
                TU_LOG_INFO << "found entry for key " << m_entry.entryKey()
                            << " with hash " << Bin((tu_uint64) m_hash);
                return m_entry;
            }
            TU_LOG_INFO << "entry not found for key " << key;
            return {};
        }

        size_t numEntries() const override { return 1; }

        HamtEntry<KeyType,ValueType> getEntry() const { return m_entry; }
        size_t getHash() const { return m_hash; }

        const std::pair<KeyType,ValueType>& getPair() const { return m_entry.entryPair(); }
        const KeyType& entryKey() const { return m_entry.entryKey(); }
        const ValueType& entryValue() const { return m_entry.entryValue(); }

    private:
        HamtEntry<KeyType,ValueType> m_entry;
        size_t m_hash;
    };

    /**
     * A HAMT intermediate node which contains a table of pointers to child nodes.
     *
     * @tparam KeyType The key type.
     * @tparam ValueType The value type.
     */
    template<class KeyType, class ValueType, class Hash, class KeyEqual>
    class HamtIndexNode : public HamtNode<KeyType,ValueType,Hash,KeyEqual> {

        struct Private {};

    public:
        explicit HamtIndexNode(Private)
            : HamtNode<KeyType,ValueType,Hash,KeyEqual>(HamtNodeType::INDEX)
        {
        }
        HamtIndexNode(HamtIndexTable<KeyType,ValueType,Hash,KeyEqual> table, Private)
            : HamtNode<KeyType,ValueType,Hash,KeyEqual>(HamtNodeType::INDEX),
              m_table(std::move(table))
        {
        }

        /**
         * Create a new empty index node.
         *
         * @return shared ptr containing the new index node.
         */
        static std::shared_ptr<HamtIndexNode<KeyType,ValueType,Hash,KeyEqual>> create()
        {
            return std::make_shared<HamtIndexNode>(Private{});
        }

        /**
         * Create a new index node with the specified `table`.
         *
         * @param table The index table.
         * @return shared ptr containing the new index node.
         */
        static std::shared_ptr<HamtIndexNode<KeyType,ValueType,Hash,KeyEqual>> create(
            HamtIndexTable<KeyType,ValueType,Hash,KeyEqual> table)
        {
            return std::make_shared<HamtIndexNode>(table, Private{});
        }

        /**
         *
         * @param key
         * @param hash
         * @return
         */
        HamtEntry<KeyType,ValueType> find(const KeyType &key, const KeyHash &hash) const override
        {
            TU_ASSERT (!hash.needsRehash());

            TU_LOG_INFO << "searching for key " << key
                        << " hash=" << hash.toString()
                        << " bitsRemaining=" << hash.getBitsRemaining()
                        << " level=" << hash.getLevel();

            tu_uint32 index = hash.getIndex();
            auto child = m_table.at(index);

            // if table element at index is nullptr then key is not present in map
            if (child == nullptr) {
                TU_LOG_INFO << "entry not found for key " << key << " at index " << index;
                return {};
            }

            // otherwise continue search in child node
            TU_LOG_INFO << "continuing search at index " << index;
            return child->find(key, hash.next());
        }

        /**
         *
         * @return
         */
        size_t numEntries() const override
        {
            size_t total = 0;
            for (const auto &child : m_table) {
                if (child != nullptr) {
                    total += child->numEntries();
                }
            }
            return total;
        }

        /**
         *
         * @param key
         * @param value
         * @param hash
         */
        void insert(
            const KeyType &key,
            const ValueType &value,
            const KeyHash &hash)
        {
            TU_ASSERT (!hash.needsRehash());

            size_t childIndex = hash.getIndex();
            auto child = m_table[childIndex];

            // case 1: table value at index is nullptr, so key is not present in map
            if (child == nullptr) {
                TU_LOG_INFO << "inserting entry for key " << key
                            << " at index " << (tu_uint8) childIndex
                            << " hash=" << Bin(static_cast<tu_uint64>(hash.getInit()))
                            << " level=" << hash.getLevel();
                m_table[childIndex] = HamtValueNode<KeyType,ValueType,Hash,KeyEqual>::create(key, value, hash.getInit());
                return;
            }

            switch (child->getType()) {

                // case 2: table value at index is a value node
                case HamtNodeType::VALUE: {
                    auto childNode = std::static_pointer_cast<HamtValueNode<KeyType,ValueType,Hash,KeyEqual>>(child);
                    if (HamtNode<KeyType,ValueType,Hash,KeyEqual>::isEqual(childNode->entryKey(), key)) {
                        TU_LOG_INFO << "replacing entry for key " << key
                                    << " at index " << (tu_uint8) childIndex
                                    << " hash=" << Bin(static_cast<tu_uint64>(hash.getInit()))
                                    << " level=" << hash.getLevel();
                        // if the existing value node has the same key, then replace the value and hash
                        m_table[childIndex] = HamtValueNode<KeyType,ValueType,Hash,KeyEqual>::create(key, value, hash.getInit());
                    } else {
                        // otherwise replace the slot with an index node containing both the existing key and the new key
                        auto existing = std::static_pointer_cast<HamtValueNode<KeyType,ValueType,Hash,KeyEqual>>(child);
                        TU_LOG_INFO << "splitting entry at index " << (tu_uint8) childIndex
                                    << " level=" << hash.getLevel();
                        m_table[childIndex] = hamt_split(existing, key, value, hash.next());
                    }
                    return;
                }
                // case 3: table value at index is an index node
                case HamtNodeType::INDEX: {
                    auto childNode = std::static_pointer_cast<HamtIndexNode>(child);
                        TU_LOG_INFO << "continuing insert for key " << key << " at index " << (tu_uint8) childIndex;
                        childNode->insert(key, value, hash.next());
                    return;
                }

                default:
                    TU_UNREACHABLE();
            }
        }

        /**
         *
         * @param key
         * @param value
         * @param hash
         * @return
         */
        std::shared_ptr<HamtNode<KeyType,ValueType,Hash,KeyEqual>> update(
            const KeyType &key,
            const ValueType &value,
            const KeyHash &hash)
        {
            TU_ASSERT (!hash.needsRehash());

            tu_uint32 index = hash.getIndex();
            HamtIndexTable<KeyType,ValueType,Hash,KeyEqual> table;

            // copy all slots into updated node except the node at index
            for (tu_uint32 i = 0; i < m_table.size(); i++) {
                if (i != index) {
                    auto node = m_table[i];
                    table[i] = node;
                }
            }
            auto child = m_table.at(index);

            // case 1: table value at index is nullptr, so key is not present in trie
            if (child == nullptr) {
                auto added = HamtValueNode<KeyType,ValueType,Hash,KeyEqual>::create(key, value, hash.getInit());
                table[index] = added;
                return HamtIndexNode::create(std::move(table));
            }

            switch (child->getType()) {

                // case 2: table value at index is a value node
                case HamtNodeType::VALUE: {
                    auto existing = std::static_pointer_cast<HamtValueNode<KeyType,ValueType,Hash,KeyEqual>>(child);
                    if (HamtNode<KeyType,ValueType,Hash,KeyEqual>::isEqual(existing->entryKey(), key)) {
                        TU_LOG_INFO << "updating entry for key " << key
                                    << " at index " << (tu_uint8) index
                                    << " hash=" << Bin(static_cast<tu_uint64>(hash.getInit()))
                                    << " level=" << hash.getLevel();
                        table[index] = HamtValueNode<KeyType,ValueType,Hash,KeyEqual>::create(key, value, hash.getInit());
                    } else {
                        auto added = HamtIndexNode::create();
                        KeyHash existingHash(existing->getHash(), hash.getLevel());
                        auto entry = existing->getEntry();
                        added->insert(entry.entryKey(), entry.entryValue(), existingHash);
                        added->insert(key, value, hash);
                    }
                    return HamtIndexNode::create(std::move(table));
                }

                // case 3: table value at index is an IndexMapNode
                case HamtNodeType::INDEX: {
                        auto existing = std::static_pointer_cast<HamtIndexNode>(child);
                        return existing->update(key, value, hash.next());
                }

                default:
                    TU_UNREACHABLE();
            }
        }

    private:
        HamtIndexTable<KeyType,ValueType,Hash,KeyEqual> m_table;
        size_t m_total = 0;
    };

    /**
     *
     * @tparam KeyType
     * @tparam ValueType
     */
    template<class KeyType, class ValueType, class Hash, class KeyEqual>
    class HamtChainNode : public HamtNode<KeyType,ValueType,Hash,KeyEqual> {
    public:
        HamtChainNode()
            : HamtNode<KeyType,ValueType,Hash,KeyEqual>(HamtNodeType::CHAIN)
        {
        }

    private:
        KeyType m_key;
        ValueType m_value;
        std::shared_ptr<HamtChainNode> m_next;
    };

    /**
     *
     * @param existing
     * @param key
     * @param value
     * @param hash
     * @return
     */
    template<class KeyType, class ValueType, class Hash, class KeyEqual>
    std::shared_ptr<HamtIndexNode<KeyType,ValueType,Hash,KeyEqual>> hamt_split(
        std::shared_ptr<HamtValueNode<KeyType,ValueType,Hash,KeyEqual>> existing,
        const KeyType &key,
        const ValueType &value,
        const KeyHash &hash)
    {
        TU_ASSERT (!hash.needsRehash());

        KeyHash existingHash(existing->getHash(), hash.getLevel());
        HamtIndexTable<KeyType,ValueType,Hash,KeyEqual> table;

        if (hash.getIndex() != existingHash.getIndex()) {
            TU_LOG_INFO << "reinserting entry for existing key " << existing->getEntry().entryKey()
                        << " at index " << existingHash.getIndex()
                        << " hash=" << Bin(static_cast<tu_uint64>(existingHash.getInit()))
                        << " level=" << existingHash.getLevel();
            table[existingHash.getIndex()] = existing;
            auto added = HamtValueNode<KeyType,ValueType,Hash,KeyEqual>::create(key, value, hash.getInit());
            TU_LOG_INFO << "inserting entry for key " << key
                        << " at index " << hash.getIndex()
                        << " hash=" << Bin(static_cast<tu_uint64>(hash.getInit()))
                        << " level=" << hash.getLevel();
            table[hash.getIndex()] = added;
            return HamtIndexNode<KeyType,ValueType,Hash,KeyEqual>::create(std::move(table));
        }

        TU_LOG_INFO << "splitting entry at index " << hash.getIndex()
                    << " level=" << hash.getLevel();
        table[hash.getIndex()] = hamt_split(existing, key, value, hash.next());
        return HamtIndexNode<KeyType,ValueType,Hash,KeyEqual>::create(std::move(table));
    }
}

#endif // TEMPO_UTILS_HAMT_NODE_H
