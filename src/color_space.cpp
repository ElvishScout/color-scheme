#include "color_space.h"

#include <cmath>

const double PI = 3.1415926535897932354626;

double degrees(double n) { return n * (180 / PI); }

double radians(double n) { return n * (PI / 180); }

double hpF(double x, double y) {
  if (x == 0 && y == 0) {
    return 0;
  } else {
    double tmphp = degrees(atan2(x, y));
    if (tmphp >= 0) {
      return tmphp;
    } else {
      return tmphp + 360;
    }
  }
}

double dhpF(double C1, double C2, double h1p, double h2p) {
  if (C1 * C2 == 0) {
    return 0;
  } else if (fabs(h2p - h1p) <= 180) {
    return h2p - h1p;
  } else if (h2p - h1p > 180) {
    return h2p - h1p - 360;
  } else if (h2p - h1p < -180) {
    return h2p - h1p + 360;
  }
  return 0;
}

double aHpF(double C1, double C2, double h1p, double h2p) {
  if (C1 * C2 == 0) {
    return h1p + h2p;
  } else if (fabs(h1p - h2p) <= 180) {
    return (h1p + h2p) / 2.0;
  } else if (fabs(h1p - h2p) > 180 && h1p + h2p < 360) {
    return (h1p + h2p + 360) / 2.0;
  } else if (fabs(h1p - h2p) > 180 && h1p + h2p >= 360) {
    return (h1p + h2p - 360) / 2.0;
  }
  return 0;
}

XYZ rgb_to_xyz(const RGB &rgb) {
  double r = rgb.r / 255.0;
  double g = rgb.g / 255.0;
  double b = rgb.b / 255.0;

  r = r > 0.04045 ? pow((r + 0.055) / 1.055, 2.4) : r / 12.92;
  g = g > 0.04045 ? pow((g + 0.055) / 1.055, 2.4) : g / 12.92;
  b = b > 0.04045 ? pow((b + 0.055) / 1.055, 2.4) : b / 12.92;

  double x = r * 0.4124 + g * 0.3576 + b * 0.1805;
  double y = r * 0.2126 + g * 0.7152 + b * 0.0722;
  double z = r * 0.0193 + g * 0.1192 + b * 0.9505;

  return {x * 100, y * 100, z * 100};
}

RGB xyz_to_rgb(const XYZ &xyz) {
  double x = xyz.x / 100.0;
  double y = xyz.y / 100.0;
  double z = xyz.z / 100.0;

  double r = x * 3.2406 + y * -1.5372 + z * -0.4986;
  double g = x * -0.9689 + y * 1.8758 + z * 0.0415;
  double b = x * 0.0557 + y * -0.204 + z * 1.057;

  r = r > 0.0031308 ? 1.055 * pow(r, 1.0 / 2.4) - 0.055 : r * 12.92;
  g = g > 0.0031308 ? 1.055 * pow(g, 1.0 / 2.4) - 0.055 : g * 12.92;
  b = b > 0.0031308 ? 1.055 * pow(b, 1.0 / 2.4) - 0.055 : b * 12.92;

  r = r < 0 ? 0 : (r > 1 ? 1 : r);
  g = g < 0 ? 0 : (g > 1 ? 1 : g);
  b = b < 0 ? 0 : (b > 1 ? 1 : b);

  return {r * 255, g * 255, b * 255};
}

LAB xyz_to_lab(const XYZ &xyz) {
  double x = xyz.x / 95.047;
  double y = xyz.y / 100.0;
  double z = xyz.z / 108.883;

  x = x > 0.008856 ? pow(x, 1.0 / 3) : 7.787 * x + 16.0 / 116;
  y = y > 0.008856 ? pow(y, 1.0 / 3) : 7.787 * y + 16.0 / 116;
  z = z > 0.008856 ? pow(z, 1.0 / 3) : 7.787 * z + 16.0 / 116;

  double l = 116 * y - 16;
  double a = 500 * (x - y);
  double b = 200 * (y - z);

  return {l, a, b};
}

