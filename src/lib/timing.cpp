#include <chrono>
#include <cmath>

#include "timing.hpp"


double get_time() {
    return std::chrono::steady_clock::now();
}


double get_diff(double t0, double t1) {
    return std::chrono::duration<double>(t1 - t0).count();
}