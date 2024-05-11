
#include <tempo_tracing/internal/spanset_reader.h>
#include <tempo_tracing/spanset_state.h>
#include <tempo_tracing/tempo_spanset.h>
#include <tempo_tracing/tracing_types.h>
#include <tempo_utils/memory_bytes.h>

tempo_tracing::SpansetState::SpansetState(tempo_utils::TraceId id)
    : m_id(id),
      m_size(0)
{
    m_last = m_spans.before_begin();
}

tempo_tracing::SpanData&
tempo_tracing::SpansetState::appendSpan(tempo_utils::SpanId id)
{
    tu_uint32 index = m_size;

    // construct span data in-place at the end of the spans list
    m_last = m_spans.emplace_after(m_last, index, id);
    SpanData& data = *m_last;
    m_size++;

    return data;
}

tempo_tracing::SpanData&
tempo_tracing::SpansetState::appendSpan(tempo_utils::SpanId id, tu_uint32 parentIndex, tempo_utils::SpanId parentId)
{
    tu_uint32 index = m_size;
    TU_ASSERT (parentIndex < index);    // the parent span must come before the child  span (prevents cycles)

    // construct span data in-place at the end of the spans list
    m_last = m_spans.emplace_after(m_last, index, id, parentIndex, parentId);
    SpanData& data = *m_last;
    m_size++;

    return data;
}

static std::pair<tts1::Value,flatbuffers::Offset<void>>
serialize_value(flatbuffers::FlatBufferBuilder &buffer, const tempo_utils::AttrValue &value)
{
    switch (value.getType()) {
        case tempo_utils::ValueType::Nil: {
            auto type = tts1::Value::TrueFalseNilValue;
            auto offset = tts1::CreateTrueFalseNilValue(buffer, tts1::TrueFalseNil::Nil).Union();
            return {type, offset};
        }
        case tempo_utils::ValueType::Bool: {
            auto type = tts1::Value::TrueFalseNilValue;
            auto tfn = value.getBool()? tts1::TrueFalseNil::True : tts1::TrueFalseNil::False;
            auto offset = tts1::CreateTrueFalseNilValue(buffer, tfn).Union();
            return {type, offset};
        }
        case tempo_utils::ValueType::Int64: {
            auto type = tts1::Value::Int64Value;
            auto offset = tts1::CreateInt64Value(buffer, value.getInt64()).Union();
            return {type, offset};
        }
        case tempo_utils::ValueType::Float64: {
            auto type = tts1::Value::Float64Value;
            auto offset = tts1::CreateFloat64Value(buffer, value.getFloat64()).Union();
            return {type, offset};
        }
        case tempo_utils::ValueType::UInt64: {
            auto type = tts1::Value::UInt64Value;
            auto offset = tts1::CreateUInt64Value(buffer, value.getUInt64()).Union();
            return {type, offset};
        }
        case tempo_utils::ValueType::UInt32: {
            auto type = tts1::Value::UInt32Value;
            auto offset = tts1::CreateUInt32Value(buffer, value.getUInt32()).Union();
            return {type, offset};
        }
        case tempo_utils::ValueType::UInt16: {
            auto type = tts1::Value::UInt16Value;
            auto offset = tts1::CreateUInt16Value(buffer, value.getUInt16()).Union();
            return {type, offset};
        }
        case tempo_utils::ValueType::UInt8: {
            auto type = tts1::Value::UInt8Value;
            auto offset = tts1::CreateUInt8Value(buffer, value.getUInt8()).Union();
            return {type, offset};
        }
        case tempo_utils::ValueType::String: {
            auto type = tts1::Value::StringValue;
            auto offset = tts1::CreateStringValue(buffer, buffer.CreateSharedString(value.stringView())).Union();
            return {type, offset};
        }
        default:
            TU_UNREACHABLE();
    }
}