XYZ lab_to_xyz(const LAB &lab) {
  double y = (lab.l + 16) / 116;
  double x = lab.a / 500 + y;
  double z = y - lab.b / 200;

  double y2 = pow(y, 3);
  double x2 = pow(x, 3);
  double z2 = pow(z, 3);

  y = y2 > 0.008856 ? y2 : (y - 16.0 / 116) / 7.787;
  x = x2 > 0.008856 ? x2 : (x - 16.0 / 116) / 7.787;
  z = z2 > 0.008856 ? z2 : (z - 16.0 / 116) / 7.787;

  x *= 95.047;
  y *= 100;
  z *= 108.883;

  return {x, y, z};
}

LAB rgb_to_lab(const RGB &rgb) { return xyz_to_lab(rgb_to_xyz(rgb)); }

RGB lab_to_rgb(const LAB &lab) { return xyz_to_rgb(lab_to_xyz(lab)); }

double color_diff(const LAB &lab1, const LAB &lab2) {
  double L1 = lab1.l;
  double a1 = lab1.a;
  double b1 = lab1.b;

  double L2 = lab2.l;
  double a2 = lab2.a;
  double b2 = lab2.b;

  // Weight factors
  double kL = 1;
  double kC = 1;
  double kH = 1;

  /**
   * Step 1: Calculate C1p, C2p, h1p, h2p
   */
  double C1 = sqrt(pow(a1, 2) + pow(b1, 2)); // (2)
  double C2 = sqrt(pow(a2, 2) + pow(b2, 2)); // (2)

  double aC1C2 = (C1 + C2) / 2.0; // (3)

  double G = 0.5 * (1 - sqrt(pow(aC1C2, 7.0) / (pow(aC1C2, 7.0) + pow(25.0, 7.0)))); // (4)

  double a1p = (1.0 + G) * a1; // (5)
  double a2p = (1.0 + G) * a2; // (5)

  double C1p = sqrt(pow(a1p, 2) + pow(b1, 2)); // (6)
  double C2p = sqrt(pow(a2p, 2) + pow(b2, 2)); // (6)

  double h1p = hpF(b1, a1p); // (7)
  double h2p = hpF(b2, a2p); // (7)

  /**
   * Step 2: Calculate dLp, dCp, dHp
   */
  double dLp = L2 - L1;   // (8)
  double dCp = C2p - C1p; // (9)

  double dhp = dhpF(C1, C2, h1p, h2p);                        // (10)
  double dHp = 2 * sqrt(C1p * C2p) * sin(radians(dhp) / 2.0); // (11)

  /**
   * Step 3: Calculate CIEDE2000 Color-Difference
   */
  double aL = (L1 + L2) / 2.0;    // (12)
  double aCp = (C1p + C2p) / 2.0; // (13)

  double aHp = aHpF(C1, C2, h1p, h2p); // (14)
  double T = 1 - 0.17 * cos(radians(aHp - 30)) + 0.24 * cos(radians(2 * aHp)) + 0.32 * cos(radians(3 * aHp + 6)) -
             0.2 * cos(radians(4 * aHp - 63));                              // (15)
  double dRo = 30 * exp(-pow((aHp - 275) / 25, 2));                         // (16)
  double RC = sqrt(pow(aCp, 7.0) / (pow(aCp, 7.0) + pow(25.0, 7.0)));       // (17)
  double SL = 1 + (0.015 * pow(aL - 50, 2)) / sqrt(20 + pow(aL - 50, 2.0)); // (18)
  double SC = 1 + 0.045 * aCp;                                              // (19)
  double SH = 1 + 0.015 * aCp * T;                                          // (20)
  double RT = -2 * RC * sin(radians(2 * dRo));                              // (21)
  double dE = sqrt(pow(dLp / (SL * kL), 2) + pow(dCp / (SC * kC), 2) + pow(dHp / (SH * kH), 2) +
                   RT * (dCp / (SC * kC)) * (dHp / (SH * kH))); // (22)
  return dE;
}
