#ifndef TEMPO_CONFIG_CONFIG_NODE_H
#define TEMPO_CONFIG_CONFIG_NODE_H

#include <filesystem>
#include <string>
#include <vector>

#include <absl/container/btree_map.h>
#include <absl/container/flat_hash_map.h>

#include <tempo_utils/integer_types.h>

namespace tempo_config {

    constexpr const char *kDefaultWorkspaceConfigFileName       = "workspace.config";
    constexpr const char *kDefaultConfigDirectoryName           = "config";
    constexpr const char *kDefaultVendorConfigDirectoryName     = "vendor-config";

    enum class ConfigSourceType {
        Invalid,
        File,
        CommandLine,
        Environment,
    };

    class ConfigSource {
    public:
        ConfigSource();
        ConfigSource(ConfigSourceType type, std::string_view source);
        ConfigSource(const ConfigSource &other);

        bool isValid() const;

        ConfigSourceType getType() const;
        std::string getSource() const;
        std::string_view sourceView() const;

    private:
        ConfigSourceType m_type;
        std::string m_source;
    };

    class ConfigLocation {
    public:
        ConfigLocation();
        ConfigLocation(
            std::shared_ptr<ConfigSource> source,
            tu_uint32 lineNumber,
            tu_uint32 columnNumber,
            tu_uint32 textOffset,
            tu_uint32 textSpan);
        ConfigLocation(const ConfigLocation &other);

        std::shared_ptr<ConfigSource> getSource() const;
        tu_uint32 getLineNumber() const;
        tu_uint32 getColumnNumber() const;
        tu_uint32 getTextOffset() const;
        tu_uint32 getTextSpan() const;

        std::string toString() const;

    private:
        std::shared_ptr<ConfigSource> m_source;
        tu_uint32 m_lineNumber;
        tu_uint32 m_columnNumber;
        tu_uint32 m_textOffset;
        tu_uint32 m_textSpan;
    };

    enum class Reformat {
        None,
        OnlyChanged,
        Full,
    };

    struct PrinterOptions {
        int indent = 2;
        Reformat reformat = Reformat::OnlyChanged;
    };

    enum class ConfigNodeType {
        kNil = 0,
        kValue = 1,
        kSeq = 2,
        kMap = 3,
    };

    const char *config_node_type_to_string(ConfigNodeType type);

    struct ConfigNodeData;

    class ConfigNode;
    class ConfigNil;
    class ConfigValue;
    class ConfigSeq;
    class ConfigMap;

    class ConfigNode {
    public:
        ConfigNode();
        ConfigNode(const ConfigNode &other);
        ConfigNode(ConfigNode &&other) noexcept;
        ConfigNode& operator=(const ConfigNode &other);
        ConfigNode& operator=(ConfigNode &&other) noexcept;

        bool isNil() const;
        ConfigNodeType getNodeType() const;
        ConfigLocation getNodeLocation() const;

        bool operator==(const ConfigNode &other) const;
        bool operator!=(const ConfigNode &other) const;
        bool operator<(const ConfigNode &other) const;

        ConfigValue toValue() const;
        ConfigSeq toSeq() const;
        ConfigMap toMap() const;
        ConfigNode toNode() const;

        std::string toString() const;

        int compare(const ConfigNode &other) const;

        void hash(absl::HashState state) const;

        template <typename H>
        friend H AbslHashValue(H state, const ConfigNode &node) {
            node.hash(absl::HashState::Create(&state));
            return std::move(state);
        }

    protected:
        std::shared_ptr<ConfigNodeData> m_data;

        explicit ConfigNode(std::shared_ptr<ConfigNodeData> data);
    };

    struct ConfigNodeData {
        ConfigNodeType type;
        ConfigLocation location;
        std::string value;
        std::vector<ConfigNode> seq;
        absl::btree_map<std::string,ConfigNode> map;
    };

    class ConfigNil : public ConfigNode {
    public:
        ConfigNil();
        explicit ConfigNil(const ConfigLocation &location);

