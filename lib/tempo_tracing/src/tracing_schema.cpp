
#include <tempo_tracing/tracing_schema.h>

const tempo_schema::BoolAttr tempo_tracing::kOpentracingError(
    &tempo_tracing::kOpentracingErrorProperty);

const tempo_schema::StringAttr tempo_tracing::kOpentracingComponent(
    &tempo_tracing::kOpentracingComponentProperty);

const tempo_schema::StringAttr tempo_tracing::kOpentracingEvent(
    &tempo_tracing::kOpentracingEventProperty);

const tempo_schema::StringAttr tempo_tracing::kOpentracingErrorKind(
    &tempo_tracing::kOpentracingErrorKindProperty);

const tempo_schema::StringAttr tempo_tracing::kOpentracingMessage(
    &tempo_tracing::kOpentracingMessageProperty);

const tempo_schema::StringAttr tempo_tracing::kOpentracingStack(
    &tempo_tracing::kOpentracingStackProperty);

const tempo_schema::UInt64Attr tempo_tracing::kTempoTracingContinuationHi(
    &tempo_tracing::kTempoTracingContinuationHiProperty);

const tempo_schema::UInt64Attr tempo_tracing::kTempoTracingContinuationLo(
    &tempo_tracing::kTempoTracingContinuationLoProperty);

const tempo_schema::Int64Attr tempo_tracing::kTempoTracingErrorCondition(
    &tempo_tracing::kTempoTracingErrorConditionProperty);

const tempo_schema::Int64Attr tempo_tracing::kTempoTracingErrorCode(
    &tempo_tracing::kTempoTracingErrorCodeProperty);

const tempo_schema::StringAttr tempo_tracing::kTempoTracingErrorCategoryName(
    &tempo_tracing::kTempoTracingErrorCategoryNameProperty);

const tempo_schema::UInt64Attr tempo_tracing::kTempoTracingLineNumber(
    &tempo_tracing::kTempoTracingLineNumberProperty);

const tempo_schema::UInt64Attr tempo_tracing::kTempoTracingColumnNumber(
    &tempo_tracing::kTempoTracingColumnNumberProperty);

const tempo_schema::UInt64Attr tempo_tracing::kTempoTracingFileOffset(
    &tempo_tracing::kTempoTracingFileOffsetProperty);

const tempo_schema::UInt64Attr tempo_tracing::kTempoTracingTextSpan(
    &tempo_tracing::kTempoTracingTextSpanProperty);

const tempo_schema::StringAttr tempo_tracing::kTempoTracingFilePath(
    &tempo_tracing::kTempoTracingFilePathProperty);
