
/* [C2 2026-05-20] Signature: `undefined4 * FUN_004d8480(undefined4 *param_1)`. */

undefined4 * FUN_004d8480(undefined4 *param_1)

{
  if ((*(int *)(DAT_007d6c5c + DAT_007d3ff8) == 0) &&
     (*(int *)(DAT_007d6c5c + 4 + DAT_007d3ff8) == -0x80000000)) {
    if ((param_1[1] & 0x80000000) == 0) {
      *(undefined4 *)(DAT_007d6c5c + DAT_007d3ff8) = *param_1;
    }
    else {
      *(undefined4 *)(DAT_007d6c5c + DAT_007d3ff8) = 0;
    }
    *(undefined4 *)(DAT_007d6c5c + 4 + DAT_007d3ff8) = param_1[1];
    return param_1;
  }
  return param_1;
}

