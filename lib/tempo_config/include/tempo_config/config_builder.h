#ifndef TEMPO_CONFIG_CONFIG_BUILDER_H
#define TEMPO_CONFIG_CONFIG_BUILDER_H

#include "config_types.h"

namespace tempo_config {

    ConfigNode nilNode();

    ConfigNode valueNode(std::string_view value);

    class ConfigSeqBuilder {
    public:
        ConfigSeqBuilder();

        ConfigSeqBuilder append(const ConfigNil &nil);
        ConfigSeqBuilder append(const ConfigValue &value);
        ConfigSeqBuilder append(const ConfigSeq &seq);
        ConfigSeqBuilder append(const ConfigMap &map);
        ConfigSeqBuilder append(const ConfigNode &node);

        ConfigNode buildNode();
        ConfigSeq buildSeq();

    private:
        struct Priv {
            std::vector<ConfigNode> seq;
        };
        std::shared_ptr<Priv> m_priv;
    };

    ConfigSeqBuilder startSeq();

    class ConfigMapBuilder {
    public:
        ConfigMapBuilder();

        ConfigMapBuilder put(std::string_view name, const ConfigNil &nil);
        ConfigMapBuilder put(std::string_view name, const ConfigValue &value);
        ConfigMapBuilder put(std::string_view name, const ConfigSeq &seq);
        ConfigMapBuilder put(std::string_view name, const ConfigMap &map);
        ConfigMapBuilder put(std::string_view name, const ConfigNode &node);

        ConfigNode buildNode();
        ConfigMap buildMap();

    private:
        struct Priv {
            absl::flat_hash_map<std::string,ConfigNode> map;
        };
        std::shared_ptr<Priv> m_priv;
    };

    ConfigMapBuilder startMap();

}

#endif // TEMPO_CONFIG_CONFIG_BUILDER_H
