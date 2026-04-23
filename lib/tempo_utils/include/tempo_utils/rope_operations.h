#ifndef TEMPO_UTILS_ROPE_OPERATIONS_H
#define TEMPO_UTILS_ROPE_OPERATIONS_H

#include <memory>
#include <span>
#include <vector>

#include "log_message.h"
#include "rope_chunk.h"

namespace tempo_utils {

    /**
     *
     */
    class RopeOperations {
    public:
        virtual ~RopeOperations() = default;

        virtual size_t getWeight() const = 0;
        virtual size_t getSize() const = 0;
        virtual size_t getDepth() const = 0;

        bool isBalanced() const;
    };

    /**
     * The type of Rope node.
     */
    enum class RopeNodeType {
        INVALID,                /**< Invalid node. */
        LEAF,                   /**< Leaf node. */
        CONCAT,                 /**< Concat node. */
    };

    // forward declarations for node classes
    template<class ElementType> class RopeNode;
    template<class ElementType> class ConcatRopeNode;
    template<class ElementType> class LeafRopeNode;

    // generic type alias for shared ptr to a rope node
    template<class ElementType>
    using SharedRopeNode = std::shared_ptr<RopeNode<ElementType>>;

    // generic type alias for shared ptr to a rope node
    template<class ElementType>
    using SharedRopeNodeSplit = std::pair<SharedRopeNode<ElementType>,SharedRopeNode<ElementType>>;

    // forward declaration for collect_leaves
    template<class ElementType>
    void collect_leaves(
        SharedRopeNode<ElementType> rope,
        std::vector<std::shared_ptr<LeafRopeNode<ElementType>>> &leaves);

    /**
     *
     * @tparam ElementType
     */
    template<typename ElementType>
    class RopeNode : public RopeOperations {
    public:
        explicit RopeNode(RopeNodeType type)
            : m_type(type)
        {
            TU_ASSERT (m_type != RopeNodeType::INVALID);
        }

        virtual ~RopeNode() = default;

        RopeNodeType getType() const { return m_type; }

        virtual const ElementType& elementAt(size_t index) const = 0;

        virtual SharedRopeNode<ElementType> splitAt(
            size_t index,
            std::vector<std::shared_ptr<LeafRopeNode<ElementType>>> &leaves) = 0;

    private:
        RopeNodeType m_type;
    };

    /**
     *
     * @tparam ElementType
     */
    template<typename ElementType>
    class LeafRopeNode : public RopeNode<ElementType> {
    public:
        LeafRopeNode() : RopeNode<ElementType>(RopeNodeType::LEAF), m_chunk() {}

        explicit LeafRopeNode(RopeChunk<ElementType> chunk)
            : RopeNode<ElementType>(RopeNodeType::LEAF),
              m_chunk(std::move(chunk))
        {
        }

        template<class InputIt>
        LeafRopeNode(InputIt begin, InputIt end)
            : RopeNode<ElementType>(RopeNodeType::LEAF),
              m_chunk(begin, end)
        {
        }

        LeafRopeNode(std::initializer_list<ElementType> init) : LeafRopeNode(init.begin(), init.end()) {}

        RopeChunk<ElementType> getChunk() const { return m_chunk; }

        size_t getWeight() const override { return m_chunk.numElements(); }

        size_t getSize() const override { return m_chunk.numElements(); }

        size_t getDepth() const override { return 0; }

        const ElementType& elementAt(size_t index) const override { return m_chunk.elementAt(index); }

        SharedRopeNode<ElementType> splitAt(
            size_t index,
            std::vector<std::shared_ptr<LeafRopeNode<ElementType>>> &leaves) override
        {
            if (index == 0) {
                leaves.push_back(std::make_shared<LeafRopeNode<ElementType>>(m_chunk));
                return std::make_shared<LeafRopeNode<ElementType>>();
            }
            if (m_chunk.numElements() <= index) {
                return std::make_shared<LeafRopeNode<ElementType>>(m_chunk);
            }
            auto split = m_chunk.split(index);
            leaves.push_back(std::make_shared<LeafRopeNode>(split.second));
            return std::make_shared<LeafRopeNode>(split.first);
        }

    private:
        RopeChunk<ElementType> m_chunk;
    };

