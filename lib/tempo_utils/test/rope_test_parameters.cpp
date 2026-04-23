
#include "rope_test_parameters.h"

#include "tempo_config/config_types.h"
#include "tempo_config/config_utils.h"

// forward declarations
static tempo_utils::SharedRopeNode<std::string> parse_concat_rope_node(
    const tempo_config::ConfigMap &mapNode,
    std::vector<std::vector<std::string>> &totalChunks,
    std::vector<std::string> &totalElements);
static tempo_utils::SharedRopeNode<std::string> parse_leaf_rope_node(
    const tempo_config::ConfigSeq &seqNode,
    std::vector<std::vector<std::string>> &totalChunks,
    std::vector<std::string> &totalElements);

static tempo_utils::SharedRopeNode<std::string> parse_rope_node(
    const tempo_config::ConfigNode &node,
    std::vector<std::vector<std::string>> &totalChunks,
    std::vector<std::string> &totalElements)
{
    switch (node.getNodeType()) {
    case tempo_config::ConfigNodeType::kSeq:
        return parse_leaf_rope_node(node.toSeq(), totalChunks, totalElements);
    case tempo_config::ConfigNodeType::kMap:
        return parse_concat_rope_node(node.toMap(), totalChunks, totalElements);
    default:
        TU_UNREACHABLE();
    }
}

static tempo_utils::SharedRopeNode<std::string> parse_concat_rope_node(
    const tempo_config::ConfigMap &mapNode,
    std::vector<std::vector<std::string>> &totalChunks,
    std::vector<std::string> &totalElements)
{
    auto l = mapNode.mapAt("l");
    auto leftNode = parse_rope_node(l, totalChunks, totalElements);
    auto r = mapNode.mapAt("r");
    auto rightNode = parse_rope_node(r, totalChunks, totalElements);
    return std::make_shared<tempo_utils::ConcatRopeNode<std::string>>(leftNode, rightNode);
}

static tempo_utils::SharedRopeNode<std::string> parse_leaf_rope_node(
    const tempo_config::ConfigSeq &seqNode,
    std::vector<std::vector<std::string>> &totalChunks,
    std::vector<std::string> &totalElements)
{
    std::vector<std::string> elements;
    for (auto it = seqNode.seqBegin(); it != seqNode.seqEnd(); ++it) {
        const auto &child = *it;
        TU_ASSERT (child.getNodeType() == tempo_config::ConfigNodeType::kValue);
        elements.push_back(child.toString());
        totalElements.push_back(child.toString());
    }
    totalChunks.emplace_back(elements.begin(), elements.end());
    return std::make_shared<tempo_utils::LeafRopeNode<std::string>>(elements.begin(), elements.end());
}

static void parse_rope_from_config(
    std::string_view utf8,
    RopeTestParameter &p)
{
    tempo_config::ConfigNode rootNode;
    TU_ASSIGN_OR_RAISE (rootNode, tempo_config::read_config_string(utf8));
    p.ropeNode = parse_rope_node(rootNode, p.totalChunks, p.totalElements);
    p.rope = tempo_utils::Rope<std::string>(p.ropeNode);
}

RopeTestParameter empty_rope()
{
    RopeTestParameter p{"EmptyRope"};
    return p;

}

RopeTestParameter
leaf_node_containing_1_element()
{
    RopeTestParameter p{"LeafNodeContainingOneElement"};
    parse_rope_from_config(R"([1])", p);
    return p;
}

RopeTestParameter
leaf_node_containing_5_elements()
{
    RopeTestParameter p{"LeafNodeContainingFiveElements"};
    parse_rope_from_config(R"([1, 2, 3, 4, 5])", p);
    return p;
}

RopeTestParameter
concat_of_leaf_nodes()
{
    RopeTestParameter p{"ConcatOfLeafNodes"};
    parse_rope_from_config(R"(
{
    "l": [1, 2, 3],
    "r": [4, 5, 6],
}
    )", p);
    return p;
}

RopeTestParameter
concat_of_two_concats_of_leaf_nodes()
{
    RopeTestParameter p{"ConcatOfTwoConcatsOfLeafNodes"};
    parse_rope_from_config(R"(
{
    "l": {
        "l": [1, 2, 3],
        "r": [4, 5, 6],
    },
    "r": {
        "l": [7, 8, 9],
        "r": [10, 11, 12],
    }
}
    )", p);
    return p;
}
