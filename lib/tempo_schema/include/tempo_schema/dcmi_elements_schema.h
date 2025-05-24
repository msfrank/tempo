#ifndef TEMPO_SCHEMA_DCMI_ELEMENTS_SCHEMA_H
#define TEMPO_SCHEMA_DCMI_ELEMENTS_SCHEMA_H

#include <array>

#include "schema.h"
#include "schema_namespace.h"

namespace tempo_schema {

    class DcmiElementsNs : public SchemaNs {
    public:
        constexpr DcmiElementsNs() : SchemaNs("http://purl.org/dc/elements/1.1/") {};
    };
    constexpr DcmiElementsNs kDcmiElementsNs;

    enum class DcmiElementsId {
        Contributor,   // "An entity responsible for making contributions to the resource".
        Coverage,      // "The spatial or temporal topic of the resource, the spatial applicability of the resource, or the jurisdiction under which the resource is relevant".
        Creator,       // "An entity primarily responsible for making the resource".
        Date,          // "A point or period of time associated with an event in the lifecycle of the resource".
        Description,   // "An account of the resource".
        Format,        // "The file format, physical medium, or dimensions of the resource".
        Identifier,    // "An unambiguous reference to the resource within a given context".
        Language,      // "A language of the resource".
        Publisher,     // "An entity responsible for making the resource available".
        Relation,      // "A related resource".
        Rights,        // "Information about rights held in and over the resource".
        Source,        // "A related resource from which the described resource is derived".
        Subject,       // "The topic of the resource".
        Title,         // "A name given to the resource".
        Type,          // "The nature or genre of the resource".
        NUM_IDS,
    };

    constexpr SchemaProperty<DcmiElementsNs,DcmiElementsId>
    kDcmiElementsContributorProperty(
        &kDcmiElementsNs, DcmiElementsId::Contributor, "Contributor", PropertyType::kString);

    constexpr SchemaProperty<DcmiElementsNs,DcmiElementsId>
    kDcmiElementsCoverageProperty(
        &kDcmiElementsNs, DcmiElementsId::Coverage, "Coverage", PropertyType::kString);

    constexpr SchemaProperty<DcmiElementsNs,DcmiElementsId>
    kDcmiElementsCreatorProperty(
        &kDcmiElementsNs, DcmiElementsId::Creator, "Creator", PropertyType::kString);

    constexpr SchemaProperty<DcmiElementsNs,DcmiElementsId>
    kDcmiElementsDateProperty(
        &kDcmiElementsNs, DcmiElementsId::Date, "Date", PropertyType::kString);

    constexpr SchemaProperty<DcmiElementsNs,DcmiElementsId>
    kDcmiElementsDescriptionProperty(
        &kDcmiElementsNs, DcmiElementsId::Description, "Description", PropertyType::kString);

    constexpr SchemaProperty<DcmiElementsNs,DcmiElementsId>
    kDcmiElementsFormatProperty(
        &kDcmiElementsNs, DcmiElementsId::Format, "Format", PropertyType::kString);

    constexpr SchemaProperty<DcmiElementsNs,DcmiElementsId>
    kDcmiElementsIdentifierProperty(
        &kDcmiElementsNs, DcmiElementsId::Identifier, "Identifier", PropertyType::kString);

    constexpr SchemaProperty<DcmiElementsNs,DcmiElementsId>
    kDcmiElementsLanguageProperty(
        &kDcmiElementsNs, DcmiElementsId::Language, "Language", PropertyType::kString);

    constexpr SchemaProperty<DcmiElementsNs,DcmiElementsId>
    kDcmiElementsPublisherProperty(
        &kDcmiElementsNs, DcmiElementsId::Publisher, "Publisher", PropertyType::kString);

    constexpr SchemaProperty<DcmiElementsNs,DcmiElementsId>
    kDcmiElementsRelationProperty(
        &kDcmiElementsNs, DcmiElementsId::Relation, "Relation", PropertyType::kString);

    constexpr SchemaProperty<DcmiElementsNs,DcmiElementsId>
    kDcmiElementsRightsProperty(
        &kDcmiElementsNs, DcmiElementsId::Rights, "Rights", PropertyType::kString);

    constexpr SchemaProperty<DcmiElementsNs,DcmiElementsId>
    kDcmiElementsSourceProperty(
        &kDcmiElementsNs, DcmiElementsId::Source, "Source", PropertyType::kString);

    constexpr SchemaProperty<DcmiElementsNs,DcmiElementsId>
    kDcmiElementsSubjectProperty(
        &kDcmiElementsNs, DcmiElementsId::Subject, "Subject", PropertyType::kString);

    constexpr SchemaProperty<DcmiElementsNs,DcmiElementsId>
    kDcmiElementsTitleProperty(
        &kDcmiElementsNs, DcmiElementsId::Title, "Title", PropertyType::kString);

    constexpr SchemaProperty<DcmiElementsNs,DcmiElementsId>
    kDcmiElementsTypeProperty(
        &kDcmiElementsNs, DcmiElementsId::Type, "Type", PropertyType::kString);
}

#endif // TEMPO_SCHEMA_DCMI_ELEMENTS_SCHEMA_H