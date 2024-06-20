#include "color_scheme.h"
#include "color_space.h"
#include "myrand.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <utility>
#include <vector>

void display(const std::vector<std::pair<RGB, double>> &scheme, bool colorful) {
  for (auto &color : scheme) {
    unsigned char R = round(color.first.r);
    unsigned char G = round(color.first.g);
    unsigned char B = round(color.first.b);

    unsigned char k = (R > 127 && G > 127 && B > 127) ? 0 : 255;

    unsigned char r = k;
    unsigned char g = k;
    unsigned char b = k;

    if (colorful) {
      printf("\033[1m\033[38;2;%d;%d;%dm\033[48;2;%d;%d;%dm  #%02x%02x%02x | %5.2f%%  \033[0m\n", r, g, b, R, G, B, R,
             G, B, color.second * 100);
    } else {
      printf("#%02x%02x%02x | %5.2f%%\n", R, G, B, color.second * 100);
    }
  }
}

int main(int argc, const char **argv) {
  const char *help_msg = "Usage: color-scheme [OPTION] FILE\n"
                         "Generate color scheme from image, based on k-means algorithm.\n"
                         "\n"
                         "Options:\n"
                         "  -h, --help    display this help and exit\n"
                         "  -c, --color   enable colorful printing\n"
                         "  --sample      sample size\n"
                         "  --cluster     number of clusters for k-means algorithm\n"
                         "  --seed        RNG seed, negative for random seed\n";

  const char *filename = nullptr;
  int samples = 1000;
  int clusters = 8;
  int seed = -1;
  bool help = false;
  bool colorful = false;

  for (int i = 1; i < argc; i++) {
    const char *arg = argv[i];
    const char *next_arg = argv[i + 1];
    const char *key = nullptr;
    const char *value = nullptr;

    if (arg[0] == '-') {
      if (arg[1] == '-') {
        if (arg[2]) {
          key = arg + 2;
        } else {
          i++;
        }
      } else {
        key = arg + 1;
      }
      value = next_arg;
    } else {
      value = arg;
    }

    if (key) {
      if (!strcmp(key, "h") || !strcmp(key, "help")) {
        help = true;
      } else if (!strcmp(key, "c") || !strcmp(key, "color")) {
        colorful = true;
      } else if (!strcmp(key, "sample")) {
        samples = atoi(value);
        i++;
      } else if (!strcmp(key, "cluster")) {
        clusters = atoi(value);
        i++;
      } else if (!strcmp(key, "seed")) {
        seed = atoi(value);
        i++;
      }
    } else if (value) {
      filename = value;
    }
  }

  if (help) {
    printf("%s", help_msg);
    exit(0);
  }

  if (!filename) {
    throw std::runtime_error("error: no input file");
  }
  if (clusters < 1) {
    throw std::runtime_error("error: number of clusters must be positive");
  }
  if (samples < 1) {
    throw std::runtime_error("error: number of samples must be positive");
  }
  if (clusters > samples) {
    throw std::runtime_error("error: more clusters than samples");
  }

  MyRand rng = seed < 0 ? MyRand() : MyRand(seed);
  auto scheme = color_scheme(filename, clusters, samples, rng);
  display(scheme, colorful);

  return 0;
}
