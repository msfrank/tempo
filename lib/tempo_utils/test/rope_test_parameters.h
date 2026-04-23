#ifndef ROPE_TEST_PARAMETERS_H
#define ROPE_TEST_PARAMETERS_H

#include <string>

#include <tempo_utils/rope.h>

struct RopeTestParameter {
    std::string paramName;
    tempo_utils::SharedRopeNode<std::string> ropeNode;
    tempo_utils::Rope<std::string> rope;
    std::vector<std::vector<std::string>> totalChunks;
    std::vector<std::string> totalElements;
};

RopeTestParameter empty_rope();

RopeTestParameter leaf_node_containing_1_element();

RopeTestParameter leaf_node_containing_5_elements();

RopeTestParameter concat_of_leaf_nodes();

RopeTestParameter concat_of_two_concats_of_leaf_nodes();

#endif // ROPE_TEST_PARAMETERS_H
