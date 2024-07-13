#ifndef TEMPO_UTILS_RESULT_H
#define TEMPO_UTILS_RESULT_H

#include "log_stream.h"
#include "option_template.h"
#include "status.h"

namespace tempo_utils {

    template<class S>
    class MaybeStatus {

    public:
        MaybeStatus();
        MaybeStatus(const S &status);
        MaybeStatus(const MaybeStatus<S> &other);
        MaybeStatus(MaybeStatus<S> &&other) noexcept;

        MaybeStatus& operator=(const MaybeStatus<S> &other);
        MaybeStatus& operator=(MaybeStatus<S> &&other) noexcept;

        bool isStatus() const;
        S getStatus() const;
        Option<S> statusOption() const;
        const S& peekStatus() const;

        using StatusType = S;

    protected:
        bool m_isStatus;
        S m_status;

    public:
        /**
         *
         * @tparam StatusType
         * @tparam ConditionType
         * @param status
         * @return
         */
        template <typename StatusType>
        bool convertTo(StatusType &status)
        {
            if (!isStatus())
                return false;
            return StatusTraits<StatusType>::convert(status, m_status);
        };

        template <typename ConditionType,
            typename StatusType = typename ConditionTraits<ConditionType>::StatusType>
        bool matchesCondition(ConditionType condition)
        {
            StatusType status;
            if (!convertTo(status))
                return false;
            return status.getCondition() == condition;
        };
    };

    template <class S>
    MaybeStatus<S>::MaybeStatus()
        : m_isStatus(false),
          m_status()
    {
    }

    template <class S>
    MaybeStatus<S>::MaybeStatus(const S &status)
        : m_isStatus(true),
          m_status(status)
    {
    }

    template <class S>
    MaybeStatus<S>::MaybeStatus(const MaybeStatus<S> &other)
        : m_isStatus(other.m_isStatus),
          m_status(other.m_status)
    {
    }

    template <class S>
    MaybeStatus<S>::MaybeStatus(MaybeStatus<S> &&other) noexcept
    {
        m_isStatus = other.m_isStatus;
        m_status = std::move(other.m_status);
    }

    template <class S>
    MaybeStatus<S>&
    MaybeStatus<S>::operator=(const MaybeStatus<S> &other)
    {
        if (this != &other) {
            m_isStatus = other.m_isStatus;
            m_status = other.m_status;
        }
        return *this;
    }

    template <class S>
    MaybeStatus<S>&
    MaybeStatus<S>::operator=(MaybeStatus<S> &&other) noexcept
    {
        if (this != &other) {
            m_isStatus = other.m_isStatus;
            m_status = std::move(other.m_status);
        }
        return *this;
    }

    template <class S>
    bool
    MaybeStatus<S>::isStatus() const
    {
        return m_isStatus;
    }

    template <class S>
    S
    MaybeStatus<S>::getStatus() const
    {
        return m_status;
    }

    template <class S>
    Option<S>
    MaybeStatus<S>::statusOption() const
    {
        return m_isStatus? Option<S>(m_status) : Option<S>();
    }

    template <class S>
    const S&
    MaybeStatus<S>::peekStatus() const
    {
        return m_status;
    }

    template<class R, class S>
    class TypedResult : public MaybeStatus<S> {

    public:
        explicit TypedResult(const R &result);
        explicit TypedResult(R &&result);
        explicit TypedResult(const S &status);
        TypedResult(const TypedResult<R,S> &other);
        TypedResult(TypedResult<R,S> &&other) noexcept;

        TypedResult<R,S>& operator=(const TypedResult<R,S> &other);
        TypedResult<R,S>& operator=(TypedResult<R,S> &&other) noexcept;

        bool isResult() const;
        R getResult() const;
        Option<R> resultOption() const;
        const R& peekResult() const;
        R&& moveResult();

        R orElse(const R &value) const;
        R orElseThrow() const;

        using ResultType = R;

    private:
        R m_result;
    };

    template <class R, class S>
    TypedResult<R,S>::TypedResult(const R &result)
        : MaybeStatus<S>(),
          m_result(result)
    {
    }

    template <class R, class S>
    TypedResult<R,S>::TypedResult(R &&result)
        : MaybeStatus<S>(),
          m_result(std::move(result))
    {
    }

