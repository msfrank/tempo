#ifndef TEMPO_CONFIG_CONTAINER_CONVERSIONS_H
#define TEMPO_CONFIG_CONTAINER_CONVERSIONS_H

#include <vector>

#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>

#include <tempo_utils/option_template.h>

#include "abstract_config_parser.h"
#include "config_serde.h"
#include "config_types.h"
#include "config_result.h"

namespace tempo_config {

    /**
     *
     * @tparam T
     */
    template<class T>
    class OptionTParser : public AbstractConfigParser<Option<T>> {
    public:

        /**
         *
         * @param valueParser
         */
        OptionTParser(const AbstractConfigParser<T> *valueParser)
            : m_valueParser(valueParser) {
        }

        /**
         *
         * @param node
         * @param o
         * @return
         */
        tempo_utils::Status
        parseValue(
            const ConfigNode &node,
            Option<T> &o) const override {
            if (node.isNil()) {
                o = Option<T>();
                return ConfigStatus::ok();
            }
            T v;
            auto status = m_valueParser->parseValue(node, v);
            if (status.notOk())
                return status;
            o = Option<T>(v);
            return ConfigStatus::ok();
        }

    private:
        const AbstractConfigParser<T> *m_valueParser;
    };

    /**
     *
     * @tparam T
     */
    template<class T>
    class SeqTParser : public AbstractConfigParser<std::vector<T>> {
    public:

        /**
         *
         * @param elementParser
         */
        SeqTParser(const AbstractConfigParser<T> *elementParser)
            : m_elementParser(elementParser) {
        }

        /**
         *
         * @param elementParser
         * @param seqDefault
         */
        SeqTParser(const AbstractConfigParser<T> *elementParser, const std::vector<T> &seqDefault)
            : m_elementParser(elementParser), m_default(seqDefault) {
        }

        /**
         *
         * @param node
         * @param vec
         * @return
         */
        tempo_utils::Status
        parseValue(
            const ConfigNode &node,
            std::vector<T> &vec) const override {
            if (node.isNil() && !m_default.isEmpty()) {
                vec = m_default.getValue();
                return ConfigStatus::ok();
            }
            if (node.getNodeType() != ConfigNodeType::kSeq)
                return ConfigStatus::forCondition(ConfigCondition::kWrongType,
                    "expected Seq node but found {}", config_node_type_to_string(node.getNodeType()));
            auto nodeSeq = node.toSeq();
            std::vector<T> vec__;
            for (auto iterator = nodeSeq.seqBegin(); iterator != nodeSeq.seqEnd(); iterator++) {
                T element;
                auto status = m_elementParser->parseValue(*iterator, element);
                if (status.notOk())
                    return status;
                vec__.push_back(element);
            }
            vec = std::move(vec__);
            return ConfigStatus::ok();
        }

    private:
        const AbstractConfigParser<T> *m_elementParser;
        Option<std::vector<T>> m_default;
    };

    /**
     *
     * @tparam T
     */
    template<class T>
    class SetTParser : public AbstractConfigParser<absl::flat_hash_set<T>> {
    public:

        /**
         *
         * @param elementParser
         */
        SetTParser(const AbstractConfigParser<T> *elementParser)
            : m_elementParser(elementParser) {
        }

        /**
         *
         * @param elementParser
         * @param seqDefault
         */
        SetTParser(const AbstractConfigParser<T> *elementParser, const absl::flat_hash_set<T> &seqDefault)
            : m_elementParser(elementParser), m_default(seqDefault) {
        }

        /**
         *
         * @param node
         * @param set
         * @return
         */
        tempo_utils::Status
        parseValue(
            const ConfigNode &node,
            absl::flat_hash_set<T> &set) const override {
            if (node.isNil() && !m_default.isEmpty()) {
                set = m_default.getValue();
                return ConfigStatus::ok();
            }
            if (node.getNodeType() != ConfigNodeType::kSeq)
                return ConfigStatus::forCondition(ConfigCondition::kWrongType,
                    "expected Seq node but found {}", config_node_type_to_string(node.getNodeType()));
            auto nodeSeq = node.toSeq();
            absl::flat_hash_set<T> set__;
            for (auto iterator = nodeSeq.seqBegin(); iterator != nodeSeq.seqEnd(); iterator++) {
                T element;
                auto status = m_elementParser->parseValue(*iterator, element);
                if (status.notOk())
                    return status;
                set__.insert(element);
            }
            set = std::move(set__);
            return ConfigStatus::ok();
        }

    private:
        const AbstractConfigParser<T> *m_elementParser;
        Option<absl::flat_hash_set<T>> m_default;
    };

    /**
     *
     * @tparam K
     * @tparam V
     */
    template<class K, class V>
    class MapKVParser : public AbstractConfigParser<absl::flat_hash_map<K, V>> {
    public:

        /**
         *
         * @param keyParser
         * @param valueParser
         */
        MapKVParser(const AbstractConfigParser<K> *keyParser, AbstractConfigParser<V> *valueParser)
            : m_keyParser(keyParser), m_valueParser(valueParser)
        {
        }

        /**
         *
         * @param keyParser
         * @param valueParser
         * @param mapDefault
         */
        MapKVParser(
            const AbstractConfigParser<K> *keyParser,
            const AbstractConfigParser<V> *valueParser,
            const absl::flat_hash_map<K, V> &mapDefault)
            : m_keyParser(keyParser),
              m_valueParser(valueParser),
              m_default(mapDefault)
        {
        }

        /**
         *
         * @param node
         * @param map
         * @return
         */
        tempo_utils::Status
        parseValue(
            const ConfigNode &node,
            absl::flat_hash_map<K, V> &map) const override
        {
            if (node.isNil() && !m_default.isEmpty()) {
                map = m_default.getValue();
                return ConfigStatus::ok();
            }
            if (node.getNodeType() != ConfigNodeType::kMap)
                return ConfigStatus::forCondition(ConfigCondition::kWrongType,
                    "expected Map node but found {}", config_node_type_to_string(node.getNodeType()));
            auto nodeMap = node.toMap();
            absl::flat_hash_map<K, V> map__;
                for (auto iterator = nodeMap.mapBegin(); iterator != nodeMap.mapEnd(); iterator++) {
                    K key;
                    V value;
                    tempo_utils::Status status;
                    status = m_keyParser->parseValue(ConfigValue(iterator->first), key);
                    if (status.notOk())
                        return status;
                    status = m_valueParser->parseValue(iterator->second, value);
                    if (status.notOk())
                        return status;
                    map__[key] = value;
                }
                map = std::move(map__);
                return ConfigStatus::ok();
        }

    private:
        const AbstractConfigParser<K> *m_keyParser;
        const AbstractConfigParser<V> *m_valueParser;
        Option<absl::flat_hash_map<K, V>> m_default;
    };
}

#endif // TEMPO_CONFIG_CONTAINER_CONVERSIONS_H
