export type RGB = [number, number, number];
export type XYZ = [number, number, number];
export type LAB = [number, number, number];

function degrees(n: number) {
  return n * (180 / Math.PI);
}

function radians(n: number) {
  return n * (Math.PI / 180);
}

function hpF(x: number, y: number) {
  if (x === 0 && y === 0) return 0;
  else {
    const tmphp = degrees(Math.atan2(x, y));
    if (tmphp >= 0) return tmphp;
    else return tmphp + 360;
  }
}

function dhpF(C1: number, C2: number, h1p: number, h2p: number) {
  if (C1 * C2 === 0) return 0;
  else if (Math.abs(h2p - h1p) <= 180) return h2p - h1p;
  else if (h2p - h1p > 180) return h2p - h1p - 360;
  else if (h2p - h1p < -180) return h2p - h1p + 360;
  else throw new Error();
}

function aHpF(C1: number, C2: number, h1p: number, h2p: number) {
  if (C1 * C2 === 0) return h1p + h2p;
  else if (Math.abs(h1p - h2p) <= 180) return (h1p + h2p) / 2.0;
  else if (Math.abs(h1p - h2p) > 180 && h1p + h2p < 360) return (h1p + h2p + 360) / 2.0;
  else if (Math.abs(h1p - h2p) > 180 && h1p + h2p >= 360) return (h1p + h2p - 360) / 2.0;
  else throw new Error();
}

export function rgbToXyz([r, g, b]: RGB) {
  r /= 255;
  g /= 255;
  b /= 255;

  // Assume sRGB
  r = r > 0.04045 ? ((r + 0.055) / 1.055) ** 2.4 : r / 12.92;
  g = g > 0.04045 ? ((g + 0.055) / 1.055) ** 2.4 : g / 12.92;
  b = b > 0.04045 ? ((b + 0.055) / 1.055) ** 2.4 : b / 12.92;

  const x = r * 0.4124 + g * 0.3576 + b * 0.1805;
  const y = r * 0.2126 + g * 0.7152 + b * 0.0722;
  const z = r * 0.0193 + g * 0.1192 + b * 0.9505;

  return [x * 100, y * 100, z * 100] as XYZ;
}

export function xyzToRgb([x, y, z]: XYZ) {
  x /= 100;
  y /= 100;
  z /= 100;

  let r;
  let g;
  let b;

  r = x * 3.2406 + y * -1.5372 + z * -0.4986;
  g = x * -0.9689 + y * 1.8758 + z * 0.0415;
  b = x * 0.0557 + y * -0.204 + z * 1.057;

  // Assume sRGB
  r = r > 0.0031308 ? 1.055 * r ** (1.0 / 2.4) - 0.055 : r * 12.92;
  g = g > 0.0031308 ? 1.055 * g ** (1.0 / 2.4) - 0.055 : g * 12.92;
  b = b > 0.0031308 ? 1.055 * b ** (1.0 / 2.4) - 0.055 : b * 12.92;

  r = Math.min(Math.max(0, r), 1);
  g = Math.min(Math.max(0, g), 1);
  b = Math.min(Math.max(0, b), 1);

  return [r * 255, g * 255, b * 255] as RGB;
}

export function xyzToLab([x, y, z]: XYZ) {
  x /= 95.047;
  y /= 100;
  z /= 108.883;

  x = x > 0.008856 ? x ** (1 / 3) : 7.787 * x + 16 / 116;
  y = y > 0.008856 ? y ** (1 / 3) : 7.787 * y + 16 / 116;
  z = z > 0.008856 ? z ** (1 / 3) : 7.787 * z + 16 / 116;

  const l = 116 * y - 16;
  const a = 500 * (x - y);
  const b = 200 * (y - z);

  return [l, a, b] as LAB;
}

