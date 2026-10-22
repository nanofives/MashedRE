# usage: ab_leaf.sh <name> <hooks> [extra scenario args...]   (env passed through)
R=/d/MashedWT/dcmix_root/original; N=$1; H=$2; shift 2
O=/d/MashedWT/dcmix_data/leaf_ab
rm -f $R/leaf_selftest.log $R/phys_c4_*selftest.log
MASHED_ROOT='D:/MashedWT/dcmix_root' MASHED_LEAF_SELFTEST=1 py -3.12 re/frida/scenario_launch.py --hooks $H --cars 4 --hold ${HOLD:-60} "$@" > $O/$N.out 2>&1
[ -f $R/leaf_selftest.log ] && cp $R/leaf_selftest.log $O/$N.leaf.log
grep -E 'VERDICT|Traceback|crash|ACCESS' $O/$N.out | head -3
grep '^SUM' $O/$N.leaf.log | tail -2
