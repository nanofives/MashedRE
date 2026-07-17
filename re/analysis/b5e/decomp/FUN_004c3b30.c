
/* [C1 2026-05-08] If param_1 == 0.0, returns 0.0 immediately (zero guard). */

float10 FUN_004c3b30(float param_1)

{
  if (param_1 != 0.0) {
    param_1 = (float)(*(int *)(*(int *)(DAT_007d3ffc + DAT_007d3ff8) +
                              ((int)param_1 + 0x800U >> 0xc & 0xfff) * 4) +
                     ((int)param_1 + 0x800U >> 1 & 0x3fc00000));
  }
  return (float10)param_1;
}

