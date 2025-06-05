#ifndef TEMPO_CONFIG_INTERNAL_TRACING_ERROR_LISTENER_H
#define TEMPO_CONFIG_INTERNAL_TRACING_ERROR_LISTENER_H

#include <memory>

#include <antlr4-runtime.h>

#include <tempo_tracing/trace_span.h>

namespace tempo_config::internal {

    class ConfigListener;

    class TracingErrorListener : public antlr4::BaseErrorListener {
    public:
        explicit TracingErrorListener(ConfigListener *listener);

        void syntaxError(
            antlr4::Recognizer *recognizer,
            antlr4::Token *offendingSymbol,
            size_t line,
            size_t charPositionInLine,
            const std::string &message,
            std::exception_ptr e) override;

    private:
        ConfigListener *m_listener;
        std::shared_ptr<tempo_tracing::TraceSpan> m_span;
        tempo_utils::Status m_status;
    };
}

#endif // TEMPO_CONFIG_INTERNAL_TRACING_ERROR_LISTENER_H