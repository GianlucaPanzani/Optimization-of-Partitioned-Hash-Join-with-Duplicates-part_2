#include "results.hpp"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {
std::string escape_csv_field(const std::string& field) {
    if (field.find_first_of(",\"\n\r") == std::string::npos) {
        return field;
    }

    std::string escaped = "\"";
    for (const char current : field) {
        if (current == '"') {
            escaped += "\"\"";
        } else {
            escaped.push_back(current);
        }
    }
    escaped.push_back('"');
    return escaped;
}

std::string format_double(double value) {
    std::ostringstream stream;
    stream << std::setprecision(17) << value;
    return stream.str();
}

} // namespace

double compute_throughput(std::uint64_t total_elements, double partition_time_seconds) {
    if (partition_time_seconds <= 0.0) {
        throw std::invalid_argument("partition_time_seconds must be > 0");
    }
    return static_cast<double>(total_elements) / partition_time_seconds;
}


void append_to_csv(
    const std::string& csv_path,
    std::uint64_t N,
    std::uint32_t P,
    const std::string& checksum,
    double throughput,
    double partition_time,
    const std::size_t nr,
    const std::size_t ns,
    const std::uint64_t max_key,
    
) {
    const std::filesystem::path output_path(csv_path);
    const std::filesystem::path parent = output_path.parent_path();
    if (!parent.empty()) {
        std::filesystem::create_directories(parent);
    }

    const bool needs_header = !std::filesystem::exists(output_path) || std::filesystem::file_size(output_path) == 0;

    std::ofstream out(output_path, std::ios::app);
    if (!out) {
        throw std::runtime_error("Cannot open CSV file for writing: " + output_path.string());
    }

    if (needs_header) {
        out << "N,P,HASH,EXEC_TYPE,CHECKSUM,THROUGHPUT,PARTITION_TIME,GLOBAL_TIME\n";
    }

    out << N << ','
        << P << ','
        << escape_csv_field(checksum) << ','
        << format_double(throughput) << ','
        << format_double(partition_time) << '\n';

    if (!out) {
        throw std::runtime_error("Error while writing CSV file: " + output_path.string());
    }
}
