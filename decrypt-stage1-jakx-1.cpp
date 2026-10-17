#include <iostream>
#include <fstream>
#include <vector>
#include "minilzo.h"

enum ELF_TYPE: unsigned int{
    US,
    PAL
};

constexpr const auto US_ELF_FILENAME = "SCUS_974.29";
constexpr const auto PAL_ELF_FILENAME = "SCES_532.86";
constexpr const auto DEFAULT_ELF = US_ELF_FILENAME;
int main(int argc, char **argv) {

    std::string elfName;
    ELF_TYPE elfType;
    if(argc < 2){
        elfName = DEFAULT_ELF;
        std::cout << "No ELF file specified, defaulting to " << DEFAULT_ELF << std::endl;
    }
    else{
        elfName = argv[1];
    }

    if(elfName == US_ELF_FILENAME){
        elfType = US;
    }
    else if(elfName == PAL_ELF_FILENAME){
        elfType = PAL;
    }
    else{
        elfType = US;
        std::cerr << "ELF file not named " << US_ELF_FILENAME << " or " << PAL_ELF_FILENAME << "." << std::endl;
        std::cerr << "Assuming it is SCUS_974.29" << std::endl;
    }

    // Initialize LZO
    if (lzo_init() != LZO_E_OK) {
        std::cerr << "LZO initialization failed" << std::endl;
        return 1;
    }

    // Read the entire ELF file
    std::ifstream input{ elfName, std::ios::binary };
    if (!input) {
        std::cerr << "Failed to open input file" << std::endl;
        return 1;
    }

    std::vector<unsigned char> elf_data((std::istreambuf_iterator<char>(input)),
                                        std::istreambuf_iterator<char>());
    input.close();

    // Extract the compressed portion
    size_t compressed_offset;
    size_t compressed_size;
    size_t expected_decompressed_size;
    if(elfType == US){
        compressed_offset = 0x166ab4 - 0x100000;
        compressed_size = 0x88f9f;
        expected_decompressed_size = 0x170CA4;
    }
    else{ //elfType == PAL
        compressed_offset = 0x16bbc4 - 0x100000;
        compressed_size = 0x89002;
        expected_decompressed_size = 0x170DA4;
    }
    
    if (elf_data.size() < compressed_offset + compressed_size) {
        std::cerr << "ELF file is smaller than expected" << std::endl;
        std::cout
            << "elf_data: " << elf_data.size() << std::endl
            << "  compressed_offset: " << compressed_offset << std::endl
            << "  compressed_size: " << compressed_size << std::endl
            << "  compressed_offset + compressed_size: " << compressed_offset + compressed_size << std::endl
            ;
        return 1;
    }

    std::vector<unsigned char> compressed_data(elf_data.begin() + compressed_offset,
                                               elf_data.begin() + compressed_offset + compressed_size);

    // Prepare buffer for decompressed data
    std::vector<unsigned char> decompressed_data(expected_decompressed_size);
    lzo_uint decompressed_size = 0;

    // Decompress
    int r = lzo1x_decompress(compressed_data.data(), compressed_data.size(),
                             decompressed_data.data(), &decompressed_size, nullptr);

    if (r != LZO_E_OK) {
        std::cerr << "Decompression failed" << std::endl;
        return 1;
    }
    std::cout
        << "expected elf size: " << expected_decompressed_size << std::endl
        << "actual elf size: " << decompressed_size << std::endl
        ;

    // Resize the decompressed data to its actual size
    decompressed_data.resize(decompressed_size);

    // Write the decompressed ELF
    std::string output_elf = "decrypted-" + elfName;
    std::ofstream output(output_elf, std::ios::binary);
    if (!output) {
        std::cerr << "Failed to open output file " << output_elf << std::endl;
        return 1;
    }

    // Write the decompressed part
    output.write(reinterpret_cast<const char*>(decompressed_data.data()), decompressed_size);

    output.close();

    std::cout << "Decompression to " << output_elf << " completed successfully" << std::endl;
    return 0;
}
