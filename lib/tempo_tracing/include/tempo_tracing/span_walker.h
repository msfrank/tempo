#ifndef TEMPO_TRACING_SPAN_WALKER_H
#define TEMPO_TRACING_SPAN_WALKER_H

#include "log_walker.h"
#include "spanset_attr_parser.h"
#include "tracing_result.h"
#include "tracing_types.h"

namespace tempo_tracing {

    class SpanWalker {

    public:
        SpanWalker();
        SpanWalker(const SpanWalker &other);

        bool isValid() const;

        tempo_utils::SpanId getId() const;
        std::string getOperationName() const;
        absl::Time getStartTime() const;
        absl::Time getEndTime() const;

        bool hasTag(const tempo_schema::AttrKey &key) const;
        bool hasTag(const tempo_schema::AbstractAttrValidator &validator) const;
        int numTags() const;

        LogWalker getLog(tu_uint32 index) const;
        int numLogs() const;

        bool hasParent() const;
        SpanWalker getParent() const;

        SpanWalker getChild(tu_uint32 index) const;
        int numChildren() const;
        SpanWalker getSpanAtOffset(tu_uint32 offset) const;

    private:
        std::shared_ptr<const internal::SpansetReader> m_reader;
        tu_uint32 m_index;

        SpanWalker(std::shared_ptr<const internal::SpansetReader> reader, tu_uint32 index);
        friend class ErrorWalker;
        friend class RootWalker;
        friend class TempoSpanset;

        tu_uint32 findIndexForTag(const tempo_schema::AttrKey &key) const;

    public:
        /**
         *
         * @tparam AttrType
         * @tparam SerdeType
         * @param attr
         * @param value
         * @return
         */
        template<class AttrType,
            typename SerdeType = typename AttrType::SerdeType>
        tempo_utils::Status
        parseTag(const AttrType &attr, SerdeType &value) const
        {
            auto index = findIndexForTag(attr.getKey());
            if (index == kInvalidAddressU32)
                return TracingStatus::forCondition(TracingCondition::kMissingTag);
            SpansetAttrParser parser(m_reader);
            return attr.parseAttr(index, &parser, value);
        }

    };
}

#endif // TEMPO_TRACING_SPAN_WALKER_H