
#include <tempo_config/config_parser.h>
#include <tempo_config/config_result.h>
#include <tempo_config/internal/config_listener.h>
#include <tempo_config/internal/tracing_error_listener.h>
#include <tempo_tracing/enter_scope.h>
#include <tempo_utils/file_reader.h>

#include <antlr4-runtime.h>

#include "ConfigLexer.h"
#include "ConfigParser.h"

tempo_config::ConfigParser::ConfigParser(const ConfigParserOptions &options)
    : m_options(options)
{
}

tempo_utils::Result<tempo_config::ConfigNode>
tempo_config::ConfigParser::parseString(
    std::string_view utf8,
    std::shared_ptr<ConfigSource> configSource,
    std::shared_ptr<tempo_tracing::TraceRecorder> recorder) const
{
    if (utf8.empty())
        return ConfigStatus::forCondition(ConfigCondition::kMissingValue, "empty config string");

    antlr4::ANTLRInputStream input(utf8.data(), (size_t) utf8.size());
    tcf1::ConfigLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    tcf1::ConfigParser parser(&tokens);

    if (configSource == nullptr) {
        configSource = std::make_shared<ConfigSource>();
    }

    // create the trace context
    std::shared_ptr<tempo_tracing::TraceContext> context;
    if (recorder != nullptr) {
        TU_ASSIGN_OR_RETURN (context, tempo_tracing::TraceContext::makeUnownedContextAndSwitch(recorder));
    } else {
        TU_ASSIGN_OR_RETURN (context, tempo_tracing::TraceContext::makeContextAndSwitch());
    }

    // ensure context is released
    tempo_tracing::ReleaseContext releaser(context);

    // create the root span
    tempo_tracing::EnterScope scope("tempo_config::ConfigParser");

    // create the listener
    internal::ConfigListener listener(configSource, context);

    // create the error listener
    internal::TracingErrorListener tracingErrorListener(&listener);
    lexer.removeErrorListeners();
    lexer.addErrorListener(&tracingErrorListener);
    parser.removeErrorListeners();
    parser.addErrorListener(&tracingErrorListener);

    //
    if (m_options.enableExtraDiagnostics) {
        antlr4::DiagnosticErrorListener diagnosticErrorListener(!m_options.reportAllAmbiguities);
        parser.addErrorListener(&diagnosticErrorListener);
    }

    try {
        antlr4::tree::ParseTree *tree = parser.root();
        antlr4::tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);
    } catch (tempo_utils::StatusException &ex) {
        return ex.getStatus();
    } catch (antlr4::ParseCancellationException &ex) {
        return ConfigStatus::forCondition(ConfigCondition::kConfigInvariant, ex.what());
    }

    return listener.toConfig();
}

tempo_utils::Result<tempo_config::ConfigNode>
tempo_config::ConfigParser::parseFile(
    const std::filesystem::path &path,
    std::shared_ptr<tempo_tracing::TraceRecorder> recorder) const
{
    tempo_utils::FileReader fileReader(path);
    TU_RETURN_IF_NOT_OK (fileReader.getStatus());

    auto bytes = fileReader.getBytes();
    std::string_view utf8((const char *) bytes->getData(), bytes->getSize());
    auto configSource = std::make_shared<ConfigSource>(ConfigSourceType::File, path.string());

    return parseString(utf8, configSource, std::move(recorder));
}