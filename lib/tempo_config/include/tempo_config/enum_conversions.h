#ifndef TEMPO_CONFIG_ENUM_CONVERSIONS_H
#define TEMPO_CONFIG_ENUM_CONVERSIONS_H

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
    template <typename T,
        typename = std::enable_if_t<std::is_enum<T>::value>>
    class EnumTParser : public AbstractConfigParser<T> {
    public:

        /**
         *
         * @param stringToEnumMap
         */
        EnumTParser(const absl::flat_hash_map<std::string,T> &stringToEnumMap)
            : m_stringToEnumMap(stringToEnumMap)
        {
        }

        /**
         *
         * @param stringToEnumMap
         * @param enumDefault
         */
        EnumTParser(
            const absl::flat_hash_map<std::string,T> &stringToEnumMap,
            const T enumDefault)
            : m_stringToEnumMap(stringToEnumMap), m_default(Option<T>(enumDefault))
        {
        }

        /**
         *
         * @param node
         * @param e
         * @return
         */
        tempo_utils::Status
        parseValue(
            const ConfigNode &node,
            T &e) const override
        {
            if (node.isNil() && !m_default.isEmpty()) {
                e = m_default.getValue();
                return tempo_config::ConfigStatus::forCondition(ConfigCondition::kMissingValue,
                    "missing required enum value");
            }
            if (node.getNodeType() != ConfigNodeType::kValue)
                return ConfigStatus::forCondition(ConfigCondition::kWrongType,
                    "expected Value node but found {}", config_node_type_to_string(node.getNodeType()));

            auto value = node.toValue().getValue();
            if (m_stringToEnumMap.contains(value)) {
                e = m_stringToEnumMap.at(value);
                return tempo_config::ConfigStatus::ok();
            }
            return tempo_config::ConfigStatus::forCondition(ConfigCondition::kMissingValue,
                "missing required enum value");
        }

    private:
        const absl::flat_hash_map<std::string,T> m_stringToEnumMap;
        const Option<T> m_default;
    };
}

#endif // TEMPO_CONFIG_ENUM_CONVERSIONS_H
