#ifndef TEMPO_CONFIG_CONFIG_PATH_H
#define TEMPO_CONFIG_CONFIG_PATH_H

#include <string>
#include <memory>
#include <vector>

namespace tempo_config {

    enum class ConfigPathPartType {
        Invalid,
        Index,
        Key,
    };

    class ConfigPathPart {
    public:
        ConfigPathPart();
        explicit ConfigPathPart(int index);
        explicit ConfigPathPart(std::string_view key);
        ConfigPathPart(const ConfigPathPart &other);

        bool isValid() const;
        ConfigPathPartType getType() const;
        int getIndex() const;
        std::string getKey() const;

    private:
        struct Priv {
            ConfigPathPartType type;
            int index;
            std::string key;
        };
        std::shared_ptr<Priv> m_priv;

        ConfigPathPart(std::shared_ptr<Priv> priv);

        friend class ConfigPath;
    };

    class ConfigPath {
    public:
        ConfigPath();
        ConfigPath(std::vector<ConfigPathPart> &&parts);
        ConfigPath(const ConfigPath &other);

        bool isRoot() const;

        std::vector<ConfigPathPart>::const_iterator partsBegin() const;
        std::vector<ConfigPathPart>::const_iterator partsEnd() const;
        int numParts() const;

        ConfigPath traverse(const ConfigPathPart &part) const;
        ConfigPath parent() const;

    private:
        struct Priv {
            std::vector<ConfigPathPart> parts;
        };
        std::shared_ptr<Priv> m_priv;

        ConfigPath(std::shared_ptr<Priv> priv);

    public:
        template<class... Args>
        ConfigPath traverse(const ConfigPathPart &part, Args... args) const
        {
            auto traversed = traverse(part);
            return traversed.traverse(args...);
        }
        template<class... Args>
        ConfigPath traverse(int index, Args... args) const
        {
            auto traversed = traverse(ConfigPathPart(index));
            return traversed.traverse(args...);
        }
        template<class... Args>
        ConfigPath traverse(std::string_view key, Args... args) const
        {
            auto traversed = traverse(ConfigPathPart(key));
            return traversed.traverse(args...);
        }
        ConfigPath traverse() const
        {
            return *this;
        }

    };
}

#endif // TEMPO_CONFIG_CONFIG_PATH_H