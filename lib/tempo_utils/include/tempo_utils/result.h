#ifndef TEMPO_UTILS_RESULT_H
#define TEMPO_UTILS_RESULT_H

#include "log_stream.h"
#include "option_template.h"
#include "status.h"

namespace tempo_utils {

    class IsStatus {

    public:
        IsStatus(bool isStatus);

        bool isStatus() const;

    protected:
        bool m_isStatus;
    };

    template<class S>
    class MaybeStatus : public IsStatus {

    public:
        MaybeStatus();
        MaybeStatus(const S &status);

        S getStatus() const;
        Option<S> statusOption() const;

        using StatusType = S;

    protected:
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
        : IsStatus(false),
          m_status()
    {
    }

    template <class S>
    MaybeStatus<S>::MaybeStatus(const S &status)
        : IsStatus(true),
          m_status(status)
    {
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

    template<class R, class S>
    class TypedResult : public MaybeStatus<S> {

    public:
        explicit TypedResult(const R &result);
        explicit TypedResult(R &&result);
        explicit TypedResult(const S &status);

        bool isResult() const;
        R getResult() const;
        Option<R> resultOption() const;

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
        Result() : TypedResult<T, Status>() {};
        Result(const T &result) : TypedResult<T, Status>(result) {};
        Result(const Status &status) : TypedResult<T, Status>(status) {};
    };

    template<class R, class S>
    tempo_utils::LogMessage&& operator<<(tempo_utils::LogMessage&& message, const TypedResult<R,S> &result) {
        if (result.isResult()) {
            std::forward<tempo_utils::LogMessage>(message) << result.getResult();
        } else {
            std::forward<tempo_utils::LogMessage>(message) << result.getStatus();
        }
        return std::move(message);
    }
}

#endif // TEMPO_UTILS_RESULT_H