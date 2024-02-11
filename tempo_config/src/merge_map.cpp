
#include <tempo_config/merge_map.h>

static tempo_config::ConfigNode
merge(const tempo_config::ConfigNode &baseNode, const tempo_config::ConfigNode &overrideNode)
{
    // if base node is not a map, then replace base node with override node
    if (baseNode.getNodeType() != tempo_config::ConfigNodeType::kMap)
        return overrideNode;

    // if override node is not a map, then replace base node with override node
    if (overrideNode.getNodeType() != tempo_config::ConfigNodeType::kMap)
        return overrideNode;

    // if override node is an empty map, then return the base map unmodified
    auto overrideMap = overrideNode.toMap();
    if (overrideMap.mapSize() == 0)
        return baseNode;

    auto baseMap = baseNode.toMap();
    absl::flat_hash_map<std::string,tempo_config::ConfigNode> mergedEntries(
        baseMap.mapBegin(), baseMap.mapEnd());

    for (auto iterator = overrideMap.mapBegin(); iterator != overrideMap.mapEnd(); iterator++) {
        const auto &key = iterator->first;
        const auto &value = iterator->second;

        if (mergedEntries.contains(key)) {
            mergedEntries[key] = merge(mergedEntries.at(key), value);
        } else {
            mergedEntries[key] = value;
        }
    }

    return tempo_config::ConfigMap(std::move(mergedEntries));
}

tempo_config::ConfigMap
tempo_config::merge_map(const ConfigMap &baseMap, const ConfigMap &overrideMap)
{
    auto mergedMap = merge(baseMap, overrideMap);
    return mergedMap.toMap();
}
