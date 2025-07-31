
#include <absl/time/time.h>
#include <absl/time/clock.h>

#include <tempo_utils/date_time.h>
#include <tempo_utils/log_message.h>

int64_t tempo_utils::seconds_since_epoch()
{
    return ToUnixSeconds(absl::Now());
}

int64_t tempo_utils::millis_since_epoch()
{
    return ToUnixMillis(absl::Now());
}

struct tempo_utils::Duration::Priv {
    absl::Duration d;
};

tempo_utils::Duration::Duration()
{
}

tempo_utils::Duration::Duration(std::shared_ptr<Priv> priv)
    : m_priv(std::move(priv))
{
    TU_ASSERT (m_priv != nullptr);
}

tempo_utils::Duration::Duration(const Duration &other)
    : m_priv(other.m_priv)
{
}

bool
tempo_utils::Duration::isValid() const
{
    return m_priv != nullptr;
}

tu_int64
tempo_utils::Duration::getNanos() const
{
    if (isValid())
        return absl::ToInt64Nanoseconds(m_priv->d);
    return 0;
}

tu_int64
tempo_utils::Duration::getMicros() const
{
    if (isValid())
        return absl::ToInt64Microseconds(m_priv->d);
    return 0;
}

tu_int64
tempo_utils::Duration::getMillis() const
{
    if (isValid())
        return absl::ToInt64Milliseconds(m_priv->d);
    return 0;
}

tu_int64
tempo_utils::Duration::getSeconds() const
{
    if (isValid())
        return ToInt64Seconds(m_priv->d);
    return 0;
}

tu_int64
tempo_utils::Duration::getMinutes() const
{
    if (isValid())
        return absl::ToInt64Minutes(m_priv->d);
    return 0;
}

tu_int64
tempo_utils::Duration::getHours() const
{
    if (isValid())
        return ToInt64Hours(m_priv->d);
    return 0;
}

tempo_utils::Duration
tempo_utils::Duration::nanos(tu_int64 nanos)
{
    return Duration(std::make_shared<Priv>(absl::Nanoseconds(nanos)));
}

tempo_utils::Duration
tempo_utils::Duration::micros(tu_int64 micros)
{
    return Duration(std::make_shared<Priv>(absl::Microseconds(micros)));
}

tempo_utils::Duration
tempo_utils::Duration::millis(tu_int64 millis)
{
    return Duration(std::make_shared<Priv>(absl::Milliseconds(millis)));
}

tempo_utils::Duration
tempo_utils::Duration::seconds(tu_int64 seconds)
{
    return Duration(std::make_shared<Priv>(absl::Seconds(seconds)));
}

tempo_utils::Duration
tempo_utils::Duration::minutes(tu_int64 minutes)
{
    return Duration(std::make_shared<Priv>(absl::Minutes(minutes)));
}

tempo_utils::Duration
tempo_utils::Duration::hours(tu_int64 hours)
{
    return Duration(std::make_shared<Priv>(absl::Hours(hours)));
}

tempo_utils::Duration
tempo_utils::Duration::operator+(const Duration &other) const
{
    if (!isValid())
        return other;
    if (!other.isValid())
        return *this;
    auto d = m_priv->d + other.m_priv->d;
    return Duration(std::make_shared<Priv>(d));
}

tempo_utils::Duration
tempo_utils::Duration::operator-(const Duration &other) const
{
    absl::Duration d;
    if (isValid()) {
        if (!other.isValid())
            return *this;
        d = m_priv->d - other.m_priv->d;
    } else {
        if (!other.isValid())
            return *this;
        d = absl::Duration{} - other.m_priv->d;
    }
    return Duration(std::make_shared<Priv>(d));
}

struct tempo_utils::Timezone::Priv {
    absl::TimeZone tz;
};

tempo_utils::Timezone::Timezone()
{
}

tempo_utils::Timezone::Timezone(std::shared_ptr<Priv> priv)
    : m_priv(priv)
{
    TU_ASSERT (m_priv != nullptr);
}

tempo_utils::Timezone::Timezone(const Timezone &other)
    : m_priv(other.m_priv)
{
}

bool
tempo_utils::Timezone::isValid() const
{
    return m_priv != nullptr;
}

std::string
tempo_utils::Timezone::getName() const
{
    if (!isValid())
        return {};
    return m_priv->tz.name();
}

tempo_utils::Timezone
tempo_utils::Timezone::load(std::string_view tzName)
{
    absl::TimeZone tz;
    if (!absl::LoadTimeZone(tzName, &tz))
        return {};
    return Timezone(std::make_shared<Priv>(tz));
}

struct tempo_utils::Instant::Priv {
    absl::Time t;
};

tempo_utils::Instant::Instant()
{
}

tempo_utils::Instant::Instant(std::shared_ptr<Priv> priv)
    : m_priv(std::move(priv))
{
}

tempo_utils::Instant::Instant(const Instant &other)
    : m_priv(other.m_priv)
{
}

bool
tempo_utils::Instant::isValid() const
{
    return m_priv != nullptr;
}

tu_int64
tempo_utils::Instant::getEpochSeconds() const
{
    if (isValid())
        return ToUnixSeconds(m_priv->t);
    return 0;
}

tu_int64
tempo_utils::Instant::getEpochMillis() const
{
    if (isValid())
        return absl::ToUnixMillis(m_priv->t);
    return 0;
}

tempo_utils::Instant
tempo_utils::Instant::operator+(const Duration &d) const
{
    if (!isValid())
        return {};
    if (!d.isValid())
        return *this;
    return Instant(std::make_shared<Priv>(m_priv->t + d.m_priv->d));
}

tempo_utils::Instant
tempo_utils::Instant::operator-(const Duration &d) const
{
    if (!isValid())
        return {};
    if (!d.isValid())
        return *this;
    return Instant(std::make_shared<Priv>(m_priv->t - d.m_priv->d));
}

tempo_utils::Instant
tempo_utils::Instant::now()
{
    return Instant(std::make_shared<Priv>(absl::Now()));
}

tempo_utils::Instant
tempo_utils::Instant::fromEpochSeconds(tu_int64 epochSeconds)
{
    return Instant(std::make_shared<Priv>(absl::FromUnixSeconds(epochSeconds)));
}

tempo_utils::Instant
tempo_utils::Instant::fromEpochMillis(tu_int64 epochMillis)
{
    return Instant(std::make_shared<Priv>(absl::FromUnixMillis(epochMillis)));
}