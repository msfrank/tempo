#ifndef TEMPO_SCHEMA_STATEFUL_SERDE_H
#define TEMPO_SCHEMA_STATEFUL_SERDE_H

namespace tempo_schema {

    /**
     * @tparam StateType
     */
    template<class PStateType>
    class AbstractAttrParserWithState : public AbstractAttrParser {
    public:
        virtual ~AbstractAttrParserWithState() = default;
        virtual PStateType *getParserState() = 0;
        virtual tempo_utils::Status getHandle(tu_uint32 index, AttrHandle &handle) = 0;
    };

    /**
     * @tparam StateType
     */
    template<class WStateType>
    class AbstractAttrWriterWithState : public AbstractAttrWriter {
    public:
        virtual ~AbstractAttrWriterWithState() = default;
        virtual WStateType *getWriterState() = 0;
        virtual tempo_utils::Result<tu_uint32> putHandle(AttrHandle handle) = 0;
    };

    /**
     *
     */
    template<class ParseType, class StateType>
    class StatefulParsingSerde {
    public:
        virtual Status parseAttr(
            tu_uint32 index,
            AbstractAttrParserWithState<StateType> *parser,
            ParseType &value) const = 0;
    };

    /**
     *
     */
    template<class WriteType, class StateType>
    class StatefulWritingSerde {
    public:
        virtual Result<tu_uint32> writeAttr(
            AbstractAttrWriterWithState<StateType> *writer,
            const WriteType &value) const = 0;
    };
}

#endif // TEMPO_SCHEMA_STATEFUL_SERDE_H
