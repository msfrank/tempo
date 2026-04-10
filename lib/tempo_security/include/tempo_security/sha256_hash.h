#ifndef TEMPO_SECURITY_SHA256_HASH_H
#define TEMPO_SECURITY_SHA256_HASH_H

#include <span>
#include <string_view>
#include <vector>

#include "tempo_utils/integer_types.h"

namespace tempo_security {

    class Sha256Hash {

    public:
        Sha256Hash();
        ~Sha256Hash();

        bool addData(const char *data, size_t size);
        bool addData(std::string_view data);
        bool addData(const tu_uint8 *data, size_t size);
        bool addData(std::span<const tu_uint8> data);
        bool addData(const std::vector<tu_uint8> &data);
        std::string getResult();

        static std::string hash(std::string_view data);

    private:
        void *m_ctx;
        bool m_dirty;
    };
}

#endif // TEMPO_SECURITY_SHA256_HASH_H
