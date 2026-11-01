#!/usr/bin/env bash
# HB-SHADOW-ALLOC-COMPARE pass 2 (PREREG amendment 1): arms S, C0, K0, M1, M2 on .asi 63299e3c, then G on exe 28ad585e.
cd "$(dirname "$0")/../.."
bash verify/hb_shadowalloc_20261101/run_arms_sa.sh || exit $?
echo "[arm G]"; bash verify/hb_shadowalloc_20261101/run_gates.sh; echo "[gates rc=$?]"
