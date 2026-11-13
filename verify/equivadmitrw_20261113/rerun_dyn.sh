#!/bin/bash
D=verify/equivadmitrw_20261113
py -3.12 re/tools/match/equiv_dyn.py --tsv $D/admit_in.tsv --runs 16 --out $D/dyn_default.tsv 2>&1 | tee $D/dyn_default.log
py -3.12 re/tools/match/equiv_dyn.py --tsv $D/admit_in.tsv --runs 16 --seeded --out $D/dyn_seeded.tsv 2>&1 | tee $D/dyn_seeded.log
