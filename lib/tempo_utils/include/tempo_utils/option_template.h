#ifndef OPTION_TEMPLATE_H
#define OPTION_TEMPLATE_H

#include "log_stream.h"

template<class T>
class Option {

public:
    Option();
    explicit Option(const T &value);

    bool isEmpty() const;
    T getValue() const;
    T getOrDefault(const T &value) const;

private:
    bool m_empty;
    T m_value;
};

template<class T>
Option<T>::Option()
{
    m_empty = true;
}

template<class T>
Option<T>::Option(const T &value)
{
    m_empty = false;
    m_value = value;
}

template<class T>
bool
Option<T>::isEmpty() const
{
    return m_empty;
}

template<class T>
T
Option<T>::getValue() const
{
    return m_value;
}

template<class T>
T
Option<T>::getOrDefault(const T &value) const
{
    if (!m_empty)
        return m_value;
    return value;
}

template<class T>
tempo_utils::LogMessage&& operator<<(tempo_utils::LogMessage&& message, const Option<T> &option) {
    if (!option.isEmpty()) {
        std::forward<tempo_utils::LogMessage>(message) << "Option(" << option.getValue() << ")";
    } else {
        std::forward<tempo_utils::LogMessage>(message) << "Option()";
    }
    return std::move(message);
}

template<class T>
inline bool operator==(const Option<T> &lhs, const Option<T> &rhs)
{
    if (lhs.isEmpty() && rhs.isEmpty())
        return true;
    if (lhs.isEmpty() && !rhs.isEmpty())
        return false;
    if (!lhs.isEmpty() && rhs.isEmpty())
        return false;
    return lhs.getValue() == rhs.getValue();
}

template<class T>
inline bool operator!=(const Option<T> &lhs, const Option<T> &rhs)
{
    return !(lhs == rhs);
}

#endif // OPTION_TEMPLATE_H
