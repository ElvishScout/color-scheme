#include "color_scheme.h"
#include "myrand.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <utility>
#include <vector>

#define HELP_MSG                                                                                                       \
  "Usage: %s [OPTION] FILE\n"                                                                                          \
  "Generate color scheme from image, based on k-means algorithm.\n"                                                    \
  "\n"                                                                                                                 \
  "Options:\n"                                                                                                         \
  "  -c, --color         enable colorful printing\n"                                                                   \
  "  -h, --help          display this help and exit\n"                                                                 \
  "  -n lines            max output lines\n"                                                                           \
  "  --sample samples    sample size\n"                                                                                \
  "  --cluster clusters  number of clusters for k-means algorithm\n"                                                   \
  "  --seed seed         RNG seed, negative for random seed\n"

void display(const std::vector<std::pair<RGB, double>> &scheme, bool colorful, int lines) {
  for (int i = 0; i < scheme.size() && (lines <= 0 || i < lines); i++) {
    unsigned char R = round(scheme[i].first.r);
    unsigned char G = round(scheme[i].first.g);
    unsigned char B = round(scheme[i].first.b);

    unsigned char k = (R > 127 && G > 127 && B > 127) ? 0 : 255;

    unsigned char r = k;
    unsigned char g = k;
    unsigned char b = k;

    if (colorful) {
      printf("\033[1m\033[38;2;%d;%d;%dm\033[48;2;%d;%d;%dm  #%02x%02x%02x | %5.2f%%  \033[0m\n", r, g, b, R, G, B, R,
             G, B, scheme[i].second * 100);
    } else {
      printf("#%02x%02x%02x | %5.2f%%\n", R, G, B, scheme[i].second * 100);
    }
  }
}

int main(int argc, const char **argv) {
  const char *filename = nullptr;
  int lines = 0;
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
      } else if (!strcmp(key, "n")) {
        lines = atoi(value);
        i++;
      } else if (!strcmp(key, "sample")) {
        samples = atoi(value);
        i++;
      } else if (!strcmp(key, "cluster")) {
        clusters = atoi(value);
        i++;
      } else if (!strcmp(key, "seed")) {
        seed = atoi(value);
        i++;
      } else {
        throw std::runtime_error(std::string() + "error: unknown parameter \"" + key + "\"");
      }
    } else if (value) {
      filename = value;
    }
  }

  if (help) {
    const char *basename;
    for (basename = argv[0] + strlen(argv[0]); basename != argv[0]; basename--) {
      if (*basename == '/' || *basename == '\\') {
        break;
      }
    }
    basename++;
    printf(HELP_MSG, basename);
    exit(0);
  }

  if (!filename) {
    throw std::runtime_error("error: no input file");
  }

  MyRand rng = seed < 0 ? MyRand() : MyRand(seed);
  auto scheme = color_scheme(filename, clusters, samples, rng);
  display(scheme, colorful, lines);

  return 0;
}
