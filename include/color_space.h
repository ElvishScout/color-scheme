#pragma once

struct RGB {
  double r;
  double g;
  double b;
};

struct LAB {
  double l;
  double a;
  double b;
};

struct XYZ {
  double x;
  double y;
  double z;
};

LAB rgb_to_lab(const RGB &);
RGB lab_to_rgb(const LAB &);

double color_diff(const LAB &, const LAB &);
