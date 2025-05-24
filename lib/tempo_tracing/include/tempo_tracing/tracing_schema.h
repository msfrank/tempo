#ifndef TEMPO_TRACING_TRACING_SCHEMA_H
#define TEMPO_TRACING_TRACING_SCHEMA_H

#include <tempo_schema/attr_serde.h>
#include <tempo_schema/schema.h>
#include <tempo_schema/schema_namespace.h>
#include <tempo_schema/schema_resource.h>

#include "tracing_types.h"

namespace tempo_tracing {

    class OpentracingNs : public tempo_schema::SchemaNs {
    public:
        constexpr OpentracingNs() : tempo_schema::SchemaNs("io.opentracing") {};
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

    constexpr tempo_schema::SchemaProperty<OpentracingNs,OpentracingId>
    kOpentracingErrorProperty(&kOpentracingNs, OpentracingId::Error, "Error", tempo_schema::PropertyType::kBool);

    constexpr tempo_schema::SchemaProperty<OpentracingNs,OpentracingId>
    kOpentracingComponentProperty(&kOpentracingNs, OpentracingId::Component, "Component", tempo_schema::PropertyType::kString);

    constexpr tempo_schema::SchemaProperty<OpentracingNs,OpentracingId>
    kOpentracingEventProperty(&kOpentracingNs, OpentracingId::Event, "Event", tempo_schema::PropertyType::kString);

    constexpr tempo_schema::SchemaProperty<OpentracingNs,OpentracingId>
    kOpentracingErrorKindProperty(&kOpentracingNs, OpentracingId::ErrorKind, "ErrorKind", tempo_schema::PropertyType::kString);

    constexpr tempo_schema::SchemaProperty<OpentracingNs,OpentracingId>
    kOpentracingMessageProperty(&kOpentracingNs, OpentracingId::Message, "Message", tempo_schema::PropertyType::kString);

    constexpr tempo_schema::SchemaProperty<OpentracingNs,OpentracingId>
    kOpentracingStackProperty(&kOpentracingNs, OpentracingId::Stack, "Stack", tempo_schema::PropertyType::kString);

    constexpr std::array<
        const tempo_schema::SchemaResource<OpentracingNs,OpentracingId> *,
        static_cast<std::size_t>(OpentracingId::NUM_IDS)>
    kOpentracingResources = {
        &kOpentracingErrorProperty,
        &kOpentracingComponentProperty,
        &kOpentracingEventProperty,
        &kOpentracingErrorKindProperty,
        &kOpentracingMessageProperty,
        &kOpentracingStackProperty
    };

    constexpr tempo_schema::SchemaVocabulary<OpentracingNs, OpentracingId>
    kOpentracingVocabulary(&kOpentracingNs, &kOpentracingResources);

    extern const tempo_schema::BoolAttr kOpentracingError;
    extern const tempo_schema::StringAttr kOpentracingComponent;
    extern const tempo_schema::StringAttr kOpentracingEvent;
    extern const tempo_schema::StringAttr kOpentracingErrorKind;
    extern const tempo_schema::StringAttr kOpentracingMessage;
    extern const tempo_schema::StringAttr kOpentracingStack;

    class TempoTracingNs : public tempo_schema::SchemaNs {
    public:
        constexpr TempoTracingNs() : tempo_schema::SchemaNs("dev.zuri.ns:tempo_tracing") {};
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

    constexpr tempo_schema::SchemaProperty<TempoTracingNs,TempoTracingId>
    kTempoTracingErrorConditionProperty(
        &kTempoTracingNs,
        TempoTracingId::ErrorCondition,
        "ErrorCondition",
        tempo_schema::PropertyType::kInt64);

    constexpr tempo_schema::SchemaProperty<TempoTracingNs,TempoTracingId>
    kTempoTracingErrorCodeProperty(
        &kTempoTracingNs,
        TempoTracingId::ErrorCode,
        "ErrorCode",
        tempo_schema::PropertyType::kInt64);

    constexpr tempo_schema::SchemaProperty<TempoTracingNs,TempoTracingId>
    kTempoTracingErrorCategoryNameProperty(
        &kTempoTracingNs,
        TempoTracingId::ErrorCategoryName,
        "ErrorCategoryName",
        tempo_schema::PropertyType::kString);

    constexpr tempo_schema::SchemaProperty<TempoTracingNs,TempoTracingId>
    kTempoTracingContinuationHiProperty(
        &kTempoTracingNs,
        TempoTracingId::ContinuationHi,
        "ContinuationHi",
        tempo_schema::PropertyType::kUInt64);

    constexpr tempo_schema::SchemaProperty<TempoTracingNs,TempoTracingId>
    kTempoTracingContinuationLoProperty(
        &kTempoTracingNs,
        TempoTracingId::ContinuationLo,
        "ContinuationLo",
        tempo_schema::PropertyType::kUInt64);

    constexpr std::array<
        const tempo_schema::SchemaResource<TempoTracingNs,TempoTracingId> *,
        static_cast<std::size_t>(TempoTracingId::NUM_IDS)>
    kTempoTracingResources = {
        &kTempoTracingErrorConditionProperty,
        &kTempoTracingErrorCodeProperty,
        &kTempoTracingErrorCategoryNameProperty,
        &kTempoTracingContinuationHiProperty,
        &kTempoTracingContinuationLoProperty,
    };

    constexpr tempo_schema::SchemaVocabulary<TempoTracingNs, TempoTracingId>
    kTempoTracingVocabulary(&kTempoTracingNs, &kTempoTracingResources);

    extern const tempo_schema::Int64Attr kTempoTracingErrorCondition;
    extern const tempo_schema::Int64Attr kTempoTracingErrorCode;
    extern const tempo_schema::StringAttr kTempoTracingErrorCategoryName;
    extern const tempo_schema::UInt64Attr kTempoTracingContinuationHi;
    extern const tempo_schema::UInt64Attr kTempoTracingContinuationLo;
}

#endif // TEMPO_TRACING_TRACING_SCHEMA_H