export function labToXyz([l, a, b]: LAB) {
  let x;
  let y;
  let z;

  y = (l + 16) / 116;
  x = a / 500 + y;
  z = y - b / 200;

  const y2 = y ** 3;
  const x2 = x ** 3;
  const z2 = z ** 3;
  y = y2 > 0.008856 ? y2 : (y - 16 / 116) / 7.787;
  x = x2 > 0.008856 ? x2 : (x - 16 / 116) / 7.787;
  z = z2 > 0.008856 ? z2 : (z - 16 / 116) / 7.787;

  x *= 95.047;
  y *= 100;
  z *= 108.883;

  return [x, y, z] as XYZ;
}

export function rgbToLab([r, g, b]: RGB) {
  return xyzToLab(rgbToXyz([r, g, b]));
}

export function labToRgb([l, a, b]: LAB) {
  return xyzToRgb(labToXyz([l, a, b]));
}

export function diff([L1, a1, b1]: LAB, [L2, a2, b2]: LAB) {
  // Weight factors
  const kL = 1;
  const kC = 1;
  const kH = 1;

  /**
   * Step 1: Calculate C1p, C2p, h1p, h2p
   */
  const C1 = Math.sqrt(Math.pow(a1, 2) + Math.pow(b1, 2)); // (2)
  const C2 = Math.sqrt(Math.pow(a2, 2) + Math.pow(b2, 2)); // (2)

  const aC1C2 = (C1 + C2) / 2.0; // (3)

  const G = 0.5 * (1 - Math.sqrt(Math.pow(aC1C2, 7.0) / (Math.pow(aC1C2, 7.0) + Math.pow(25.0, 7.0)))); // (4)

  const a1p = (1.0 + G) * a1; // (5)
  const a2p = (1.0 + G) * a2; // (5)

  const C1p = Math.sqrt(Math.pow(a1p, 2) + Math.pow(b1, 2)); // (6)
  const C2p = Math.sqrt(Math.pow(a2p, 2) + Math.pow(b2, 2)); // (6)

  const h1p = hpF(b1, a1p); // (7)
  const h2p = hpF(b2, a2p); // (7)

  /**
   * Step 2: Calculate dLp, dCp, dHp
   */
  const dLp = L2 - L1; // (8)
  const dCp = C2p - C1p; // (9)

  const dhp = dhpF(C1, C2, h1p, h2p); // (10)
  const dHp = 2 * Math.sqrt(C1p * C2p) * Math.sin(radians(dhp) / 2.0); // (11)

  /**
   * Step 3: Calculate CIEDE2000 Color-Difference
   */
  const aL = (L1 + L2) / 2.0; // (12)
  const aCp = (C1p + C2p) / 2.0; // (13)

  const aHp = aHpF(C1, C2, h1p, h2p); // (14)
  const T =
    1 -
    0.17 * Math.cos(radians(aHp - 30)) +
    0.24 * Math.cos(radians(2 * aHp)) +
    0.32 * Math.cos(radians(3 * aHp + 6)) -
    0.2 * Math.cos(radians(4 * aHp - 63)); // (15)
  const dRo = 30 * Math.exp(-Math.pow((aHp - 275) / 25, 2)); // (16)
  const RC = Math.sqrt(Math.pow(aCp, 7.0) / (Math.pow(aCp, 7.0) + Math.pow(25.0, 7.0))); // (17)
  const SL = 1 + (0.015 * Math.pow(aL - 50, 2)) / Math.sqrt(20 + Math.pow(aL - 50, 2.0)); // (18)
  const SC = 1 + 0.045 * aCp; // (19)
  const SH = 1 + 0.015 * aCp * T; // (20)
  const RT = -2 * RC * Math.sin(radians(2 * dRo)); // (21)
  const dE = Math.sqrt(
    Math.pow(dLp / (SL * kL), 2) +
      Math.pow(dCp / (SC * kC), 2) +
      Math.pow(dHp / (SH * kH), 2) +
      RT * (dCp / (SC * kC)) * (dHp / (SH * kH))
  ); // (22)
  return dE;
}
