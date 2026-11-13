#!/bin/bash
# PROV-WRITESET-SCENARIO: decode every armed group run (wsa_gNN) with prov_writeset.py, then join into SCEN_TABLE.tsv
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/PROV-WRITESET-SCENARIO
V=verify/provwritesetscen_20261112; D=D:/MashedWT/PROV-WRITESET-SCENARIO_data; I=D:/MashedWT/PROV-INSITU_data
OUT=$D/${PFX:-wsa}_all.tsv; : > $OUT
while read g rv; do
  py -3.12 re/tools/prov/prov_writeset.py --bodies verify/provinsitu_20261111/bodies_all.json --run $D/${PFX:-wsa}_$g \
     --allowed-ref $I/ref_a,$I/ref_b --rvas $rv --out $D/${PFX:-wsa}_$g.tsv --stores-out $D/${PFX:-wsa}_$g.sites.tsv | tail -1
  [ -s $OUT ] && tail -n +2 $D/${PFX:-wsa}_$g.tsv >> $OUT || cat $D/${PFX:-wsa}_$g.tsv > $OUT
done < ${GROUPS_FILE:-$V/groups.txt}
