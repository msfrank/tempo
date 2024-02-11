
#include <absl/strings/str_join.h>

#include <tempo_config/config_result.h>
#include <tempo_config/extend_map.h>

inline std::string path_to_string(const std::vector<std::string> &path)
{
    std::string s("/");
    return s.append(absl::StrJoin(path, "/"));
}

static tempo_utils::Result<tempo_config::ConfigNode>
extend(
    const tempo_config::ConfigNode &baseNode,
    const tempo_config::ConfigNode &overrideNode,
    std::vector<std::string> &currPath)
{
    // if base node is not a map, then replace base node with override node
    if (baseNode.getNodeType() != tempo_config::ConfigNodeType::kMap)
        return tempo_config::ConfigStatus::forCondition(tempo_config::ConfigCondition::kParseError,
            "failed to extend node {}:{}; node must be a map",
            baseNode.getNodeLocation().toString(), path_to_string(currPath));

    // if override node is not a map, then replace base node with override node
    if (overrideNode.getNodeType() != tempo_config::ConfigNodeType::kMap)
        return tempo_config::ConfigStatus::forCondition(tempo_config::ConfigCondition::kParseError,
            "failed to extend node {}:{}; override node must be a map",
            baseNode.getNodeLocation().toString(), path_to_string(currPath));

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
            currPath.push_back(key);
            auto extendResult = extend(mergedEntries.at(key), value, currPath);
            if (extendResult.isStatus())
                return extendResult.getStatus();
            mergedEntries[key] = extendResult.getResult();
            currPath.pop_back();
        } else {
            mergedEntries[key] = value;
        }
    }

    return tempo_config::ConfigMap(std::move(mergedEntries));
}

tempo_utils::Result<tempo_config::ConfigMap>
tempo_config::extend_map(const ConfigMap &baseMap, const ConfigMap &overrideMap)
{
    std::vector<std::string> nodePath;
    auto extendResult = extend(baseMap, overrideMap, nodePath);
    if (extendResult.isStatus())
        return extendResult.getStatus();
    return extendResult.getResult().toMap();
}