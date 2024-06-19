#define STB_IMAGE_IMPLEMENTATION

#include "color_scheme.h"
#include "kmeans.h"
#include "myrand.h"
#include "stb_image.h"

#include <algorithm>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

std::vector<std::pair<RGB, double>> color_scheme(const std::string &filename, int schemes, int samples) {
  MyRand rng;
  return color_scheme(filename, schemes, samples, rng);
}

std::vector<std::pair<RGB, double>> color_scheme(const std::string &filename, int schemes, int samples, MyRand &rng) {
  if (schemes > samples) {
    schemes = samples;
  }

  std::vector<Point> points;

  {
    int x, y, n;
    unsigned char *data = stbi_load(filename.c_str(), &x, &y, &n, STBI_rgb);
    if (!data) {
      exit(1);
    }
    for (int i = 0; i < samples; i++) {
      int index = rng.randint(0, x * y) * 3;
      points.push_back({(double)data[index], (double)data[index + 1], (double)data[index + 2]});
    }
    stbi_image_free(data);
  }

  KMeans km(points, 3);
  std::vector<Cluster> clusters = km.cluster(schemes, rng);
  std::sort(clusters.begin(), clusters.end(), [](Cluster &a, Cluster &b) { return a.points.size() > b.points.size(); });

  std::vector<std::pair<RGB, double>> results;
  for (Cluster &cluster : clusters) {
    if (cluster.points.empty()) {
      break;
    }
    results.push_back(
        {{cluster.centroid[0], cluster.centroid[1], cluster.centroid[2]}, (double)cluster.points.size() / samples});
  };

  return results;
}
