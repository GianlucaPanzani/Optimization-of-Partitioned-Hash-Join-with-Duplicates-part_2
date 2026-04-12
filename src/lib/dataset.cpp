#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>

#include "dataset.hpp"



Dataset load_dataset(const std::string& path) {
    // Input file stream
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        throw std::runtime_error("Cannot open dataset file: " + path);
    }

    // Read the size of the dataset (first element stored in the file)
    Dataset ds{};
    in.read(reinterpret_cast<char*>(&ds.size), sizeof(ds.size));
    if (!in) {
        throw std::runtime_error("Failed to read dataset size from: " + path);
    }

    // Read the data/keys
    ds.keys.resize(ds.size); // set the size of the vector as the size read before (size = number of uint64_t)
    in.read(reinterpret_cast<char*>(ds.keys.data()), static_cast<std::streamsize>(ds.size * sizeof(uint64_t)));
    if (!in) {
        throw std::runtime_error("Failed to read dataset payload from: " + path);
    }

    return ds;
}