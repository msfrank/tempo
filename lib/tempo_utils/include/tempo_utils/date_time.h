#ifndef TEMPO_UTILS_DATE_TIME_H
#define TEMPO_UTILS_DATE_TIME_H

#include "integer_types.h"

namespace tempo_utils {

    tu_int64 seconds_since_epoch();

    tu_int64 millis_since_epoch();

    class Duration {
    public:
        Duration();
        Duration(const Duration &other);

        bool isValid() const;

        tu_int64 getNanos() const;
        tu_int64 getMicros() const;
        tu_int64 getMillis() const;
        tu_int64 getSeconds() const;
        tu_int64 getMinutes() const;
        tu_int64 getHours() const;

        static Duration nanos(tu_int64 nanos);
        static Duration micros(tu_int64 millis);
        static Duration millis(tu_int64 millis);
        static Duration seconds(tu_int64 seconds);
        static Duration minutes(tu_int64 minutes);
        static Duration hours(tu_int64 hours);

        Duration operator+(const Duration &other) const;
        Duration operator-(const Duration &other) const;

    private:
        struct Priv;
        std::shared_ptr<Priv> m_priv;

        explicit Duration(std::shared_ptr<Priv> priv);

        friend class Instant;
    };

    class Timezone {
    public:
        Timezone();
        Timezone(const Timezone &other);

        bool isValid() const;

        std::string getName() const;

        static Timezone load(std::string_view tzName);

    private:
        struct Priv;
        std::shared_ptr<Priv> m_priv;

        explicit Timezone(std::shared_ptr<Priv> priv);
    };

    class Instant {
    public:
        Instant();
        Instant(const Instant &other);

        bool isValid() const;

        tu_int64 getEpochSeconds() const;
        tu_int64 getEpochMillis() const;

        Instant operator+(const Duration &d) const;
        Instant operator-(const Duration &d) const;

        static Instant now();
        static Instant fromEpochSeconds(tu_int64 epochSeconds);
        static Instant fromEpochMillis(tu_int64 epochMillis);

    private:
        struct Priv;
        std::shared_ptr<Priv> m_priv;

        explicit Instant(std::shared_ptr<Priv> priv);
    };
};

#endif // TEMPO_UTILS_DATE_TIME_H
