
#include <tempo_config/internal/json_array.h>

tempo_config::internal::JsonArray::JsonArray(const ConfigLocation &location)
    : m_location(location)
{
}

tempo_config::internal::JsonArray::~JsonArray()
{
    for (auto &element : m_elements) {
        delete element;
    }
}

tempo_config::internal::JsonNodeType
tempo_config::internal::JsonArray::getNodeType() const
{
    return JsonNodeType::Array;
}

tempo_config::ConfigNode
tempo_config::internal::JsonArray::getConfig() const
{
    std::vector<tempo_config::ConfigNode> seq;
    for (const auto &element : m_elements) {
        seq.push_back(element->getConfig());
    }
    return tempo_config::ConfigSeq(seq, m_location);
}

tempo_config::internal::AbstractJsonNode *
tempo_config::internal::JsonArray::getElement(int index) const
{
    if (0 <= index && index < static_cast<int>(m_elements.size()))
        return m_elements.at(index);
    return nullptr;
}

void
tempo_config::internal::JsonArray::prependElement(AbstractJsonNode *element)
{
    m_elements.insert(m_elements.begin(), element);
}

void
tempo_config::internal::JsonArray::appendElement(AbstractJsonNode *element)
{
    m_elements.push_back(element);
}

std::vector<tempo_config::internal::AbstractJsonNode *>::const_iterator
tempo_config::internal::JsonArray::cbegin() const
{
    return m_elements.cbegin();
}

std::vector<tempo_config::internal::AbstractJsonNode *>::const_iterator
tempo_config::internal::JsonArray::cend() const
{
    return m_elements.cend();
}

int
tempo_config::internal::JsonArray::numElements() const
{
    return m_elements.size();
}
