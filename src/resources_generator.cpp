#include <iostream>
#include <fstream>
#include <filesystem>
#include <fmt/format.h>
#include <array>

int main(int argc, char * argv[])
{
    namespace fs = std::filesystem;

    if (argc < 4) {
        return 0;
    }

    std::ofstream out(argv[1]);
    out << "#include <stdint.h>\n\n";

    std::ofstream header(argv[2]);
    header << "#pragma once\n\n";
    header << "#include <stdint.h>\n\n";
    header << "#ifdef __cplusplus\n";
    header << "extern \"C\"{\n";
    header << "#endif\n\n";

    for (const auto & entry : fs::directory_iterator(argv[3])) {
        std::ifstream input(entry.path(), std::ios::binary);
        std::vector<char> bytes(
            (std::istreambuf_iterator<char>(input)),
            (std::istreambuf_iterator<char>()));

        auto folder = entry.path().parent_path().stem().string();
        auto name = entry.path().stem().string();
        out << "const size_t " << folder << "_" << name << "_size = " << bytes.size() << "UL;\n";
        out << "const uint8_t " << folder << "_" << name << "[] = {\n";
        out << fmt::format("{:d}", fmt::join(bytes, ", "));
        out << "};\n\n";

        header << "extern const size_t " << folder << "_" << name << "_size;\n";
        header << "extern const uint8_t " << folder << "_" << name << "[];\n\n";
    }

    header << "#ifdef __cplusplus\n";
    header << "}\n";
    header << "#endif\n\n";

    return 0;
}
