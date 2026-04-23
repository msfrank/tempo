#ifndef TEMPO_UTILS_ROPE_CHUNK_H
#define TEMPO_UTILS_ROPE_CHUNK_H

#include <memory>
#include <vector>

#include "log_message.h"

namespace tempo_utils {

    /**
     *
     * @tparam ElementType
     */
    template<class ElementType>
    class RopeChunk {
    public:
        RopeChunk() : m_priv(std::make_unique<Priv>()) {}

        explicit RopeChunk(std::vector<ElementType> &&elements)
            : m_priv(std::make_unique<Priv>(std::move(elements)))
        {
        }

        template<class InputIt>
        RopeChunk(InputIt begin, InputIt end)
            : m_priv(std::make_unique<Priv>(std::vector(begin, end)))
        {
        }

        RopeChunk(RopeChunk &&other) noexcept
            : m_priv(std::move(other.m_priv))
        {
        }

        RopeChunk(const RopeChunk &other)
            : m_priv(other.m_priv)
        {
        }

        RopeChunk& operator=(RopeChunk &&other) noexcept
        {
            if (this != &other) {
                m_priv = std::move(other.m_priv);
            }
            return *this;
        }

        RopeChunk& operator=(const RopeChunk &other)
        {
            if (this != &other) {
                m_priv = other.m_priv;
            }
            return *this;
        }

        bool isEmpty() const { return m_priv->elements.empty(); }

        size_t numElements() const { return m_priv->elements.size(); }

        const ElementType& elementAt(size_t index) const { return m_priv->elements.at(index); }

        ElementType& elementAt(size_t index) { return m_priv->elements.at(index); }

        std::vector<ElementType>::const_iterator cbegin() const { return m_priv->elements.cbegin(); }

        std::vector<ElementType>::const_iterator cend() const { return m_priv->elements.cend(); }

        std::vector<ElementType>::iterator begin() const { return m_priv->elements.begin(); }

        std::vector<ElementType>::iterator end() const { return m_priv->elements.end(); }

        std::pair<RopeChunk,RopeChunk> split(size_t index)
        {
            TU_ASSERT (index < m_priv->elements.size());
            std::vector<ElementType> lhs, rhs;
            for (const auto &e : m_priv->elements) {
                if (index <= lhs.size()) {
                    rhs.push_back(e);
                } else {
                    lhs.push_back(e);
                }
            }
            return std::pair(RopeChunk(std::move(lhs)), RopeChunk(std::move(rhs)));
        }

        std::span<ElementType> subspan(size_t offset, size_t count)
        {
            auto size = m_priv->elements.size();
            if (size <= offset)
                return {};
            return std::span(m_priv->elements.data() + offset, std::min(count, size - offset));
        }

        std::span<ElementType> subspan(size_t offset)
        {
            return subspan(offset, m_priv->elements.size() - offset);
        }

    private:
        struct Priv {
            std::vector<ElementType> elements;
        };
        std::shared_ptr<Priv> m_priv;
    };
}

#endif // TEMPO_UTILS_ROPE_CHUNK_H
