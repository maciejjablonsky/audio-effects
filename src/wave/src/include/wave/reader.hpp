#pragma once
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <span>
#include <wave/header.hpp>


namespace splt::wave
{
class reader
{
  public:
    reader(const std::filesystem::path& path);
    void read(const std::span<std::byte> buffer);

    const header& get_header() const;
  private:
    header _header = {};
    std::ifstream _file;
};
} // namespace splt::wave
