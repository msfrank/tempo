#ifndef TEMPO_TRACING_INTERNAL_SPANSET_READER_H
#define TEMPO_TRACING_INTERNAL_SPANSET_READER_H

#include <span>
#include <string>

#include <tempo_tracing/generated/spanset.h>
#include <tempo_utils/tracing.h>

namespace tempo_tracing::internal {

    class SpansetReader {

    public:
        explicit SpansetReader(std::span<const tu_uint8> bytes);

        bool isValid() const;

        tts1::SpansetVersion getABI() const;

        tempo_utils::TraceId getTraceId() const;

        const tts1::NamespaceDescriptor *getNamespace(uint32_t index) const;
        uint32_t numNamespaces() const;

        const tts1::AttributeDescriptor *getAttribute(uint32_t index) const;
        uint32_t numAttributes() const;

        const tts1::LogDescriptor *getLog(uint32_t index) const;
        uint32_t numLogs() const;

        const tts1::SpanDescriptor *getSpan(uint32_t index) const;
        uint32_t numSpans() const;

        uint32_t getRoot(uint32_t index) const;
        uint32_t numRoots() const;

        uint32_t getError(uint32_t index) const;
        uint32_t numErrors() const;

        std::span<const tu_uint8> bytesView() const;

        std::string dumpJson() const;

    private:
        std::span<const tu_uint8> m_bytes;
        const tts1::Spanset *m_spanset;
    };

};

#endif // TEMPO_TRACING_INTERNAL_SPANSET_READER_H