    /**
     *
     * @tparam ElementType
     */
    template<typename ElementType>
    class ConcatRopeNode : public RopeNode<ElementType> {
    public:
        ConcatRopeNode(
            std::shared_ptr<RopeNode<ElementType>> left,
            std::shared_ptr<RopeNode<ElementType>> right)
            : RopeNode<ElementType>(RopeNodeType::CONCAT),
              m_left(std::move(left)),
              m_right(std::move(right))
        {
            TU_NOTNULL (m_left);
            TU_NOTNULL (m_right);
            m_weight = calculateWeight(m_left);
        }

        std::shared_ptr<RopeNode<ElementType>> getLeftNode() const
        {
            return m_left;
        }

        std::shared_ptr<RopeNode<ElementType>> getRightNode() const
        {
            return m_right;
        }

        size_t getWeight() const override { return m_weight; }

        size_t getSize() const override { return m_left->getSize() + m_right->getSize(); }

        size_t getDepth() const override { return 1 + std::max(m_left->getDepth(), m_right->getDepth()); }

        const ElementType& elementAt(size_t index) const override
        {
            if (index < m_weight)
                return m_left->elementAt(index);
            return m_right->elementAt(index - m_weight);
        }

        SharedRopeNode<ElementType> splitAt(
            size_t index,
            std::vector<std::shared_ptr<LeafRopeNode<ElementType>>> &leaves) override
        {
            if (index < m_weight) {
                auto left = m_left->splitAt(index, leaves);
                collect_leaves(m_right, leaves);
                return left;
            }

            auto right = m_right->splitAt(index - m_weight, leaves);
            return std::make_shared<ConcatRopeNode>(m_left, right);
        }

    private:
        std::shared_ptr<RopeNode<ElementType>> m_left;
        std::shared_ptr<RopeNode<ElementType>> m_right;
        size_t m_weight;

        size_t calculateWeight(const std::shared_ptr<RopeNode<ElementType>> &child) const
        {
            switch (child->getType()) {
            case RopeNodeType::LEAF: {
                auto leaf = std::static_pointer_cast<LeafRopeNode<ElementType>>(child);
                return leaf->getSize();
            }
            case RopeNodeType::CONCAT: {
                auto concat = std::static_pointer_cast<ConcatRopeNode<ElementType>>(child);
                return calculateWeight(concat->getLeftNode()) + calculateWeight(concat->getRightNode());
            }
            default:
                return 0;
            }
        }
    };

    /**
     *
     * @tparam ElementType
     * @param elements
     * @return
     */
    template<class ElementType>
    SharedRopeNode<ElementType> make_rope(std::vector<ElementType> &&elements)
    {
        RopeChunk<ElementType> vec(std::move(elements));
        auto node = std::make_shared<LeafRopeNode<ElementType>>(std::move(vec));
        return std::static_pointer_cast<RopeNode<ElementType>>(node);
    }

    /**
     *
     * @tparam ElementType
     * @param element
     * @return
     */
    template<class ElementType>
    SharedRopeNode<ElementType> make_rope(ElementType element)
    {
        std::vector<ElementType> elements;
        elements.push_back(std::move(element));
        return make_rope(std::move(elements));
    }

    /**
     *
     * @tparam ElementType
     * @param r1
     * @param r2
     * @return
     */
    template<class ElementType>
    SharedRopeNode<ElementType> concat_rope(SharedRopeNode<ElementType> r1, SharedRopeNode<ElementType> r2)
    {
        TU_NOTNULL (r1);
        TU_NOTNULL (r2);
        auto node = std::make_shared<ConcatRopeNode<ElementType>>(r1, r2);
        return std::static_pointer_cast<RopeNode<ElementType>>(node);
    }

