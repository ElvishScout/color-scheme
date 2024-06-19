#include "myrand.h"
#include <random>

MyRand::MyRand() : generator(std::random_device()()) {}

MyRand::MyRand(unsigned int seed) : generator(seed) {}

int MyRand::randint(int min, int max) { return std::uniform_int_distribution<>(min, max - 1)(generator); }

double MyRand::uniform(double min, double max) { return std::uniform_real_distribution<>(min, max)(generator); }
