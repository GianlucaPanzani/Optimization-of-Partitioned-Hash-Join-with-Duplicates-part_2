#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <functional>

#include "lib/dataset.hpp"
#include "lib/timing.hpp"
#include "lib/checksum.hpp"
#include "lib/config.hpp"
#include "lib/results.hpp"
#include "lib/partition.hpp"
#ifdef ENABLE_AVX2
#include "lib/partition_avx2.hpp"
#endif


struct Args {
    uint64_t N = 10'000'000;
    uint32_t P = 128;
    std::string hash_name = "mask";
    std::string exec_type = "plain_novec";
    std::string output_csv = RESULTS_CSV_FILE; // default value defined in config.hpp
};

static Args parse_args(int argc, char** argv) {
    Args args;
    args.exec_type = std::string(argv[0]).substr(2);
    if (argc > 1) args.N = std::stoull(argv[1]);
    if (argc > 2) args.P = std::stoull(argv[2]);
    if (argc > 3) args.hash_name = std::string(argv[3]);
    if (argc > 4) args.output_csv = std::string(argv[4]);
    return args;
}

void check_args(const Args& args) {
    if (args.N == 0) {
        throw std::invalid_argument("N must be > 0");
    }
    if ((args.P & (args.P - 1)) != 0) {
        throw std::invalid_argument("P must be a power of two");
    }
    if (args.hash_name != "mask" && args.hash_name != "xorshift" && args.hash_name != "fmix32fold") {
        throw std::invalid_argument("Unsupported hash function: " + args.hash_name);
    }
    if (args.exec_type != "plain_novec" && args.exec_type != "plain_vec" && args.exec_type != "avx2") {
        throw std::invalid_argument("Unsupported execution type: " + args.exec_type);
    }
    if (args.output_csv.empty()) {
        throw std::invalid_argument("Output CSV path cannot be empty");
    }
}



// =========================================================
// ========================= MAIN ==========================
// =========================================================
int main(int argc, char** argv) {
    const Args args = parse_args(argc, argv);
    check_args(args);

    std::vector<std::uint32_t> R_partitioned(args.N), S_partitioned(args.N);
    double t0_global, t1_global, global_time, t0, t1, t, partition_time;
    const int n_digits = 8;

    // Loading datasets
    t0 = t0_global = get_time();
    Dataset R = load_dataset("dataset/R.bin");
    Dataset S = load_dataset("dataset/S.bin");
    t1 = get_time();
    t = get_diff(t0, t1, n_digits);
    if (VERBOSE) {
        std::cout << "LOADING_TIME[s]=" << t << "\n";
    }

    // Get the subset of the dataset if N is smaller than the dataset size
    if (args.N > R.size || args.N > S.size) {
        throw std::runtime_error("N cannot be larger than the dataset size");
    }
    R.keys.resize(args.N);
    S.keys.resize(args.N);
    R.size = S.size = args.N;
    if (VERBOSE) {
        std::cout << "[resize] Are used the first " << args.N << " keys of each dataset\n";
    }

    // Compute the partitions for each dataset
    if (args.exec_type != "avx2") {
        if (args.hash_name == "mask") {
            t0 = get_time();
            partition_with_mask_hashing(R.keys.data(), R_partitioned.data(), args.P, R.keys.size());
            t1 = get_time();
            partition_time = get_diff(t0, t1, n_digits);
            t0 = get_time();
            partition_with_mask_hashing(S.keys.data(), S_partitioned.data(), args.P, S.keys.size());
            t1 = get_time();
            partition_time += get_diff(t0, t1, n_digits);
        } else if (args.hash_name == "xorshift") {
            t0 = get_time();
            partition_with_xorshift_hashing(R.keys.data(), R_partitioned.data(), args.P, R.keys.size());
            t1 = get_time();
            partition_time = get_diff(t0, t1, n_digits);
            t0 = get_time();
            partition_with_xorshift_hashing(S.keys.data(), S_partitioned.data(), args.P, S.keys.size());
            t1 = get_time();
            partition_time += get_diff(t0, t1, n_digits);
        } else if (args.hash_name == "fmix32fold") {
            t0 = get_time();
            partition_with_fmix32fold_hashing(R.keys.data(), R_partitioned.data(), args.P, R.keys.size());
            t1 = get_time();
            partition_time = get_diff(t0, t1, n_digits);
            t0 = get_time();
            partition_with_fmix32fold_hashing(S.keys.data(), S_partitioned.data(), args.P, S.keys.size());
            t1 = get_time();
            partition_time += get_diff(t0, t1, n_digits);
        } else {
            throw std::invalid_argument("The hash function could only be: mask, xorshift, fmix32fold");
        }
    } else {
        if (args.hash_name != "mask") {
            std::cout << "--> The hash function " << args.hash_name << " is not supported for the execution type " << args.exec_type << "\n";
            return 0;
        }
        #ifdef ENABLE_AVX2
            t0 = get_time();
            partition_with_mask_hashing_avx2(R.keys.data(), R_partitioned.data(), args.P, R.keys.size());
            t1 = get_time();
            partition_time = get_diff(t0, t1, n_digits);
            t0 = get_time();
            partition_with_mask_hashing_avx2(S.keys.data(), S_partitioned.data(), args.P, S.keys.size());
            t1 = get_time();
            partition_time += get_diff(t0, t1, n_digits);
        #else
            throw std::invalid_argument("This binary was compiled without AVX2 support");
        #endif
        
    }
    std::cout << "PARTITION_TIME[s]=" << partition_time << " ";
    
    // Compute the throughput of computing partitions
    const double throughput = compute_throughput(
        static_cast<std::uint64_t>(R.size) + static_cast<std::uint64_t>(S.size),
        partition_time
    );
    std::cout << "THROUGHPUT[elems/s]=" << throughput << "\n";

    // Generate the checksum of the datasets
    t0 = get_time();
    uint64_t checksum_R = compute_checksum(R_partitioned);
    uint64_t checksum_S = compute_checksum(S_partitioned);
    std::string checksum = std::to_string(checksum_R) + std::to_string(checksum_S);
    t1 = get_time();
    t = get_diff(t0, t1, n_digits);
    if (VERBOSE) {
        std::cout << "CHECKSUM_TIME[s]=" << t << "\n";
    }

    // Save the partitioned datasets
    t0 = get_time();
    const std::string R_filename = "dataset/R_partitioned.bin";
    const std::string S_filename = "dataset/S_partitioned.bin";
    write_binary_dataset(std::filesystem::path(R_filename), R_partitioned);
    write_binary_dataset(std::filesystem::path(S_filename), S_partitioned);
    t1 = t1_global = get_time();
    t = get_diff(t0, t1, n_digits);
    global_time = get_diff(t0_global, t1_global, n_digits);
    if (VERBOSE) {
        std::cout << "SAVE_TIME[s]=" << t << "\n";
    }

    // Final outputs
    if (VERBOSE) {
        std::cout << "CHECKSUM=" << checksum << "\n";
        std::cout << "THROUGHPUT[elems/s]=" << throughput << "\n";
        std::cout << "GLOBAL_TIME[s]=" << global_time << "\n";
    }
    append_to_csv(
        args.output_csv,
        args.N,
        args.P,
        args.hash_name,
        args.exec_type,
        checksum,
        throughput,
        partition_time,
        global_time
    );
    return 0;
}
