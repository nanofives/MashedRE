"""Writes ce970_v3.txt: exit structures x local declaration order for 0x004ce970 (image destroyed before mask on
the failure path, as at 0x004cea03 / 0x004cea48)."""
import os
D = {'im': 'RwImage *image;\n  RwImage *mask;', 'mi': 'RwImage *mask;\n  RwImage *image;'}
S = {}
S['g1'] = """  image = FUN_004ce390(param_1);
  if (!image) goto ret0;
  if (param_2 && *param_2) {
    mask = FUN_004ce390(param_2);
    if (!mask) { ImgDestroy(image); goto ret0; }
    if (!FUN_004cde50(mask) || !FUN_004cdf20(image, mask)) { ImgDestroy(image); ImgDestroy(mask); goto ret0; }
    ImgDestroy(mask);
  }
  return image;
ret0:
  return 0;"""
S['g2'] = """  image = FUN_004ce390(param_1);
  if (image) {
    if (!param_2 || !*param_2) return image;
    mask = FUN_004ce390(param_2);
    if (mask) {
      if (FUN_004cde50(mask) && FUN_004cdf20(image, mask)) { ImgDestroy(mask); return image; }
      ImgDestroy(image);
      ImgDestroy(mask);
    } else {
      ImgDestroy(image);
    }
  }
  return 0;"""
S['g3'] = """  image = FUN_004ce390(param_1);
  if (image) {
    if (param_2 && *param_2) {
      mask = FUN_004ce390(param_2);
      if (!mask) {
        ImgDestroy(image);
        return 0;
      }
      if (FUN_004cde50(mask) && FUN_004cdf20(image, mask)) {
        ImgDestroy(mask);
        return image;
      }
      ImgDestroy(image);
      ImgDestroy(mask);
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
          ImgDestroy(image);
          ImgDestroy(mask);
          return 0;
        }
        ImgDestroy(mask);
        return image;
      }
      ImgDestroy(image);
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
          ImgDestroy(mask);
          return image;
        }
        ImgDestroy(image);
        ImgDestroy(mask);
      } else {
        ImgDestroy(image);
      }
      return 0;
    }
  }
  return image;"""
S['g6'] = """  image = FUN_004ce390(param_1);
  if (image && param_2 && *param_2) {
    mask = FUN_004ce390(param_2);
    if (!mask) { ImgDestroy(image); return 0; }
    if (FUN_004cde50(mask) && FUN_004cdf20(image, mask)) { ImgDestroy(mask); return image; }
    ImgDestroy(image);
    ImgDestroy(mask);
    return 0;
  }
  return image;"""
import sys
kw = sys.argv[1] if len(sys.argv) > 1 else 'static __forceinline'
PRE = kw + """ int ImgDestroy(RwImage *img)
{
  IMAGEDESTROY(img);
  return 1;
}
"""
out = [PRE]
for sk, sv in S.items():
    for dk, dv in D.items():
        out.append(f"@@ {sk}{dk}\n// MATCH: 0x004ce970 FUN_004ce970\nRwImage *FUN_004ce970(const char *param_1, const char *param_2)\n{{\n  {dv}\n\n{sv}\n}}\n")
open(os.path.join(os.path.dirname(__file__), sys.argv[2] if len(sys.argv) > 2 else 'ce970_v3.txt'), 'w', encoding='utf-8').write('\n'.join(out))
