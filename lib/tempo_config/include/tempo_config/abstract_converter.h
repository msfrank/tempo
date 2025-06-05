#ifndef TEMPO_CONFIG_ABSTRACT_CONVERTER_H
#define TEMPO_CONFIG_ABSTRACT_CONVERTER_H

#include <tempo_utils/option_template.h>
#include <tempo_utils/status.h>

#include "config_types.h"

namespace tempo_config {

    /**
     * AbstractConverter defines the operations needed to parse a ConfigNode into a typed value.
     *
     * @tparam T The value type.
     */
    template <class T>
    class AbstractConverter {
    public:
        virtual ~AbstractConverter() = default;

        /**
         * Convert the given ConfigNode into a typed value. <EM>If parsing fails then value must not be modified.</EM>
         *
         * @param node The ConfigNode to parse.
         * @param value Typed value reference to store the parsed result.
         * @return A `tempo_utils::Status` containing the status of the operation.
         */
        virtual tempo_utils::Status convertValue(const ConfigNode &node, T &value) const = 0;

        /**
         * Returns a string representation of the default value configured by the converter, or an empty Option if
         * there is no default value. The default implementation returns an empty Option.
         *
         * @return A `tempo_utils::Option` containing a string representation of the default value, or an empty
         *     Option if there is no default value.
         */
        virtual Option<std::string> getDefault() const
        {
            return Option<std::string>();
        }
    };
}

#endif // TEMPO_CONFIG_ABSTRACT_CONVERTER_H
