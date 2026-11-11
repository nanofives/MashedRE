#!/bin/bash
# PROV-INSITU: group tests per tier, in sequence (run under test-slot; each game run takes the game lock itself)
trap "exit 1" INT TERM
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/PROV-INSITU
D=D:/MashedWT/PROV-INSITU_data; V=verify/provinsitu_20261111
B="py -3.12 -u re/tools/prov/prov_bisect.py --census $D/census2 --ref $D/r1 --ref2 $D/r2 --pair $D/x_ref,$D/x_ref2 --max-runs 200"
$B --bodies $V/bodies_95.json --tag t95 --split-first || exit 1
$B --bodies $V/bodies_80.json --tag t80 --group 6 || exit 1
$B --bodies $V/bodies_50.json --tag t50 --group 2 || exit 1
$B --bodies $V/bodies_0.json --tag t0 --group 1
