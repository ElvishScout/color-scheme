#pragma once

#include "color_space.h"
#include "myrand.h"

#include <string>
#include <utility>
#include <vector>

std::vector<std::pair<RGB, double>> color_scheme(const std::string &, int, int);
std::vector<std::pair<RGB, double>> color_scheme(const std::string &, int, int, MyRand &);