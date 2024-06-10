#ifndef TEMPO_UTILS_ITERATOR_TEMPLATE_H
#define TEMPO_UTILS_ITERATOR_TEMPLATE_H

#include <tempo_utils/option_template.h>

template <class ValueType>
class Iterator {
public:
    virtual ~Iterator() = default;

    virtual bool getNext(ValueType &value) = 0;
    virtual Option<ValueType> maybeNext();
};

template<class ValueType>
Option<ValueType>
Iterator<ValueType>::maybeNext()
{
    ValueType value;
    if (getNext(value))
        return Option<ValueType>(value);
    return Option<ValueType>();
}

#endif // TEMPO_UTILS_ITERATOR_TEMPLATE_H
