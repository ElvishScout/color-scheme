#pragma once

#include "myrand.h"

#include <string>
#include <utility>
#include <vector>

struct RGB {
  double r;
  double g;
  double b;
};

std::vector<std::pair<RGB, double>> color_scheme(const std::string &, int, int);
std::vector<std::pair<RGB, double>> color_scheme(const std::string &, int, int, MyRand &);