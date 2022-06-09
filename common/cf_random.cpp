#include <random>

static std::mt19937 generator;

extern "C" uint32_t cf_random(void) {
    return generator();
}

extern "C" void cf_srandom(unsigned long seed) {
    generator.seed(seed);
}
