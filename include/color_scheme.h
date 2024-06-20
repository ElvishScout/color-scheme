#pragma once

#include "myrand.h"
#include "color_space.h"

#include <string>
#include <utility>
#include <vector>

std::vector<std::pair<RGB, double>> color_scheme(const std::string &, int, int);
std::vector<std::pair<RGB, double>> color_scheme(const std::string &, int, int, MyRand &);