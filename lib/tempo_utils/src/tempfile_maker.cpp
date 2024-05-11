
#include <tempo_utils/file_result.h>
#include <tempo_utils/file_utilities.h>
#include <tempo_utils/file_writer.h>
#include <tempo_utils/tempfile_maker.h>

#define POSIX_NUM_GENERATE_TRIES      3

static tempo_utils::Status
posix_create_tempfile(
    const std::filesystem::path &base,
    std::string_view tempname,
    const char *data,
    tu_uint32 size,
    std::filesystem::path &tempfile)
{
    if (!tempo_utils::is_valid_tempname(tempname))
        return tempo_utils::FileStatus::forCondition(
            tempo_utils::FileCondition::kInvalidTemplate,
            "invalid template name '{}'", tempname);

    for (int attempt = 0; attempt < POSIX_NUM_GENERATE_TRIES; attempt++) {
        auto filename = tempo_utils::generate_name(tempname);
        if (filename.empty())
            return tempo_utils::FileStatus::forCondition(
                tempo_utils::FileCondition::kSystemInvariant,
                "failed to generate filename from template '{}'", tempname);

        tempfile = base / filename;
        tempo_utils::FileWriter tempfileMaker(tempfile.string(), std::string_view(data, size),
            tempo_utils::FileWriterMode::CREATE_ONLY,
            std::filesystem::perms::owner_read | std::filesystem::perms::owner_write);

        auto status = tempfileMaker.getStatus();
        if (status.matchesCondition(tempo_utils::FileCondition::kFileExists))
            break;
        return status;
    }

    // exhausted retries
    return tempo_utils::FileStatus::forCondition(
        tempo_utils::FileCondition::kSystemInvariant,
        "failed to generate unique directory");
}

tempo_utils::TempfileMaker::TempfileMaker(
    std::string_view tempname,
    std::span<const tu_uint8> bytes)
    : TempfileMaker(std::filesystem::temp_directory_path(), tempname, bytes)
{
}

tempo_utils::TempfileMaker::TempfileMaker(
    std::string_view tempname,
    std::string_view str)
    : TempfileMaker(std::filesystem::temp_directory_path(), tempname, str)
{
}

tempo_utils::TempfileMaker::TempfileMaker(
    const std::filesystem::path &base,
    std::string_view tempname,
    std::span<const tu_uint8> bytes)
{
    m_status = posix_create_tempfile(base, tempname, (const char *) bytes.data(), bytes.size(), m_tempfile);
}

tempo_utils::TempfileMaker::TempfileMaker(
    const std::filesystem::path &base,
    std::string_view tempname,
    std::string_view str)
{
    m_status = posix_create_tempfile(base, tempname, str.data(), str.size(), m_tempfile);
}

bool
tempo_utils::TempfileMaker::isValid() const
{
    return m_status.isOk();
}

tempo_utils::Status
tempo_utils::TempfileMaker::getStatus() const
{
    return m_status;
}

std::filesystem::path
tempo_utils::TempfileMaker::getTempfile() const
{
    return m_tempfile;
}
