
#include <tempo_schema/vocabulary_cache.h>

tempo_schema::VocabularyCache::VocabularyCache()
{
}

bool
tempo_schema::VocabularyCache::containsVocabulary(std::string_view nsString) const
{
    return m_nsIndex.contains(nsString);
}

bool
tempo_schema::VocabularyCache::containsVocabulary(tempo_utils::Url nsUrl) const
{
    return containsVocabulary(nsUrl.uriView());
}

bool
tempo_schema::VocabularyCache::containsVocabulary(const SchemaNs &ns) const
{
    return containsVocabulary(std::string_view(ns.getNs()));
}

int
tempo_schema::VocabularyCache::getVocabularyIndex(std::string_view nsString) const
{
    if (!m_nsIndex.contains(nsString))
        return -1;
    return m_nsIndex.at(nsString);
}

int
tempo_schema::VocabularyCache::getVocabularyIndex(tempo_utils::Url nsUrl) const
{
    return getVocabularyIndex(nsUrl.uriView());
}

int
tempo_schema::VocabularyCache::getVocabularyIndex(const SchemaNs &ns) const
{
    return getVocabularyIndex(std::string_view(ns.getNs()));
}

const tempo_schema::ComparableResource *
tempo_schema::VocabularyCache::getResource(std::string_view nsString, tu_uint32 idValue) const
{
    auto index = getVocabularyIndex(nsString);
    if (index < 0)
        return nullptr;
    const auto &priv = m_vocabularies.at(index);
    if (priv.numResources <= idValue)
        return nullptr;
    return priv.resources[idValue];
}

const tempo_schema::ComparableResource *
tempo_schema::VocabularyCache::getResource(tempo_utils::Url nsUrl, tu_uint32 idValue) const
{
    return getResource(nsUrl.uriView(), idValue);
}

bool
tempo_schema::VocabularyCache::addVocabulary(
    std::string_view nsString,
    const ComparableResource * const *resources,
    int numResources)
{
    if (m_nsIndex.contains(nsString))
        return false;
    VocabularyPriv priv;
    priv.nsUrl = tempo_utils::Url::fromString(nsString);
    priv.nsString = priv.nsUrl.uriView();
    priv.resources = resources;
    auto vocabularyIndex = m_vocabularies.size();
    m_vocabularies.emplace_back(std::move(priv));
    m_nsIndex[priv.nsString] = vocabularyIndex;
    return true;
}
