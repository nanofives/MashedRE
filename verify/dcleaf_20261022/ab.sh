# usage: ab.sh <name> <hooks> [extra scenario args...]  (env passed through)
R=/d/MashedWT/dcleaf_root/original; N=$1; H=$2; shift 2
rm -f $R/phys_c4_*selftest.log
MASHED_ROOT='D:/MashedWT/dcleaf_root' MASHED_PHYS_SELFTEST_CAP=${CAP:-20} py -3.12 re/frida/scenario_launch.py --hooks $H --cars 4 --hold ${HOLD:-40} "$@" > /d/MashedWT/dcleaf_data/a6b_ab/$N.out 2>&1
for f in $R/phys_c4_*selftest.log; do [ -f "$f" ] && cp "$f" /d/MashedWT/dcleaf_data/a6b_ab/$N.$(basename $f .log | sed 's/phys_c4_//; s/_?selftest//; s/^$/a4/').log; done
grep -E 'VERDICT|Traceback|crash|0x[0-9a-f]+ ACCESS' /d/MashedWT/dcleaf_data/a6b_ab/$N.out | head -3
