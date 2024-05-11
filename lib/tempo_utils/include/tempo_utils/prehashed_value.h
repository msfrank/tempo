#ifndef TEMPO_UTILS_PREHASHED_VALUE_H
#define TEMPO_UTILS_PREHASHED_VALUE_H

#include <cstddef>
#include <memory>

#include <absl/hash/hash.h>

namespace tempo_utils {

    template <class T>
    class PrehashedValue {
    public:
        /**
         * Constructs an empy PrehashedValue instance.
         */
        PrehashedValue() : m_empty(true), m_hash(0) {};

        /**
         * Constructs a PrehashedValue instance containing value.
         *
         * @param value
         */
        PrehashedValue(T &&value) : m_empty(false), m_value(std::move(value))
        {
            m_hash = absl::HashOf(m_value);
        }

        /**
         * Copy constructor.
         */
        PrehashedValue(const PrehashedValue<T> &other)
            : m_empty(other.m_empty),
              m_value(other.m_value),
              m_hash(other.m_hash)
        {
        }

        /**
         * Move constructor.
         */
        PrehashedValue(PrehashedValue<T> &&other) noexcept
        {
            m_empty = other.m_empty;
            m_value = std::move(other.m_value);
            m_hash = other.m_hash;
        }

        /**
         * Copy assignment.
         */
        PrehashedValue<T>& operator=(const PrehashedValue<T> &other)
        {
            if (this != &other) {
                m_empty = other.m_empty;
                m_value = other.m_value;
                m_hash = other.m_hash;
            }
            return *this;
        }

        /**
         * Move assignment.
         */
        PrehashedValue<T>& operator=(PrehashedValue<T> &&other) noexcept
        {
            if (this != &other) {
                m_empty = other.m_empty;
                m_value = std::move(other.m_value);
                m_hash = other.m_hash;
            }
            return *this;
        }

        /**
         * Returns whether the PrehashedValue is empty.
         *
         * @return true if the PrehashedValue is empty, otherwise false.
         */
        bool isEmpty() const
        {
            return m_empty;
        }

        /**
         * Returns whether the PrehashedValue is non-empty.
         *
         * @return true if the PrehashedValue is non-empty, otherwise false.
         */
        bool nonEmpty() const
        {
            return !m_empty;
        }

        /**
         * Returns a reference to the value of the PrehashedValue. If the PrehashedValue is empty, then the reference
         * returned is uninitialized.
         *
         * @return The value reference.
         */
        T& operator*()
        {
            return m_value;
        }

        const T& operator*() const
        {
            return m_value;
        }

        /**
         * Returns a pointer to the value of the PrehashedValue. If the PrehashedValue is empty, then nullptr is returned.
         *
         * @return The value pointer, or nullptr if the PrehashedValue is empty.
         */
        T* operator->()
        {
            return &m_value;
        }

        const T* operator->() const
        {
            return &m_value;
        }

        /**
         * Returns whether the PrehashedValue is equal to the given PrehashedValue. If both instances are empty then they
         * are considered equal.
         *
         * @return true if the instances are equal, otherwise false.
         */
        bool operator==(const PrehashedValue<T> &other) const
        {
            if (m_empty && other.m_empty)           // if both instances are empty then they are equal
                return true;
            if (!m_empty) {
                if (other.m_empty)                  // if other instance is empty, then they are not equal
                    return false;
                if (m_hash != other.m_hash)         // if the hashes are not equal then they are not equal
                    return false;
                return m_value == other.m_value;    // otherwise test whether values are equal
            }
            return false;                           // if this instance is empty then they are not equal
        }

        /**
         * Hash the prehashed value.
         */
        template <typename H>
        friend H AbslHashValue(H h, const PrehashedValue<T> &c) {
            return H::combine(std::move(h), c.m_hash);
        }

    private:
        bool m_empty;
        T m_value;
        size_t m_hash;
    };


    template<class ValueType, class... Params>
    PrehashedValue<ValueType> make_prehashed(Params&&... params)
    {
        ValueType value(std::forward<Params>(params)...);
        return PrehashedValue<ValueType>(std::move(value));
    }
}

#endif // TEMPO_UTILS_PREHASHED_VALUE_H
