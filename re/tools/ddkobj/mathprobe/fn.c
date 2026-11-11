/* function (non-intrinsic) forms of sin and cos: the CRT's _sin / _cos entry points */
#include <math.h>
#pragma function(sin, cos)
double f_sin(double x) { return sin(x); }
double f_cos(double x) { return cos(x); }
