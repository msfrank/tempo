#ifndef TEMPO_UTILS_VOCABULARY_CACHE_H
#define TEMPO_UTILS_VOCABULARY_CACHE_H

#include <vector>

#include <absl/container/flat_hash_map.h>

#include "schema.h"
#include "url.h"

namespace tempo_utils {

    class VocabularyCache {

    public:
        VocabularyCache();

        bool containsVocabulary(std::string_view nsString) const;
        bool containsVocabulary(Url nsUrl) const;
        bool containsVocabulary(const SchemaNs &ns) const;
        int getVocabularyIndex(std::string_view nsString) const;
        int getVocabularyIndex(Url nsUrl) const;
        int getVocabularyIndex(const SchemaNs &ns) const;
        const ComparableResource *getResource(std::string_view nsString, tu_uint32 idValue) const;
        const ComparableResource *getResource(Url nsUrl, tu_uint32 idValue) const;

        bool addVocabulary(
            std::string_view nsString,
            const ComparableResource * const *resources,
            int numResources);

    private:
        struct VocabularyPriv {
            tempo_utils::Url nsUrl;
            std::string_view nsString;
            const ComparableResource * const *resources;
            tu_uint32 numResources;
        };
        std::vector<VocabularyPriv> m_vocabularies;
        absl::flat_hash_map<std::string_view, int>  m_nsIndex;

    public:
        /**
         * Add the given vocabulary to the cache.
         *
         * @tparam NsType The ns type.
         * @tparam IdType The id type.
         * @param vocabulary A reference to the vocabulary to add.
         * @return true if the vocabulary was added, otherwise false.
         */
        template<class NsType, class IdType>
        bool
        addVocabulary(const SchemaVocabulary<NsType,IdType> &vocabulary)
        {
            std::string_view nsString = vocabulary.getNs().getNs();
            return addVocabulary(nsString, vocabulary.getResources());
        }

        /**
         *
         * @tparam NsType
         * @tparam IdType
         * @param resource
         * @return
         */
        template<class NsType, class IdType>
        int
        getVocabularyIndex(const SchemaResource<NsType, IdType> &resource)
        {
            return getVocabularyIndex(resource.getNsUrl(), resource.getIdValue());
        }
    };
}

#endif // TEMPO_UTILS_VOCABULARY_CACHE_H