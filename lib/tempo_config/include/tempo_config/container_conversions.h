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
     * Convert the node into a value of type T, then wrap the value into a `Option<T>`.
     *
     * @tparam T The value type.
     */
    template<class T>
    class OptionTParser : public AbstractConverter<Option<T>> {
    public:
        explicit OptionTParser(const AbstractConverter<T> *valueParser)
            : m_valueParser(valueParser)
        {
            TU_ASSERT (m_valueParser != nullptr);
        }

        tempo_utils::Status convertValue(const ConfigNode &node, Option<T> &o) const override
        {
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
     * Convert the node into a std::vector<T> using the given `elementParser` to convert
     * each individual element. The node must be a Seq node.
     *
     * @tparam T The element type.
     */
    template<class T>
    class SeqTParser : public AbstractConverter<std::vector<T>> {

    public:
        explicit SeqTParser(const AbstractConverter<T> *elementParser)
            : m_elementParser(elementParser)
        {
            TU_ASSERT (m_elementParser != nullptr);
        }
        SeqTParser(const AbstractConverter<T> *elementParser, const std::vector<T> &seqDefault)
            : m_elementParser(elementParser),
              m_default(seqDefault)
        {
            TU_ASSERT (m_elementParser != nullptr);
        }

        tempo_utils::Status convertValue(
            const ConfigNode &node,
            std::vector<T> &vec) const override
        {
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
     * Convert the node into a absl::flat_hash_set<T> using the given `elementParser` to convert
     * each individual element. The node must be a Seq node. The type T must be compatible with
     * flat_hash_set, i.e. it must be hashable and define operator==.
     *
     * @tparam T The element type.
     */
    template<class T>
    class SetTParser : public AbstractConverter<absl::flat_hash_set<T>> {

    public:
        explicit SetTParser(const AbstractConverter<T> *elementParser)
            : m_elementParser(elementParser)
        {
            TU_ASSERT (m_elementParser != nullptr);
        }
        SetTParser(const AbstractConverter<T> *elementParser, const absl::flat_hash_set<T> &seqDefault)
            : m_elementParser(elementParser),
              m_default(seqDefault)
        {
            TU_ASSERT (m_elementParser != nullptr);
        }

        tempo_utils::Status convertValue(
            const ConfigNode &node,
            absl::flat_hash_set<T> &set) const override
        {
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
     * Convert the node into a absl::flat_hash_map<K,V> using the given `keyParser` and
     * `valueParser` to convert each individual entry. The node must be a Map node. The type
     * K must be compatible with flat_hash_map, i.e. it must be hashable and define operator==.
     *
     * @tparam K The key type.
     * @tparam V The value type.
     */
    template<class K, class V>
    class MapKVParser : public AbstractConverter<absl::flat_hash_map<K, V>> {

    public:
        MapKVParser(const AbstractConverter<K> *keyParser, const AbstractConverter<V> *valueParser)
            : m_keyParser(keyParser), m_valueParser(valueParser)
        {
            TU_ASSERT (m_keyParser != nullptr);
            TU_ASSERT (m_valueParser != nullptr);
        }
        MapKVParser(
            const AbstractConverter<K> *keyParser,
            const AbstractConverter<V> *valueParser,
            const absl::flat_hash_map<K, V> &mapDefault)
            : m_keyParser(keyParser),
              m_valueParser(valueParser),
              m_default(mapDefault)
        {
            TU_ASSERT (m_keyParser != nullptr);
            TU_ASSERT (m_valueParser != nullptr);
        }

        tempo_utils::Status convertValue(
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

    /**
     * Convert the node into a value of type T, then move the value into a
     * `std::shared_ptr<T>`. Note that the type T must be a moveable type.
     *
     * @tparam T The value type.
     */
    template<typename T>
    class SharedPtrTParser : public AbstractConverter<std::shared_ptr<T>> {
    public:
        explicit SharedPtrTParser(const AbstractConverter<T> *valueParser)
            : m_valueParser(valueParser)
        {
            TU_ASSERT (m_valueParser != nullptr);
        };

        tempo_utils::Status convertValue(
            const ConfigNode &node,
            std::shared_ptr<T> &sharedValue) const override
        {
            T value;
            TU_RETURN_IF_NOT_OK (m_valueParser->convertValue(node, value));
            sharedValue = std::make_shared<T>(std::move(value));
            return {};
        }

    private:
        const AbstractConverter<T> *m_valueParser;
    };

    /**
     * Convert the node into a value of type T, then move the value into a
     * `std::shared_ptr<const T>`. Note that the type T must be a moveable type.
     *
     * @tparam T The value type.
     */
    template<typename T>
    class SharedPtrConstTParser : public AbstractConverter<std::shared_ptr<const T>> {
    public:
        explicit SharedPtrConstTParser(const AbstractConverter<T> *valueParser)
            : m_valueParser(valueParser)
        {
            TU_ASSERT (m_valueParser != nullptr);
        };

        tempo_utils::Status convertValue(
            const ConfigNode &node,
            std::shared_ptr<const T> &sharedValue) const override
        {
            T value;
            TU_RETURN_IF_NOT_OK (m_valueParser->convertValue(node, value));
            sharedValue = std::make_shared<const T>(std::move(value));
            return {};
        }

    private:
        const AbstractConverter<T> *m_valueParser;
    };
}

#endif // TEMPO_CONFIG_CONTAINER_CONVERSIONS_H
