#ifndef TEMPO_UTILS_UUID_H
#define TEMPO_UTILS_UUID_H

#include <memory>

#include <absl/hash/hash.h>

#include "integer_types.h"
#include "result.h"

namespace tempo_utils {

    class UUID {
    public:
        UUID();

        bool isValid() const;
        bool isNil() const;

        std::string toString() const;

        static UUID nilUUID();
        static UUID randomUUID();
        static UUID parse(std::string_view s);

        bool operator==(const UUID &other) const;
        bool operator!=(const UUID &other) const;

        template <typename H>
        friend H AbslHashValue(H state, const UUID &uuid)
        {
            uuid.hashValue(absl::HashState::Create(&state));
            return std::move(state);
        }

    private:
        struct Priv;
        std::shared_ptr<Priv> m_priv;

        explicit UUID(std::shared_ptr<Priv> priv);
        void hashValue(absl::HashState state) const;
    };
}

#endif // TEMPO_UTILS_UUID_H