#ifndef TEMPO_CONFIG_CONFIG_PATH_H
#define TEMPO_CONFIG_CONFIG_PATH_H

#include <string>
#include <memory>
#include <vector>

namespace tempo_config {

    enum class ConfigPathPartType {
        Invalid,
        Root,
        Index,
        Key,
    };

    class ConfigPathPart {
    public:
        ConfigPathPart();
        explicit ConfigPathPart(int index);
        explicit ConfigPathPart(std::string_view key);
        ConfigPathPart(const ConfigPathPart &other);

        ConfigPathPartType getType() const;
        int getIndex();
        std::string getKey();

    private:
        struct Priv {
            ConfigPathPartType type;
            int index;
            std::string key;
        };
        std::shared_ptr<Priv> m_priv;

        ConfigPathPart(std::shared_ptr<Priv> priv);
        static ConfigPathPart root();

        friend class ConfigPath;
    };

    class ConfigPath {
    public:
        ConfigPath();
        ConfigPath(const ConfigPath &other);

        bool isValid() const;

        ConfigPath traverse(const ConfigPathPart &part) const;

        static ConfigPath root();

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