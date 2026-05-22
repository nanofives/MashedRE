# Ghidra Python (Jython) script — bulk write plate + bookmark + FidDB rename for sweep.
# @category Mashed
# Args: <plates_tsv_path> <bucket_name>
# plates_tsv: each line "<rva>\t<plate_text>" (rva with or without 0x)
# bucket_name: used in the bookmark comment "C1 YYYY-MM-DD session:<bucket>"
# Date prefix is read from the plate text (already embedded).
#
# Output JSON-ish lines to stdout: PLATE OK <rva>, BOOK OK <rva>, RENAME <rva> <old> -> <new>, MISS <rva>, ERR <rva> <reason>
#
# Rules:
# - For each RVA: try function_at; if it exists -> write plate, write bookmark, check FidDB comment for rename.
# - If no function: try listing code unit; if non-null -> write plate at listing addr (no bookmark, no rename).
#   If null -> emit MISS line.
# - FidDB rename: check function repeatable comment + the plate text via getComment(0 - 4) for "Library Function:" prefix.
#   If found and Ghidra name still starts with FUN_, rename to the symbol name after "Library Function:".

import sys
import re
from ghidra.program.model.address import Address
from ghidra.program.model.listing import CodeUnit
from ghidra.program.model.symbol import SourceType

PLATE_COMMENT = CodeUnit.PLATE_COMMENT
PRE_COMMENT = CodeUnit.PRE_COMMENT
POST_COMMENT = CodeUnit.POST_COMMENT
EOL_COMMENT = CodeUnit.EOL_COMMENT
REPEATABLE_COMMENT = CodeUnit.REPEATABLE_COMMENT

args = getScriptArgs()
if len(args) < 2:
    print("ERR usage: script <plates_tsv_path> <bucket_name>")
    sys.exit(1)

tsv_path = args[0]
bucket = args[1]

prog = currentProgram
af = prog.getAddressFactory()
fm = prog.getFunctionManager()
bm = prog.getBookmarkManager()
listing = prog.getListing()
st = prog.getSymbolTable()

# Read plates
rows = []
fh = open(tsv_path, "rb")
try:
    data = fh.read().decode("utf-8")
finally:
    fh.close()
for line in data.splitlines():
    if not line.strip():
        continue
    parts = line.split("\t", 1)
    if len(parts) != 2:
        continue
    rva, plate = parts[0].strip(), parts[1].rstrip()
    rows.append((rva, plate))

# Today's date for bookmark
import datetime
today = "2026-05-18"  # session date pinned

bookmark_comment = "C1 %s session:%s" % (today, bucket)

# FidDB regex: a "Library Function:" line followed by a single concrete symbol name.
fid_re = re.compile(r"Library Function:\s*([A-Za-z_][A-Za-z0-9_@$.]+)")

n_plate = 0
n_book = 0
n_rename = 0
n_miss = 0
n_err = 0

for (rva_str, plate) in rows:
    rva = rva_str
    if rva.lower().startswith("0x"):
        rva = rva[2:]
    try:
        addr = af.getAddress(rva)
    except Exception as e:
        print("ERR %s addr_parse %s" % (rva_str, str(e)))
        n_err += 1
        continue
    if addr is None:
        print("ERR %s addr_null" % rva_str)
        n_err += 1
        continue
    fn = fm.getFunctionAt(addr)
    cu = listing.getCodeUnitAt(addr)
    if fn is not None:
        # Write plate at function entry (CodeUnit at the entry)
        try:
            if cu is not None:
                cu.setComment(PLATE_COMMENT, plate)
                n_plate += 1
                print("PLATE OK %s" % rva_str)
            else:
                # Fall back: set function plate via listing at addr
                print("ERR %s no_codeunit_for_fn" % rva_str)
                n_err += 1
                continue
        except Exception as e:
            print("ERR %s plate %s" % (rva_str, str(e)))
            n_err += 1
            continue
        # Bookmark
        try:
            bm.setBookmark(addr, "Note", "first-pass", bookmark_comment)
            n_book += 1
            print("BOOK OK %s" % rva_str)
        except Exception as e:
            print("ERR %s bookmark %s" % (rva_str, str(e)))
            n_err += 1
            continue
        # FidDB check — check function repeatable comment, then code-unit repeatable comment
        comments_to_check = []
        try:
            fc = fn.getComment()
            if fc:
                comments_to_check.append(fc)
            for t in (PLATE_COMMENT, PRE_COMMENT, REPEATABLE_COMMENT, EOL_COMMENT):
                c = cu.getComment(t)
                if c:
                    comments_to_check.append(c)
        except Exception as e:
            pass
        rename_target = None
        for txt in comments_to_check:
            m = fid_re.search(txt)
            if m:
                cand = m.group(1)
                # Ensure single symbol only (not multiple "or" alternatives in same line)
                # Skip if the matched line has " or " or "/" after the symbol
                line = txt[m.start():].split("\n", 1)[0]
                if " or " in line or "/" in line[len("Library Function: "):]:
                    continue
                rename_target = cand
                break
        if rename_target and fn.getName().startswith("FUN_"):
            try:
                fn.setName(rename_target, SourceType.USER_DEFINED)
                n_rename += 1
                print("RENAME %s %s -> %s" % (rva_str, "FUN_" + rva.lower(), rename_target))
            except Exception as e:
                print("ERR %s rename %s" % (rva_str, str(e)))
                n_err += 1
                continue
    else:
        # C0 fallback: try listing code unit
        if cu is not None:
            try:
                cu.setComment(PLATE_COMMENT, plate)
                n_plate += 1
                print("PLATE C0 %s" % rva_str)
            except Exception as e:
                print("ERR %s c0_plate %s" % (rva_str, str(e)))
                n_err += 1
        else:
            print("MISS %s no_function_no_codeunit" % rva_str)
            n_miss += 1

print("SUMMARY plates=%d bookmarks=%d renames=%d misses=%d errors=%d total=%d"
      % (n_plate, n_book, n_rename, n_miss, n_err, len(rows)))
