#pragma once

#include <random>

class MyRand {
private:
  std::default_random_engine generator;

public:
  MyRand();
  MyRand(unsigned int seed);

  int randint(int, int);
  double uniform(double, double);
};
