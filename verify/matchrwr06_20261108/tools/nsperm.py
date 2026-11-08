"""nsperm.py VA SIZE <match_permute args...> : run match_permute with an explicit size for a row missing from pc_funcs.csv."""
import sys, runpy
sys.path.insert(0, 're/tools/match')
import match
va, n = int(sys.argv[1], 16), int(sys.argv[2])
match.function_table()[va] = n
sys.argv = ['match_permute.py'] + sys.argv[3:]
runpy.run_path('re/tools/match/match_permute.py', run_name='__main__')
