#pragma once

#include "myrand.h"

#include <array>
#include <cstddef>
#include <functional>
#include <unordered_set>
#include <vector>

using Point = std::vector<double>;

struct Cluster {
  Point centroid;
  std::unordered_set<const Point *> points;
};

class KMeans {
private:
  int dim;
  const std::vector<Point> &points;
  double dist_sq(const Point &a, const Point &b);

public:
  KMeans(const std::vector<Point> &, int);
  std::vector<Cluster> cluster(int);
  std::vector<Cluster> cluster(int, MyRand &);
};
