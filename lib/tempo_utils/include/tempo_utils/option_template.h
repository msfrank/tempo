#ifndef TEMPO_UTILS_OPTION_TEMPLATE_H
#define TEMPO_UTILS_OPTION_TEMPLATE_H

#include "log_stream.h"

template<class T>
class Option {

public:
    Option();
    explicit Option(const T &value);
    Option(const Option<T> &other);
    Option(Option<T> &&other) noexcept;

    Option<T>& operator=(const Option<T> &other);
    Option<T>& operator=(Option<T> &&other) noexcept;

    bool isEmpty() const;
    bool hasValue() const;
    T getValue() const;
    T getOrDefault(const T &value) const;
    const T& peekValue() const;
    T&& moveValue();

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
Option<T>::Option(const Option<T> &other)
    : m_empty(other.m_empty),
      m_value(other.m_value)
{
}

template<class T>
Option<T>::Option(Option<T> &&other) noexcept
{
    m_empty = other.m_empty;
    m_value = std::move(other.m_value);
    other.m_empty = true;
}

template<class T>
Option<T>&
Option<T>::operator=(const Option<T> &other)
{
    if (this != &other) {
        m_empty = other.m_empty;
        m_value = other.m_value;
    }
    return *this;
}

template<class T>
Option<T>&
Option<T>::operator=(Option<T> &&other) noexcept
{
    if (this != &other) {
        m_empty = other.m_empty;
        m_value = std::move(other.m_value);
        other.m_empty = true;
    }
    return *this;
}

template<class T>
bool
Option<T>::isEmpty() const
{
    return m_empty;
}

template<class T>
bool
Option<T>::hasValue() const
{
    return !m_empty;
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
const T&
Option<T>::peekValue() const
{
    return m_value;
}

template<class T>
T&&
Option<T>::moveValue()
{
    return std::move(m_value);
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

#endif // TEMPO_UTILS_OPTION_TEMPLATE_H
