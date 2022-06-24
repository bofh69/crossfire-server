#include <random>

static std::mt19937 generator;

uint32_t cf_random(void) {
    return generator() % RAND_MAX;
}

void cf_srandom(unsigned long seed) {
    generator.seed(seed);
}
