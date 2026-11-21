/* Fixture library (BIND-LIB-ROUTE): a second definition of fix_dup, so a lib rule naming it is ambiguous (refused). */
int fix_dup (void) {
  return 2;
}

static int
fix_static2 (void)
{
  return 3;
}
