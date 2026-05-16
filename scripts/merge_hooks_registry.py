"""Union-merge two hooks_registry.py files by keying on hook entry name.

Usage:
    python merge_hooks_registry.py <head_path> <branch_path> <out_path>

Strategy:
    1. Parse each file via ast — they're Python dict literals at top-level.
    2. Re-extract source text for each entry via regex (so we preserve comments
       and exact formatting; ast would lose that).
    3. Union by hook-name key:
         - HEAD-only entries: keep HEAD text.
         - Branch-only entries: keep branch text.
         - Both: keep HEAD text (HEAD is "newer" — it already absorbed prior
           merges).
    4. Reassemble file: HEAD preamble + entries (HEAD order, then new branch
       entries appended) + closing `}`.

This is used by the frida-sweep when text-conflict resolution of
hooks_registry.py would otherwise break dict syntax.
"""

import ast
import os
import re
import sys


HOOK_KEY_RE = re.compile(r"^    '([a-z_0-9]+)': \{$", re.MULTILINE)


def split_into_entries(text):
    """Return (preamble, entries_dict, trailing).

    preamble is everything up to and including the line `HOOKS = {`.
    entries_dict maps hook_name -> entry_text (multi-line, including the
    leading `    'name': {` line and the trailing `    },` line).
    trailing is the closing `}` and anything after.
    """
    m = re.search(r"^HOOKS = \{\s*$", text, re.MULTILINE)
    if not m:
        raise ValueError("could not find `HOOKS = {` line")
    body_start = m.end() + 1  # skip the newline after `{`
    # The closing `}` is the LAST top-level `}` — find it by scanning balance.
    depth = 1
    i = body_start
    in_string = None
    while i < len(text):
        c = text[i]
        if in_string:
            if c == "\\":
                i += 2
                continue
            if c == in_string:
                in_string = None
        elif c in "'\"":
            in_string = c
        elif c == "#":
            # comment to end of line
            j = text.find("\n", i)
            i = j if j != -1 else len(text)
            continue
        elif c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0:
                break
        i += 1
    if depth != 0:
        raise ValueError("unbalanced braces (file truncated mid-dict?)")
    body_text = text[body_start:i]  # between the opening `{\n` and closing `}`
    preamble = text[: body_start]
    trailing = text[i:]  # starts with `}`

    # Now split body_text into per-entry chunks.  An entry starts at a line
    # matching `    'name': {`.  Find all such starts, plus an end sentinel.
    starts = [(m.start(), m.group(1)) for m in HOOK_KEY_RE.finditer(body_text)]
    entries = {}
    leading_block = ""  # comments/whitespace before the first entry
    if starts:
        leading_block = body_text[: starts[0][0]]
    for idx, (start, name) in enumerate(starts):
        end = starts[idx + 1][0] if idx + 1 < len(starts) else len(body_text)
        entries[name] = body_text[start:end]
    return preamble, leading_block, entries, trailing


def main():
    head_path, branch_path, out_path = sys.argv[1:4]
    with open(head_path, "r", encoding="utf-8", newline="") as f:
        head_text = f.read()
    with open(branch_path, "r", encoding="utf-8", newline="") as f:
        branch_text = f.read()

    head_preamble, head_lead, head_entries, head_trailing = split_into_entries(head_text)
    _, _, branch_entries, _ = split_into_entries(branch_text)

    # Compute additions: hooks in branch that aren't in HEAD.
    new_keys = [k for k in branch_entries if k not in head_entries]
    print(f"HEAD: {len(head_entries)} entries  branch: {len(branch_entries)}  new: {len(new_keys)}")
    if new_keys:
        print("New keys:", ", ".join(new_keys))

    # Reassemble: preamble + head_lead + head entries (in original order) +
    # new branch entries (appended) + trailing.
    out = [head_preamble, head_lead]
    for k in head_entries:
        out.append(head_entries[k])
    for k in new_keys:
        out.append(branch_entries[k])
    out.append(head_trailing)
    merged = "".join(out)

    # Validate it parses.
    try:
        ast.parse(merged)
    except SyntaxError as e:
        sys.stderr.write(f"merged file fails ast.parse: {e}\n")
        # Still write it for inspection.
        with open(out_path + ".bad", "w", encoding="utf-8", newline="") as f:
            f.write(merged)
        sys.exit(1)
    with open(out_path, "w", encoding="utf-8", newline="") as f:
        f.write(merged)
    print(f"wrote {out_path}: {len(merged)} bytes")


if __name__ == "__main__":
    main()
