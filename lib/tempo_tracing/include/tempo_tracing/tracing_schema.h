#ifndef TEMPO_TRACING_TRACING_SCHEMA_H
#define TEMPO_TRACING_TRACING_SCHEMA_H

#include "tracing_types.h"

namespace tempo_tracing {

    class OpentracingNs : public tempo_utils::SchemaNs {
    public:
        constexpr OpentracingNs() : tempo_utils::SchemaNs("io.opentracing") {};
    };
    constexpr OpentracingNs kOpentracingNs;

    enum class OpentracingId {
        Error,
        Component,
        Event,
        ErrorKind,
        Message,
        Stack,
        NUM_IDS,
    };

    constexpr tempo_utils::SchemaProperty<OpentracingNs,OpentracingId>
    kOpentracingErrorProperty(&kOpentracingNs, OpentracingId::Error, "Error", tempo_utils::PropertyType::kBool);

    constexpr tempo_utils::SchemaProperty<OpentracingNs,OpentracingId>
    kOpentracingComponentProperty(&kOpentracingNs, OpentracingId::Component, "Component", tempo_utils::PropertyType::kString);

    constexpr tempo_utils::SchemaProperty<OpentracingNs,OpentracingId>
    kOpentracingEventProperty(&kOpentracingNs, OpentracingId::Event, "Event", tempo_utils::PropertyType::kString);

    constexpr tempo_utils::SchemaProperty<OpentracingNs,OpentracingId>
    kOpentracingErrorKindProperty(&kOpentracingNs, OpentracingId::ErrorKind, "ErrorKind", tempo_utils::PropertyType::kString);

    constexpr tempo_utils::SchemaProperty<OpentracingNs,OpentracingId>
    kOpentracingMessageProperty(&kOpentracingNs, OpentracingId::Message, "Message", tempo_utils::PropertyType::kString);

    constexpr tempo_utils::SchemaProperty<OpentracingNs,OpentracingId>
    kOpentracingStackProperty(&kOpentracingNs, OpentracingId::Stack, "Stack", tempo_utils::PropertyType::kString);

    constexpr std::array<
        const tempo_utils::SchemaResource<OpentracingNs,OpentracingId> *,
        static_cast<std::size_t>(OpentracingId::NUM_IDS)>
    kOpentracingResources = {
        &kOpentracingErrorProperty,
        &kOpentracingComponentProperty,
        &kOpentracingEventProperty,
        &kOpentracingErrorKindProperty,
        &kOpentracingMessageProperty,
        &kOpentracingStackProperty
    };

    constexpr tempo_utils::SchemaVocabulary<OpentracingNs, OpentracingId>
    kOpentracingVocabulary(&kOpentracingNs, &kOpentracingResources);

    extern const tempo_utils::BoolAttr kOpentracingError;
    extern const tempo_utils::StringAttr kOpentracingComponent;
    extern const tempo_utils::StringAttr kOpentracingEvent;
    extern const tempo_utils::StringAttr kOpentracingErrorKind;
    extern const tempo_utils::StringAttr kOpentracingMessage;
    extern const tempo_utils::StringAttr kOpentracingStack;

    class TempoTracingNs : public tempo_utils::SchemaNs {
    public:
        constexpr TempoTracingNs() : tempo_utils::SchemaNs("dev.zuri.ns:tempo_tracing") {};
    };
    constexpr TempoTracingNs kTempoTracingNs;

    enum class TempoTracingId {
        ErrorCondition,
        ErrorCode,
        ErrorCategoryName,
        ContinuationHi,
        ContinuationLo,
        NUM_IDS,
    };

    constexpr tempo_utils::SchemaProperty<TempoTracingNs,TempoTracingId>
    kTempoTracingErrorConditionProperty(
        &kTempoTracingNs,
        TempoTracingId::ErrorCondition,
        "ErrorCondition",
        tempo_utils::PropertyType::kInt64);

    constexpr tempo_utils::SchemaProperty<TempoTracingNs,TempoTracingId>
    kTempoTracingErrorCodeProperty(
        &kTempoTracingNs,
        TempoTracingId::ErrorCode,
        "ErrorCode",
        tempo_utils::PropertyType::kInt64);

    constexpr tempo_utils::SchemaProperty<TempoTracingNs,TempoTracingId>
    kTempoTracingErrorCategoryNameProperty(
        &kTempoTracingNs,
        TempoTracingId::ErrorCategoryName,
        "ErrorCategoryName",
        tempo_utils::PropertyType::kString);

    constexpr tempo_utils::SchemaProperty<TempoTracingNs,TempoTracingId>
    kTempoTracingContinuationHiProperty(
        &kTempoTracingNs,
        TempoTracingId::ContinuationHi,
        "ContinuationHi",
        tempo_utils::PropertyType::kUInt64);

    constexpr tempo_utils::SchemaProperty<TempoTracingNs,TempoTracingId>
    kTempoTracingContinuationLoProperty(
        &kTempoTracingNs,
        TempoTracingId::ContinuationLo,
        "ContinuationLo",
        tempo_utils::PropertyType::kUInt64);

    constexpr std::array<
        const tempo_utils::SchemaResource<TempoTracingNs,TempoTracingId> *,
        static_cast<std::size_t>(TempoTracingId::NUM_IDS)>
    kTempoTracingResources = {
        &kTempoTracingErrorConditionProperty,
        &kTempoTracingErrorCodeProperty,
        &kTempoTracingErrorCategoryNameProperty,
        &kTempoTracingContinuationHiProperty,
        &kTempoTracingContinuationLoProperty,
    };

    constexpr tempo_utils::SchemaVocabulary<TempoTracingNs, TempoTracingId>
    kTempoTracingVocabulary(&kTempoTracingNs, &kTempoTracingResources);

    extern const tempo_utils::Int64Attr kTempoTracingErrorCondition;
    extern const tempo_utils::Int64Attr kTempoTracingErrorCode;
    extern const tempo_utils::StringAttr kTempoTracingErrorCategoryName;
    extern const tempo_utils::UInt64Attr kTempoTracingContinuationHi;
    extern const tempo_utils::UInt64Attr kTempoTracingContinuationLo;
}

#endif // TEMPO_TRACING_TRACING_SCHEMA_H