#ifndef DATASET_HPP
#define DATASET_HPP

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

struct Dataset {
    uint64_t size;
    std::vector<uint64_t> keys;
};


Dataset load_dataset(const std::string& path);


template <typename T>
void write_binary_dataset(const std::filesystem::path& path, const std::vector<T>& data) {
    // Output file stream
    std::ofstream out(path, std::ios::binary);

    // Error opening the file
    if (!out) {
        throw std::runtime_error("Cannot open output file: " + path.string());
    }
    
    // Write on the output file
    const uint64_t N = static_cast<uint64_t>(data.size()); // N = dim of the dataset
    out.write(reinterpret_cast<const char*>(&N), sizeof(N));
    out.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(N * sizeof(T)));

    // Error during writing
    if (!out) {
        throw std::runtime_error("Error while writing file: " + path.string());
    }
}

#endif