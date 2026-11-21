#pragma once
#include <filesystem>
#include <iosfwd>

int runApplication(std::istream& input, std::ostream& output, std::ostream& errors,
                   const std::filesystem::path& csvPath);
