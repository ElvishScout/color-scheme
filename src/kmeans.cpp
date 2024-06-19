#include "kmeans.h"
#include "myrand.h"

#include <cmath>
#include <functional>
#include <limits>
#include <unordered_set>
#include <utility>
#include <vector>

KMeans::KMeans(const std::vector<Point> &points, int dim) : points(points) { this->dim = dim; }

std::vector<Cluster> KMeans::cluster(int k) {
  MyRand rng;
  return cluster(k, [this](auto a, auto b) { return default_diff(a, b); }, rng);
}

std::vector<Cluster> KMeans::cluster(int k, const std::function<double(const Point &a, const Point &b)> &diff) {
  MyRand rng;
  return cluster(k, diff, rng);
}

std::vector<Cluster> KMeans::cluster(int k, MyRand &rng) {
  return cluster(k, [this](auto a, auto b) { return default_diff(a, b); }, rng);
}

std::vector<Cluster> KMeans::cluster(int k, const std::function<double(const Point &, const Point &)> &diff,
                                     MyRand &rng) {
  std::vector<std::pair<double, double>> limits;
  limits.resize(dim);
  std::fill(
      limits.begin(), limits.end(),
      std::pair<double, double>{std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()});

  for (int i = 0; i < dim; i++) {
    for (auto &point : this->points) {
      if (point[i] < limits[i].first) {
        limits[i].first = point[i];
      }
      if (point[i] > limits[i].second) {
        limits[i].second = point[i];
      }
    }
  }

  std::vector<Cluster> clusters;

  {
    std::unordered_set<const Point *> centroids;
    std::vector<double> dists;
    dists.resize(points.size());
    std::fill(dists.begin(), dists.end(), 0);

    const Point *last_centroid = &points[rng.randint(0, points.size())];
    centroids.insert(last_centroid);

    for (int i = 1; i < k; i++) {
      double maxDist = 0;
      int maxJ = -1;

      for (int j = 0; j < points.size(); j++) {
        dists[j] += diff(*last_centroid, points[j]);
        if (dists[j] > maxDist && centroids.find(&points[j]) == centroids.end()) {
          maxDist = dists[j];
          maxJ = j;
        }
      }

      if (maxJ != -1) {
        last_centroid = &points[maxJ];
        centroids.insert(last_centroid);
      }
    }

    for (const Point *centroid : centroids) {
      clusters.push_back(Cluster{*centroid, {}, 0});
    }
  }

  bool flag = true;
  std::vector<std::unordered_set<const Point *>> old_cluster_points;
  old_cluster_points.resize(k);

  while (flag) {
    for (int i = 0; i < k; i++) {
      old_cluster_points[i] = std::move(clusters[i].points);
      clusters[i].radius = 0;
    }

    flag = false;
    for (int j = 0; j < points.size(); j++) {
      double min_dist = std::numeric_limits<double>::infinity();
      int min_i = -1;
      for (int i = 0; i < k; i++) {
        double dist = diff(clusters[i].centroid, points[j]);
        if (dist < min_dist) {
          min_dist = dist;
          min_i = i;
        }
      }
      if (min_i != -1) {
        if (min_dist > clusters[min_i].radius) {
          clusters[min_i].radius = min_dist;
        }
        clusters[min_i].points.insert(&points[j]);

        if (!flag && old_cluster_points[min_i].find(&points[j]) == old_cluster_points[min_i].end()) {
          flag = true;
        }
      }
    }

    for (int i = 0; i < k; i++) {
      for (int d = 0; d < dim; d++) {
        if (clusters[i].points.empty()) {
          clusters[i].centroid[d] = rng.uniform(limits[d].first, limits[d].second);
        } else {
          double sum = 0;
          for (const Point *point : clusters[i].points) {
            sum += (*point)[d];
          }
          clusters[i].centroid[d] = sum / clusters[i].points.size();
        }
      }
    }
  }

  return clusters;
}

double KMeans::default_diff(const Point &a, const Point &b) {
  double dist2 = 0;
  for (int i = 0; i < dim; i++) {
    dist2 += std::pow(a[i] - b[i], 2);
  }
  return std::sqrt(dist2);
}

// template <size_t N>
// class KMeans {
// private:
//   using Point = array<double, N>;

//   vector<Point> points;

//   static double uniform(double min, double max) {
//     static random_device device;
//     static default_random_engine generator(device());
//     return uniform_real_distribution<double>(min, max)(generator);
//   }

// public:
//   KMeans(const vector<Point> &points) { this->points = points; }

//   vector<Point> cluster(int k, const function<double(const Point &, const Point &)> &dist) {
//     vector<Point> centroids;
//     for (int i = 0; i < k; i++) {
//       centroids.push_back(Point());
//     }

//     vector<pair<double, double>> limits;
//     for (int i = 0; i < N; i++) {
//       pair<double, double> limit = {numeric_limits<double>::max(),
//                                     numeric_limits<double>::min()};
//       for (auto &point : points) {
//         limit.first = min(limit.first, point[i]);
//         limit.second = max(limit.second, point[i]);
//       }
//       limits.push_back(limit);
//       for (auto &centroid : centroids) {
//         centroid[i] = uniform(limit.first, limit.second);
//       }
//     }

//     return centroids;
//   }
// };

// int main() {
//   KMeans<3> km = KMeans<3>({{1, 2, 3}, {4, 5, 6}});
//   auto centroids = km.cluster(2, nullptr);
//   for (auto &point : centroids) {
//     for (auto &x : point) {
//       cout << x << " ";
//     }
//     cout << endl;
//   }
//   return 0;
// }