tempo_utils::Result<tempo_tracing::TempoSpanset>
tempo_tracing::SpansetState::toSpanset() const
{
    flatbuffers::FlatBufferBuilder buffer;

    auto trace_id_hi = m_id.getHi();
    auto trace_id_lo = m_id.getLo();

    absl::flat_hash_map<std::string_view,tu_uint32> nscache;

    std::vector<flatbuffers::Offset<tts1::NamespaceDescriptor>> namespaces_vector;
    std::vector<flatbuffers::Offset<tts1::SpanDescriptor>> spans_vector;
    std::vector<flatbuffers::Offset<tts1::AttributeDescriptor>> attrs_vector;
    std::vector<flatbuffers::Offset<tts1::LogDescriptor>> logs_vector;
    std::vector<tu_uint32> roots_vector;
    std::vector<tu_uint32> errors_vector;

    for (const auto &spanData : m_spans) {

        tu_uint32 span_index = spans_vector.size();

        tu_uint64 start_time = ToUnixMillis(spanData.startTime);
        tu_uint64 end_time = ToUnixMillis(spanData.endTime);

        tu_uint32 parent_index = tempo_tracing::kInvalidAddressU32;
        tu_uint64 parent_id = 0;
        if (!spanData.parentId.isValid()) {
            // if parent is invalid then this is a root span
            roots_vector.push_back(span_index);
        } else {
            // otherwise record the parent span id
            parent_index = spanData.parentIndex;
            parent_id = spanData.parentId.getId();
        }

        // if span is marked failed, then add the index to the errors vector
        if (spanData.error) {
            errors_vector.push_back(span_index);
        }

        std::vector<tu_uint32> children;
        std::vector<tu_uint32> tags;
        std::vector<tu_uint32> logs;

        for (const auto index : spanData.children) {
            children.push_back(index);
        }

        for (const auto &tag : spanData.tags) {
            tu_uint32 index = attrs_vector.size();

            const auto &key = tag.first;
            tu_uint32 ns;
            std::string_view nskey(key.ns);
            if (!nscache.contains(nskey)) {
                ns = namespaces_vector.size();
                namespaces_vector.push_back(tts1::CreateNamespaceDescriptor(buffer,
                    buffer.CreateString(key.ns)));
                nscache[nskey] = ns;
            } else {
                ns = nscache.at(std::string_view(key.ns));
            }

            auto p = serialize_value(buffer, tag.second);

            attrs_vector.push_back(tts1::CreateAttributeDescriptor(buffer,
                ns, key.id, p.first, p.second));
            tags.push_back(index);
        }

        for (const auto &log : spanData.logs) {
            std::vector<tu_uint32> fields;

            for (const auto &field : log.fields) {
                tu_uint32 index = attrs_vector.size();

                const auto &key = field.first;
                tu_uint32 ns;
                std::string_view nskey(key.ns);
                if (!nscache.contains(nskey)) {
                    ns = namespaces_vector.size();
                    namespaces_vector.push_back(tts1::CreateNamespaceDescriptor(buffer,
                        buffer.CreateString(key.ns)));
                    nscache[nskey] = ns;
                } else {
                    ns = nscache.at(std::string_view(key.ns));
                }

                auto p = serialize_value(buffer, field.second);

                attrs_vector.push_back(tts1::CreateAttributeDescriptor(buffer,
                    ns, key.id, p.first, p.second));
                fields.push_back(index);
            }

            tts1::LogSeverity log_severity;
            switch (log.severity) {
                case LogSeverity::kFatal:
                    log_severity = tts1::LogSeverity::Fatal;
                    break;
                case LogSeverity::kError:
                    log_severity = tts1::LogSeverity::Error;
                    break;
                case LogSeverity::kWarn:
                    log_severity = tts1::LogSeverity::Warn;
                    break;
                case LogSeverity::kInfo:
                    log_severity = tts1::LogSeverity::Info;
                    break;
                case LogSeverity::kVerbose:
                    log_severity = tts1::LogSeverity::Verbose;
                    break;
                case LogSeverity::kVeryVerbose:
                    log_severity = tts1::LogSeverity::VeryVerbose;
                    break;
                default:
                    TU_UNREACHABLE();
            }

            tu_uint64 log_ts = ToUnixMillis(log.ts);
            auto log_fields = buffer.CreateVector(fields);

            tu_uint32 index = logs_vector.size();
            logs_vector.push_back(tts1::CreateLogDescriptor(buffer, log_ts, log_severity, log_fields));
            logs.push_back(index);
        }

        tu_uint64 span_id = spanData.spanId.getId();

        spans_vector.push_back(tts1::CreateSpanDescriptor(buffer,
            span_id,
            buffer.CreateSharedString(spanData.operationName),
            parent_index,
            parent_id,
            buffer.CreateVector(children),
            start_time,
            end_time,
            buffer.CreateVector(tags),
            buffer.CreateVector(logs)));
    }

    auto fb_namespaces = buffer.CreateVector(namespaces_vector);
    auto fb_spans = buffer.CreateVector(spans_vector);
    auto fb_attrs = buffer.CreateVector(attrs_vector);
    auto fb_logs = buffer.CreateVector(logs_vector);
    auto fb_roots = buffer.CreateVector(roots_vector);
    auto fb_errors = buffer.CreateVector(errors_vector);

    // build spanset from buffer
    tts1::SpansetBuilder spansetBuilder(buffer);

    spansetBuilder.add_abi(tts1::SpansetVersion::Version1);
    spansetBuilder.add_trace_id_hi(trace_id_hi);
    spansetBuilder.add_trace_id_lo(trace_id_lo);
    spansetBuilder.add_namespaces(fb_namespaces);
    spansetBuilder.add_spans(fb_spans);
    spansetBuilder.add_attrs(fb_attrs);
    spansetBuilder.add_logs(fb_logs);
    spansetBuilder.add_roots(fb_roots);
    spansetBuilder.add_errors(fb_errors);

    // serialize spanset and mark the buffer as finished
    auto spanset = spansetBuilder.Finish();
    buffer.Finish(spanset, tts1::SpansetIdentifier());

    // copy the flatbuffer into our own byte array and instantiate package
    auto bytes = tempo_utils::MemoryBytes::copy(buffer.GetBufferSpan());
    return tempo_tracing::TempoSpanset(bytes);
}