    /**
     *
     * @tparam ElementType
     * @param rope
     * @param begin
     * @param end
     * @param offset
     * @param nread
     * @param leaves
     */
    template<class ElementType>
    void collect_leaves(
        SharedRopeNode<ElementType> rope,
        const size_t begin,
        const size_t end,
        size_t &offset,
        size_t &nread,
        std::vector<std::shared_ptr<LeafRopeNode<ElementType>>> &leaves)
    {
        TU_NOTNULL (rope);
        TU_ASSERT (begin <= end);

        switch (rope->getType()) {

            case RopeNodeType::LEAF: {
                auto leaf = std::static_pointer_cast<LeafRopeNode<ElementType>>(rope);
                auto size = leaf->getSize();

                // leaf chunk is past the collection interval
                if (offset >= end)
                    return;
                // leaf is prior to the collection interval
                if (offset + size <= begin)
                    return;

                //
                // count is the number of elements remaining to collect
                auto count = end - (begin + nread);

                // if leaf is larger than the number of elements needed, then we construct a new leaf from
                // a subspan, otherwise we collect the entire leaf. size is then updated with the number of
                // elements collected.
                if (offset < begin || count < size) {
                    auto chunk = leaf->getChunk();
                    auto subspan = chunk.subspan(offset < begin? begin - offset : 0, std::min(count, size));
                    auto node = std::make_shared<LeafRopeNode<ElementType>>(subspan.begin(), subspan.end());
                    leaves.push_back(std::move(node));
                    nread += subspan.size();
                } else {
                    leaves.push_back(std::move(leaf));
                    nread += size;
                }

                offset += size;
                return;
            }

            case RopeNodeType::CONCAT: {
                auto concat = std::static_pointer_cast<ConcatRopeNode<ElementType>>(rope);
                auto weight = concat->getWeight();
                if (std::max(begin, offset) < weight + offset) {
                    collect_leaves(concat->getLeftNode(), begin, end, offset, nread, leaves);
                } else {
                    offset += weight;
                }
                if (nread == end - begin)
                    return;
                collect_leaves(concat->getRightNode(), begin, end, offset, nread, leaves);
                return;
            }

            default:
                TU_UNREACHABLE();
        }
    }

    /**
     *
     * @tparam ElementType
     * @param rope
     * @param begin
     * @param end
     * @param leaves
     */
    template<class ElementType>
    void collect_leaves(
        SharedRopeNode<ElementType> rope,
        size_t begin,
        size_t end,
        std::vector<std::shared_ptr<LeafRopeNode<ElementType>>> &leaves)
    {
        size_t offset = 0;
        size_t nread = 0;
        collect_leaves(rope, begin, end, offset, nread, leaves);
    }

    /**
     *
     * @tparam ElementType
     * @param rope
     * @param leaves
     */
    template<class ElementType>
    void collect_leaves(
        SharedRopeNode<ElementType> rope,
        std::vector<std::shared_ptr<LeafRopeNode<ElementType>>> &leaves)
    {
        collect_leaves(rope, 0, std::numeric_limits<size_t>::max(), leaves);
    }

    /**
     *
     * @tparam ElementType
     * @param leaves
     * @param begin
     * @param end
     * @return
     */
    template<class ElementType>
    SharedRopeNode<ElementType> merge_leaves(
        const std::vector<std::shared_ptr<LeafRopeNode<ElementType>>> &leaves,
        size_t begin,
        size_t end)
    {
        TU_ASSERT (begin < end && end <= leaves.size());
        auto range = end - begin;
        switch (range) {
            case 1: return leaves.at(begin);
            case 2: return std::make_shared<ConcatRopeNode<ElementType>>(leaves.at(begin), leaves.at(begin + 1));
            default: {
                auto mid = begin + (range / 2);
                auto left = merge_leaves(leaves, begin, mid);
                auto right = merge_leaves(leaves, mid, end);
                return std::make_shared<ConcatRopeNode<ElementType>>(left, right);
            }
        }
    }

    /**
     *
     * @tparam ElementType
     * @param leaves
     * @return
     */
    template<class ElementType>
    SharedRopeNode<ElementType> merge_leaves(const std::vector<std::shared_ptr<LeafRopeNode<ElementType>>> &leaves)
    {
        return merge_leaves(leaves, 0, leaves.size());
    }

    /**
     *
     * @tparam ElementType
     * @param rope
     * @return
     */
    template<class ElementType>
    SharedRopeNode<ElementType> rebalance_rope(SharedRopeNode<ElementType> rope)
    {
        TU_NOTNULL (rope);
        std::vector<std::shared_ptr<LeafRopeNode<ElementType>>> leaves;
        collect_leaves(rope, leaves);
        return merge_leaves(leaves);
    }

    /**
     *
     * @tparam ElementType
     * @param rope
     * @param index
     * @return
     */
    template<class ElementType>
    SharedRopeNodeSplit<ElementType> split_rope(SharedRopeNode<ElementType> rope, size_t index)
    {
        TU_NOTNULL (rope);
        std::vector<std::shared_ptr<LeafRopeNode<ElementType>>> leaves;
        auto lhs = rope->splitAt(index, leaves);
        auto rhs = merge_leaves(leaves);
        return SharedRopeNodeSplit<ElementType>(lhs, rhs);
    }

}

#endif // TEMPO_UTILS_ROPE_OPERATIONS_H
