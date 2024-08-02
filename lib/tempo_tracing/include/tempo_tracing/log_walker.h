#ifndef TEMPO_TRACING_LOG_WALKER_H
#define TEMPO_TRACING_LOG_WALKER_H

#include "spanset_attr_parser.h"
#include "tracing_result.h"

namespace tempo_tracing {

    class LogWalker {

    public:
        LogWalker();
        LogWalker(const LogWalker &other);

        bool isValid() const;

        absl::Time getTimestamp() const;
        LogSeverity getSeverity() const;

        bool hasField(const tempo_utils::AttrKey &key) const;
        bool hasField(const tempo_utils::AbstractAttrValidator &validator) const;
        int numFields() const;

    private:
        std::shared_ptr<const internal::SpansetReader> m_reader;
        tu_uint32 m_index;

        LogWalker(std::shared_ptr<const internal::SpansetReader> reader, tu_uint32 index);
        friend class SpanWalker;

        tu_uint32 findIndexForField(const tempo_utils::AttrKey &key) const;

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
        parseField(const AttrType &attr, SerdeType &value) const
        {
            auto index = findIndexForField(attr.getKey());
            if (index == kInvalidAddressU32)
                return TracingStatus::forCondition(TracingCondition::kMissingLog);
            SpansetAttrParser parser(m_reader);
            return attr.parseAttr(index, &parser, value);
        }

    };
}

#endif // TEMPO_TRACING_LOG_WALKER_H