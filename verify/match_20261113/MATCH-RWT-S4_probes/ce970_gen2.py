"""Writes ce970_v2.txt: exit structures x local declaration order for 0x004ce970 (image destroyed before mask on
the failure path, as at 0x004cea03 / 0x004cea48)."""
import os
D = {'im': 'RwImage *image;\n  RwImage *mask;', 'mi': 'RwImage *mask;\n  RwImage *image;'}
S = {}
S['g1'] = """  image = FUN_004ce390(param_1);
  if (!image) goto ret0;
  if (param_2 && *param_2) {
    mask = FUN_004ce390(param_2);
    if (!mask) { IMAGEDESTROY(image); goto ret0; }
    if (!FUN_004cde50(mask) || !FUN_004cdf20(image, mask)) { IMAGEDESTROY(image); IMAGEDESTROY(mask); goto ret0; }
    IMAGEDESTROY(mask);
  }
  return image;
ret0:
  return 0;"""
S['g2'] = """  image = FUN_004ce390(param_1);
  if (image) {
    if (!param_2 || !*param_2) return image;
    mask = FUN_004ce390(param_2);
    if (mask) {
      if (FUN_004cde50(mask) && FUN_004cdf20(image, mask)) { IMAGEDESTROY(mask); return image; }
      IMAGEDESTROY(image);
      IMAGEDESTROY(mask);
    } else {
      IMAGEDESTROY(image);
    }
  }
  return 0;"""
S['g3'] = """  image = FUN_004ce390(param_1);
  if (image) {
    if (param_2 && *param_2) {
      mask = FUN_004ce390(param_2);
      if (!mask) {
        IMAGEDESTROY(image);
        return 0;
      }
      if (FUN_004cde50(mask) && FUN_004cdf20(image, mask)) {
        IMAGEDESTROY(mask);
        return image;
      }
      IMAGEDESTROY(image);
      IMAGEDESTROY(mask);
      return 0;
    }
    return image;
  }
  return 0;"""
S['g4'] = """  image = FUN_004ce390(param_1);
  if (image) {
    if (param_2 && *param_2) {
      mask = FUN_004ce390(param_2);
      if (mask) {
        if (!FUN_004cde50(mask) || !FUN_004cdf20(image, mask)) {
          IMAGEDESTROY(image);
          IMAGEDESTROY(mask);
          return 0;
        }
        IMAGEDESTROY(mask);
        return image;
      }
      IMAGEDESTROY(image);
      return 0;
    }
  }
  return image;"""
S['g5'] = """  image = FUN_004ce390(param_1);
  if (image) {
    if (param_2 && *param_2) {
      mask = FUN_004ce390(param_2);
      if (mask) {
        if (FUN_004cde50(mask) && FUN_004cdf20(image, mask)) {
          IMAGEDESTROY(mask);
          return image;
        }
        IMAGEDESTROY(image);
        IMAGEDESTROY(mask);
      } else {
        IMAGEDESTROY(image);
      }
      return 0;
    }
  }
  return image;"""
S['g6'] = """  image = FUN_004ce390(param_1);
  if (image && param_2 && *param_2) {
    mask = FUN_004ce390(param_2);
    if (!mask) { IMAGEDESTROY(image); return 0; }
    if (FUN_004cde50(mask) && FUN_004cdf20(image, mask)) { IMAGEDESTROY(mask); return image; }
    IMAGEDESTROY(image);
    IMAGEDESTROY(mask);
    return 0;
  }
  return image;"""
out = []
for sk, sv in S.items():
    for dk, dv in D.items():
        out.append(f"@@ {sk}{dk}\n// MATCH: 0x004ce970 FUN_004ce970\nRwImage *FUN_004ce970(const char *param_1, const char *param_2)\n{{\n  {dv}\n\n{sv}\n}}\n")
open(os.path.join(os.path.dirname(__file__), 'ce970_v2.txt'), 'w', encoding='utf-8').write('\n'.join(out))
