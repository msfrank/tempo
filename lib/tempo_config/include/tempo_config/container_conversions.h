#ifndef TEMPO_CONFIG_CONTAINER_CONVERSIONS_H
#define TEMPO_CONFIG_CONTAINER_CONVERSIONS_H

#include <vector>

#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>

#include <tempo_utils/option_template.h>

#include "abstract_converter.h"
#include "config_utils.h"
#include "config_types.h"
#include "config_result.h"

namespace tempo_config {

    /**
     *
     * @tparam T
     */
    template<class T>
    class OptionTParser : public AbstractConverter<Option<T>> {
    public:

        /**
         *
         * @param valueParser
         */
        OptionTParser(const AbstractConverter<T> *valueParser)
            : m_valueParser(valueParser) {
        }

        /**
         *
         * @param node
         * @param o
         * @return
         */
        tempo_utils::Status
        convertValue(
            const ConfigNode &node,
            Option<T> &o) const override {
            if (node.isNil()) {
                o = Option<T>();
                return {};
            }
            T v;
            auto status = m_valueParser->convertValue(node, v);
            if (status.notOk())
                return status;
            o = Option<T>(v);
            return {};
        }

    private:
        const AbstractConverter<T> *m_valueParser;
    };

    /**
     *
     * @tparam T
     */
    template<class T>
    class SeqTParser : public AbstractConverter<std::vector<T>> {
    public:

        /**
         *
         * @param elementParser
         */
        SeqTParser(const AbstractConverter<T> *elementParser)
            : m_elementParser(elementParser) {
        }

        /**
         *
         * @param elementParser
         * @param seqDefault
         */
        SeqTParser(const AbstractConverter<T> *elementParser, const std::vector<T> &seqDefault)
            : m_elementParser(elementParser), m_default(seqDefault) {
        }

        /**
         *
         * @param node
         * @param vec
         * @return
         */
        tempo_utils::Status
        convertValue(
            const ConfigNode &node,
            std::vector<T> &vec) const override {
            if (node.isNil() && !m_default.isEmpty()) {
                vec = m_default.getValue();
                return {};
            }
            if (node.getNodeType() != ConfigNodeType::kSeq)
                return ConfigStatus::forCondition(ConfigCondition::kWrongType,
                    "expected Seq node but found {}", config_node_type_to_string(node.getNodeType()));
            auto nodeSeq = node.toSeq();
            std::vector<T> vec__;
            for (auto iterator = nodeSeq.seqBegin(); iterator != nodeSeq.seqEnd(); iterator++) {
                T element;
                auto status = m_elementParser->convertValue(*iterator, element);
                if (status.notOk())
                    return status;
                vec__.push_back(element);
            }
            vec = std::move(vec__);
            return {};
        }

    private:
        const AbstractConverter<T> *m_elementParser;
        Option<std::vector<T>> m_default;
    };

    /**
     *
     * @tparam T
     */
    template<class T>
    class SetTParser : public AbstractConverter<absl::flat_hash_set<T>> {
    public:

        /**
         *
         * @param elementParser
         */
        SetTParser(const AbstractConverter<T> *elementParser)
            : m_elementParser(elementParser) {
        }

        /**
         *
         * @param elementParser
         * @param seqDefault
         */
        SetTParser(const AbstractConverter<T> *elementParser, const absl::flat_hash_set<T> &seqDefault)
            : m_elementParser(elementParser), m_default(seqDefault) {
        }

        /**
         *
         * @param node
         * @param set
         * @return
         */
        tempo_utils::Status
        convertValue(
            const ConfigNode &node,
            absl::flat_hash_set<T> &set) const override {
            if (node.isNil() && !m_default.isEmpty()) {
                set = m_default.getValue();
                return {};
            }
            if (node.getNodeType() != ConfigNodeType::kSeq)
                return ConfigStatus::forCondition(ConfigCondition::kWrongType,
                    "expected Seq node but found {}", config_node_type_to_string(node.getNodeType()));
            auto nodeSeq = node.toSeq();
            absl::flat_hash_set<T> set__;
            for (auto iterator = nodeSeq.seqBegin(); iterator != nodeSeq.seqEnd(); iterator++) {
                T element;
                auto status = m_elementParser->convertValue(*iterator, element);
                if (status.notOk())
                    return status;
                set__.insert(element);
            }
            set = std::move(set__);
            return {};
        }

    private:
        const AbstractConverter<T> *m_elementParser;
        Option<absl::flat_hash_set<T>> m_default;
    };

    /**
     *
     * @tparam K
     * @tparam V
     */
    template<class K, class V>
    class MapKVParser : public AbstractConverter<absl::flat_hash_map<K, V>> {
    public:

        /**
         *
         * @param keyParser
         * @param valueParser
         */
        MapKVParser(const AbstractConverter<K> *keyParser, AbstractConverter<V> *valueParser)
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
            const AbstractConverter<K> *keyParser,
            const AbstractConverter<V> *valueParser,
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
        convertValue(
            const ConfigNode &node,
            absl::flat_hash_map<K, V> &map) const override
        {
            if (node.isNil() && !m_default.isEmpty()) {
                map = m_default.getValue();
                return {};
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
                    status = m_keyParser->convertValue(ConfigValue(iterator->first), key);
                    if (status.notOk())
                        return status;
                    status = m_valueParser->convertValue(iterator->second, value);
                    if (status.notOk())
                        return status;
                    map__[key] = value;
                }
                map = std::move(map__);
                return {};
        }

    private:
        const AbstractConverter<K> *m_keyParser;
        const AbstractConverter<V> *m_valueParser;
        Option<absl::flat_hash_map<K, V>> m_default;
    };
}

#endif // TEMPO_CONFIG_CONTAINER_CONVERSIONS_H