    template <class R, class S>
    TypedResult<R,S>::TypedResult(const S &status)
        : MaybeStatus<S>(status),
          m_result()
    {
    }

    template <class R, class S>
    TypedResult<R,S>::TypedResult(const TypedResult<R,S> &other)
        : MaybeStatus<S>(other),
          m_result(other.m_result)
    {
    }

    template <class R, class S>
    TypedResult<R,S>::TypedResult(TypedResult<R,S> &&other) noexcept
        : MaybeStatus<S>(std::move(other))
    {
        m_result = std::move(other.m_result);
    }

    template <class R, class S>
    TypedResult<R,S>&
    TypedResult<R,S>::operator=(const TypedResult<R,S> &other)
    {
        if (this != &other) {
            MaybeStatus<S>::operator=(other);
            m_result = other.m_result;
        }
        return *this;
    }

    template <class R, class S>
    TypedResult<R,S>&
    TypedResult<R,S>::operator=(TypedResult<R,S> &&other) noexcept
    {
        if (this != &other) {
            MaybeStatus<S>::operator=(std::move(other));
            m_result = std::move(other.m_result);
        }
        return *this;
    }

    template <class R, class S>
    bool
    TypedResult<R,S>::isResult() const
    {
        return !MaybeStatus<S>::m_isStatus;
    }

    template <class R, class S>
    R
    TypedResult<R,S>::getResult() const
    {
        return m_result;
    }

    template <class R, class S>
    Option<R>
    TypedResult<R,S>::resultOption() const
    {
        if (MaybeStatus<S>::m_isStatus)
            return Option<R>();
        return Option<R>(m_result);
    }

    template <class R, class S>
    const R&
    TypedResult<R,S>::peekResult() const
    {
        return m_result;
    }

    template <class R, class S>
    R&&
    TypedResult<R,S>::moveResult()
    {
        return std::move(m_result);
    }

    template <class R, class S>
    R
    TypedResult<R,S>::orElse(const R &value) const
    {
        if (!MaybeStatus<S>::m_isStatus)
            return m_result;
        return value;
    }

    template <class R, class S>
    R
    TypedResult<R,S>::orElseThrow() const
    {
        if (!MaybeStatus<S>::m_isStatus)
            return m_result;
        throw StatusException(MaybeStatus<S>::m_status);
    }

    template<typename T>
    class Result final : public TypedResult<T,Status> {
    public:
        Result();
        Result(const T &result);
        Result(T &&result);
        Result(const Status &status);
        Result(const Result<T> &other);
        Result(Result<T> &&other) noexcept;

        Result<T>& operator=(const Result<T> &other);
        Result<T>& operator=(Result<T> &&other) noexcept;
    };

    template<typename T>
    Result<T>::Result()
        : TypedResult<T, Status>()
    {
    }

    template<typename T>
    Result<T>::Result(const T &result)
        : TypedResult<T, Status>(result)
    {
    }

    template<typename T>
    Result<T>::Result(T &&result)
        : TypedResult<T, Status>(std::move(result))
    {
    }

    template<typename T>
    Result<T>::Result(const Status &status)
        : TypedResult<T, Status>(status)
    {
    }

    template<typename T>
    Result<T>::Result(const Result<T> &other)
        : TypedResult<T, Status>(other)
    {
    }

    template<typename T>
    Result<T>::Result(Result<T> &&other) noexcept
        : TypedResult<T, Status>(std::move(other))
    {
    }

    template<typename T>
    Result<T>& Result<T>::operator=(const Result<T> &other)
    {
        if (this != &other) {
            TypedResult<T,Status>::operator=(other);
        }
        return *this;
    }

    template<typename T>
    Result<T>& Result<T>::operator=(Result<T> &&other) noexcept
    {
        if (this != &other) {
            TypedResult<T,Status>::operator=(std::move(other));
        }
        return *this;
    }

    template<class R, class S>
    LogMessage&& operator<<(LogMessage&& message, const TypedResult<R,S> &result) {
        if (result.isResult()) {
            std::forward<LogMessage>(message) << result.getResult();
        } else {
            std::forward<LogMessage>(message) << result.getStatus();
        }
        return std::move(message);
    }
}

#endif // TEMPO_UTILS_RESULT_H