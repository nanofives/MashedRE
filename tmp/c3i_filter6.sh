#!/bin/bash
cd /c/Users/maria/Desktop/Proyectos/Mashed

SUPPORTED_ARG_TYPES=$(grep -oE "arg_type === '[a-z_0-9]+'" re/frida/diff_template.js \
    | sort -u | sed -E "s/arg_type === '([^']+)'/\1/")

> /tmp/c3i_viable.txt
> /tmp/c3i_drift.txt
> /tmp/c3i_uncertain.txt
> /tmp/c3i_callee_gate.txt
> /tmp/c3i_argtype_miss.txt
> /tmp/c3i_note_missing.txt
> /tmp/c3i_already_c3.txt

check_callee() {
    local c="$1"
    c=$(echo "$c" | sed 's/^FUN_//; s/^sub_//; s/^0x//' | tr 'A-F' 'a-f')
    while [[ ${#c} -lt 8 ]]; do c="0${c}"; done
    local row=$(awk -F',' -v r="$c" '$1==r {print $4; exit}' hooks.csv)
    if [[ -z "$row" ]]; then
        echo "MISSING:$c"
        return 1
    fi
    case "$row" in
        C2|C3|C4) return 0 ;;
        *) echo "$c=$row"; return 1 ;;
    esac
}

# A note has an "unresolved inline UNCERTAIN" iff:
#  - it contains [UNCERTAIN ...] outside ## Uncertainties section, AND
#  - one of the U-IDs it cites is NOT in UNCERTAINTIES.md (i.e., not catalogued)
# OR
#  - it has bare [UNCERTAIN] without U-ID (unnumbered → uncatalogued)
has_unresolved_uncertain() {
    local note="$1"
    # Extract inline UNCERTAIN lines (outside ## Uncertainties)
    local lines=$(awk '
        /^## Uncertainties[[:space:]]*$/ {in_unc=1; next}
        /^## / && in_unc {in_unc=0}
        !in_unc && /\[UNCERTAIN/ && !/[Nn]o \[UNCERTAIN/ && !/\[UNCERTAIN[^]]*\] marker[s]? in/ {print}
    ' "$note")
    [[ -z "$lines" ]] && return 1

    # Check for bare [UNCERTAIN] (no U-ID) - hard refuse
    if echo "$lines" | grep -qE '\[UNCERTAIN\]'; then
        echo "BARE_UNCERTAIN"
        return 0
    fi

    # Check each U-ID is in UNCERTAINTIES.md
    local u_ids=$(echo "$lines" | grep -oE "U-[0-9]+" | sort -u)
    [[ -z "$u_ids" ]] && return 1
    for uid in $u_ids; do
        if ! grep -q "$uid" UNCERTAINTIES.md 2>/dev/null; then
            echo "UNFILED:$uid"
            return 0
        fi
    done
    return 1
}

while IFS='|' read -r rva subsystem notepath; do
    note=$(echo "$notepath" | cut -d';' -f1)

    if [[ ! -f "$note" ]]; then
        echo "$rva|$subsystem|$note" >> /tmp/c3i_note_missing.txt
        continue
    fi

    has_impl=$(grep -rl "0x00${rva}\|sub_${rva}\|FUN_${rva}" mashedmod/src/ 2>/dev/null | head -1)
    has_diff=$(ls log/diff_*${rva}*.csv 2>/dev/null | head -1)
    if [[ -n "$has_impl" && -n "$has_diff" ]]; then
        if grep -l "True" "$has_diff" >/dev/null 2>&1; then
            echo "$rva|$subsystem|$note|impl=$has_impl" >> /tmp/c3i_drift.txt
            continue
        fi
    fi

    higher=$(awk -F',' -v r="$rva" '$1==r && ($4=="C3" || $4=="C4") {print $4; exit}' hooks.csv)
    if [[ -n "$higher" ]]; then
        echo "$rva|$subsystem|$note|already=$higher" >> /tmp/c3i_already_c3.txt
        continue
    fi

    unc_result=$(has_unresolved_uncertain "$note")
    if [[ -n "$unc_result" ]]; then
        echo "$rva|$subsystem|$note|reason=$unc_result" >> /tmp/c3i_uncertain.txt
        continue
    fi

    callees_in_body=$(grep -oE "(FUN_|sub_)[0-9a-fA-F]{8}" "$note" | sed 's/^FUN_//; s/^sub_//' | tr 'A-F' 'a-f' | sort -u)
    rva_lc=$(echo "$rva" | tr 'A-F' 'a-f')
    callees_filtered=$(echo "$callees_in_body" | grep -v "^${rva_lc}$" || true)

    callee_fail=""
    if [[ -n "$callees_filtered" ]]; then
        for c in $callees_filtered; do
            result=$(check_callee "$c")
            if [[ -n "$result" ]]; then
                callee_fail="$result"
                break
            fi
        done
    fi
    if [[ -n "$callee_fail" ]]; then
        echo "$rva|$subsystem|$note|fail=$callee_fail" >> /tmp/c3i_callee_gate.txt
        continue
    fi

    note_arg=$(awk '/^(arg_type|signature_arg_type):[[:space:]]*/{
        sub(/^[^:]+:[[:space:]]*/,"")
        gsub(/["]/,"")
        print
        exit
    }' "$note")
    note_arg=$(echo "$note_arg" | xargs)
    if [[ -n "$note_arg" ]]; then
        if ! echo "$SUPPORTED_ARG_TYPES" | grep -qx "$note_arg"; then
            echo "$rva|$subsystem|$note|arg_type=$note_arg" >> /tmp/c3i_argtype_miss.txt
            continue
        fi
    fi

    size=$(awk '/^size_bytes:/{print $2; exit}' "$note")
    callee_count=$(echo "$callees_filtered" | grep -c . || echo 0)
    echo "$rva|$subsystem|$note|size=$size|arg=$note_arg|callees=$callee_count" >> /tmp/c3i_viable.txt

done < /tmp/c3i_dedup.txt

echo "=== FILTER RESULTS ==="
echo "Total candidates:      $(wc -l < /tmp/c3i_dedup.txt)"
echo "Viable:                $(wc -l < /tmp/c3i_viable.txt)"
echo "Drift (stale-impl):    $(wc -l < /tmp/c3i_drift.txt)"
echo "Inline UNCERTAIN:      $(wc -l < /tmp/c3i_uncertain.txt)"
echo "Callee-gate failed:    $(wc -l < /tmp/c3i_callee_gate.txt)"
echo "arg_type unsupported:  $(wc -l < /tmp/c3i_argtype_miss.txt)"
echo "Note missing:          $(wc -l < /tmp/c3i_note_missing.txt)"
echo "Already C3+:           $(wc -l < /tmp/c3i_already_c3.txt)"
echo ""
echo "=== VIABLE BREAKDOWN ==="
awk -F'|' 'NF>=3 {print $2}' /tmp/c3i_viable.txt | sort | uniq -c
