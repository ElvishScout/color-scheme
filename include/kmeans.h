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
  double radius;
};

class KMeans {
private:
  int dim;
  const std::vector<Point> &points;
  double default_diff(const Point &, const Point &);

public:
  KMeans(const std::vector<Point> &, int);
  std::vector<Cluster> cluster(int);
  std::vector<Cluster> cluster(int, const std::function<double(const Point &, const Point &)> &);
  std::vector<Cluster> cluster(int, MyRand &);
  std::vector<Cluster> cluster(int, const std::function<double(const Point &, const Point &)> &, MyRand &);
};
