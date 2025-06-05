#ifndef TEMPO_CONFIG_CONFIG_PARSER_H
#define TEMPO_CONFIG_CONFIG_PARSER_H

#include <tempo_tracing/trace_recorder.h>
#include <tempo_utils/result.h>
#include <tempo_utils/url.h>

#include "config_types.h"

namespace tempo_config {

    struct ConfigParserOptions {
        bool enableExtraDiagnostics = false;
        bool reportAllAmbiguities = false;
    };

    class ConfigParser {
    public:
        ConfigParser(const ConfigParserOptions &options);

        tempo_utils::Result<ConfigNode> parseString(
            std::string_view utf8,
            std::shared_ptr<ConfigSource> configSource,
            std::shared_ptr<tempo_tracing::TraceRecorder> recorder) const;

        tempo_utils::Result<ConfigNode> parseFile(
            const std::filesystem::path &path,
            std::shared_ptr<tempo_tracing::TraceRecorder> recorder) const;

    private:
        ConfigParserOptions m_options;
    };
}

#endif // TEMPO_CONFIG_CONFIG_PARSER_H
