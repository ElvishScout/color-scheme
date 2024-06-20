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
    dist2 += pow(a[i] - b[i], 2);
  }
  return sqrt(dist2);
}
