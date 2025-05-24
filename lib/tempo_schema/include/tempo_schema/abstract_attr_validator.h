#ifndef TEMPO_SCHEMA_ABSTRACT_ATTR_VALIDATOR_H
#define TEMPO_SCHEMA_ABSTRACT_ATTR_VALIDATOR_H

#include "attr.h"
#include "schema_resource.h"

namespace tempo_schema {

    /**
     * The AttrValidator interface.
     */
    class AbstractAttrValidator {
    public:
        virtual ~AbstractAttrValidator() = default;

        virtual const ComparableResource *getResource() const = 0;
        virtual AttrKey getKey() const = 0;
    };
}

#endif // TEMPO_SCHEMA_ABSTRACT_ATTR_VALIDATOR_H
