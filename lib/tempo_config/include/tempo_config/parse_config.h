#ifndef TEMPO_CONFIG_CONFIG_CONVERSIONS_H
#define TEMPO_CONFIG_CONFIG_CONVERSIONS_H

#include <filesystem>

#include <tempo_utils/integer_types.h>
#include <tempo_utils/option_template.h>
#include <tempo_utils/url.h>

#include "abstract_converter.h"
#include "config_utils.h"
#include "config_types.h"
#include "config_result.h"

namespace tempo_config {

    /**
     *
     * @tparam T
     * @param dst
     * @param parser
     * @param node
     * @return
     */
    template <class T>
    tempo_utils::Status
    parse_config(T &dst, const AbstractConverter<T> &parser, const ConfigNode &node)
    {
        return parser.convertValue(node, dst);
    }

    /**
     *
     * @tparam T
     * @param dst
     * @param parser
     * @param node
     * @param index
     * @return
     */
    template <class T>
    tempo_utils::Status
    parse_config(T &dst, const AbstractConverter<T> &parser, const ConfigSeq &node, int index)
    {
        return parser.convertValue(node.seqAt(index), dst);
    }

    /**
     *
     * @tparam T
     * @param dst
     * @param parser
     * @param node
     * @param key
     * @return
     */
    template <class T>
    tempo_utils::Status
    parse_config(T &dst, const AbstractConverter<T> &parser, const ConfigMap &node, std::string_view key)
    {
        return parser.convertValue(node.mapAt(key), dst);
    }

    /**
     *
     * @tparam T
     * @param dst
     * @param parser
     * @param path
     * @return
     */
    template <typename T>
    tempo_utils::Status
    parse_config_file(
        T &dst,
        const AbstractConverter<T> &parser,
        const std::filesystem::path &path)
    {
        auto result = read_config_file(path);
        if (result.isStatus())
            return result.getStatus();
        tempo_config::ConfigStatus status;
        auto root = result.getResult();
        return parser.convertValue(root, dst);
    }
}

#endif // TEMPO_CONFIG_CONFIG_CONVERSIONS_H