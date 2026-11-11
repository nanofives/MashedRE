#include <stdio.h>
#include <stdarg.h>
static void t(const char *fmt, ...) { char b[64]; int r; va_list a; va_start(a, fmt); r = _vsnprintf(b, 16, fmt, a); va_end(a);
  b[16] = 0; printf("[%s] r=%d -> %.16s|\n", fmt, r, r < 0 ? b : b); }
int main(void) { volatile double z = 0.0, o = 1.0; double inf = o / z, nan = inf * z;
  t("%e", 1.0); t("%g", 1e-5); t("%g", 1e20); t("%E", 12345.678); t("%f", 0.1); t("%.0f", 2.5); t("%.0f", 3.5);
  t("%5.2f", -0.004); t("%p", (void*)0x1234); t("%s", (char*)0); t("%x", -1); t("%d", 7); t("%-4d|", 7);
  t("%f", inf); t("%f", nan); t("%f", -nan); t("%e", inf); t("%g", nan); t("%.3f", 1.0005); t("%a", 1.0);
  t("0123456789abcdefXYZ"); t("0123456789abcdef"); t("%hd", 70000); t("%I64d", (__int64)-5); t("%c", 65); t("%.2s", "abc");
  return 0; }
