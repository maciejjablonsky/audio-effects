#include <fmt/format.h>
#include <wave/reader.hpp>

namespace splt::wave
{
wave::header reader::_read_header()
{
    std::fseek(_file, 0, SEEK_SET);
    wave::header h{};
    std::fread(std::addressof(h), sizeof(h), 1, _file);
    wave::validate_header(h);
    return h;
}

reader::reader(const std::filesystem::path& file_path)
    : _file{[&] {
          auto file = std::fopen(file_path.string().c_str(), "rb");
          if (!file)
          {
              throw std::runtime_error(
                  fmt::format("Failed to open file at {}", file_path.string()));
          }
          return file;
      }()},
      _header{_read_header()}
{
}

reader::~reader()
{
    std::fclose(_file);
}

void reader::reset()
{
    std::fseek(_file, sizeof(_header), SEEK_SET);
}

bool reader::eof() const
{
    return std::feof(_file);
}

const wave::header& reader::header() const
{
    return _header;
}

size_t reader::bytes_left() const
{
    const auto file_size = _header.file_size + wave::file_size_skipped;
    const auto pos       = std::ftell(_file);
    return file_size - pos;
}

size_t reader::frames_left() const
{
    return bytes_left() / (_header.channels * (_header.bytes_per_sample));
}
} // namespace splt::wave