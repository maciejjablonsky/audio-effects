#include <fmt/format.h>
#include <wave/reader.hpp>
#include <vector>

namespace splt::wave
{
reader::reader(const std::filesystem::path& path) : _file(path)
{
    if (!_file.is_open())
    {
        throw std::runtime_error(
            fmt::format("Could not open file {}", path.string()));
    }
    _file.read(reinterpret_cast<char*>(std::addressof(_header)), sizeof(_header));
}

void reader::read(const std::span<std::byte> buffer)
{
    if (!_file.read(reinterpret_cast<char*>(buffer.data()), buffer.size()))
    {
        throw std::runtime_error("Could not read from file");
    }
}
} // namespace splt::wave