        bool operator==(const ConfigNil &other) const;
        bool operator!=(const ConfigNil &other) const;
        bool operator<(const ConfigNil &other) const;

    private:
        explicit ConfigNil(std::shared_ptr<ConfigNodeData> data);
        friend class ConfigNode;
    };

    class ConfigValue : public ConfigNode {
    public:
        ConfigValue();
        explicit ConfigValue(const char *value, const ConfigLocation &location = {});
        explicit ConfigValue(std::string_view value, const ConfigLocation &location = {});
        explicit ConfigValue(std::string &&value, const ConfigLocation &location = {});

        std::string getValue() const;

        bool operator==(const ConfigValue &other) const;
        bool operator!=(const ConfigValue &other) const;
        bool operator<(const ConfigValue &other) const;

    private:
        explicit ConfigValue(std::shared_ptr<ConfigNodeData> data);
        friend class ConfigNode;
    };

    class ConfigSeq : public ConfigNode {
    public:
        ConfigSeq();
        explicit ConfigSeq(
            const std::vector<ConfigNode> &seq,
            const ConfigLocation &location = {});
        explicit ConfigSeq(
            std::vector<ConfigNode> &&seq,
            const ConfigLocation &location = {});
        explicit ConfigSeq(
            std::initializer_list<ConfigNode> elements,
            const ConfigLocation &location = {});

        bool seqContains(int index) const;
        bool seqGet(int index, ConfigNode &node) const;
        ConfigNode seqAt(int index) const;
        int seqSize() const;
        std::vector<ConfigNode>::const_iterator seqBegin() const;
        std::vector<ConfigNode>::const_iterator seqEnd() const;

        bool operator==(const ConfigSeq &other) const;
        bool operator!=(const ConfigSeq &other) const;
        bool operator<(const ConfigSeq &other) const;

        static ConfigSeq append(const ConfigSeq &dst, const ConfigNode &src);
        static ConfigSeq extend(const ConfigSeq &dst, const ConfigSeq &src);

    private:
        explicit ConfigSeq(std::shared_ptr<ConfigNodeData> data);
        friend class ConfigNode;
    };

    class ConfigMap : public ConfigNode {
    public:
        ConfigMap();
        explicit ConfigMap(
            const absl::btree_map<std::string,ConfigNode> &map,
            const ConfigLocation &location = {});
        explicit ConfigMap(
            absl::btree_map<std::string,ConfigNode> &&map,
            const ConfigLocation &location = {});
        explicit ConfigMap(
            const absl::flat_hash_map<std::string,ConfigNode> &map,
            const ConfigLocation &location = {});
        explicit ConfigMap(
            std::initializer_list<std::pair<std::string,ConfigNode>> entries,
            const ConfigLocation &location = {});

        bool mapContains(std::string_view key) const;
        bool mapGet(std::string_view key, ConfigNode &node) const;
        ConfigNode mapAt(std::string_view key) const;
        int mapSize() const;
        absl::btree_map<std::string,ConfigNode>::const_iterator mapBegin() const;
        absl::btree_map<std::string,ConfigNode>::const_iterator mapEnd() const;

        bool operator==(const ConfigMap &other) const;
        bool operator!=(const ConfigMap &other) const;
        bool operator<(const ConfigMap &other) const;

        static ConfigMap extend(const ConfigMap &dst, const ConfigMap &src);
        static ConfigMap update(const ConfigMap &dst, const ConfigMap &src);

    private:
        explicit ConfigMap(std::shared_ptr<ConfigNodeData> data);
        friend class ConfigNode;
    };

    class ConfigFile {
    public:
        ConfigFile();
        ConfigFile(const std::filesystem::path &path, const ConfigNode &root);

        bool isValid() const;
        std::filesystem::path getPath() const;
        ConfigNode getRoot() const;

    private:
        std::filesystem::path m_path;
        ConfigNode m_root;
    };
}

#endif // TEMPO_CONFIG_CONFIG_NODE_H
