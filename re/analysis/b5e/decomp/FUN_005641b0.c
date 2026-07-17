
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-19] AABB-containment test with hysteresis. `param_1` and `param_2` are 8-float AABBs
   (min at +0..+8, max at +0x10..+0x18). */

bool FUN_005641b0(float *param_1,float *param_2)

{
  return (byte)((*param_1 < param_2[4] * _DAT_005e5418 + *param_2 * _DAT_005cc318) +
                ((param_1[1] < param_2[5] * _DAT_005e5418 + param_2[1] * _DAT_005cc318) +
                (param_1[2] < param_2[6] * _DAT_005e5418 + param_2[2] * _DAT_005cc318) * '\x02') *
                '\x02' | (*param_2 * _DAT_005e5418 + param_2[4] * _DAT_005cc318 < param_1[4]) +
                         ((param_2[1] * _DAT_005e5418 + param_2[5] * _DAT_005cc318 < param_1[5]) +
                         (param_2[2] * _DAT_005e5418 + param_2[6] * _DAT_005cc318 < param_1[6]) *
                         '\x02') * '\x02') == 7;
}

