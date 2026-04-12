#ifndef RESULTS_HPP
#define RESULTS_HPP

#include <cstdint>
#include <string>

double compute_throughput(std::uint64_t total_elements, double partition_time_seconds);

void append_to_csv(
    const std::string& csv_path,
    std::uint64_t N,
    std::uint32_t P,
    const std::string& hash_name,
    const std::string& exec_type,
    const std::string& checksum,
    double throughput,
    double partition_time,
    double global_time
);

#endif
