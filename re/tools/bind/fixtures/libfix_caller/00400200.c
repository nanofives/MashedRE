/* Fixture matched source (BIND-LIB-ROUTE unit test, not game code): a caller of two library rvas, in the shape of a
   re/match source (Ghidra names, its own prototypes). */
int FUN_00400100(int a, int b);
int FUN_00400110(int a, int b);

int FUN_00400200(int x)
{
  return FUN_00400100(x, 2) + FUN_00400110(x, 3);
}
