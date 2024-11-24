#ifndef TEMPO_SECURITY_SHA256_HASH_H
#define TEMPO_SECURITY_SHA256_HASH_H

#include <string_view>

namespace tempo_security {

    class Sha256Hash {

    public:
        Sha256Hash();
        ~Sha256Hash();

        bool addData(std::string_view data);
        std::string getResult();

        static std::string hash(std::string_view data);

    private:
        void *m_ctx;
        bool m_dirty;
    };
}

#endif // TEMPO_SECURITY_SHA256_HASH_H
