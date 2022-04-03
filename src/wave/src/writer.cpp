#include <fmt/format.h>
#include <wave/writer.hpp>

namespace splt::wave
{
void writer::_write_header()
{
    auto pos = std::ftell(_file);
    std::fseek(_file, 0, SEEK_SET);
    std::fwrite(std::addressof(_header), sizeof(_header), 1, _file);
    std::fseek(_file, pos, SEEK_SET);
}
writer::writer(const wave::header& header,
               const std::filesystem::path& file_path)
    : _header{[&] {
          wave::validate_header(header);
          return header;
      }()},
      _file{[&] {
          auto file = std::fopen(file_path.string().c_str(), "wb+");
          if (!file)
          {
              throw std::runtime_error(
                  fmt::format("Could not open file at {}", file_path.string()));
          }
          return file;
      }()}
{
    std::fseek(_file, sizeof(_header), SEEK_SET);
}

writer::~writer()
{
    _write_header();
    std::fclose(_file);
}

} // namespace splt::wave