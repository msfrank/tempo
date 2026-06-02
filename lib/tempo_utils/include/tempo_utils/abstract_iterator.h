#ifndef TEMPO_UTILS_ABSTRACT_ITERATOR_H
#define TEMPO_UTILS_ABSTRACT_ITERATOR_H

namespace tempo_utils {

    /**
     * Interface describing an iterator.
     *
     * @tparam ValueType The value type.
     */
    template <class ValueType>
    class AbstractIterator {
    public:
        virtual ~AbstractIterator() = default;

        /**
         * Returns whether the iterator has another value available.
         *
         * @return true if another value is available, otherwise false.
         */
        virtual bool hasNext() const = 0;

        /**
         * Consumes the next value from the iterator if one is available and places it in `value`.
         *
         * @param value Mutable reference which will contain the next value if one is available.
         * @return true if a value was consumed, otherwise false.
         */
        virtual bool getNext(ValueType &value) = 0;
    };
}

#endif // TEMPO_UTILS_ABSTRACT_ITERATOR_H
