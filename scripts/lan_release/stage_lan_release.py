#!/usr/bin/env python3
r"""stage_lan_release.py - build the LAN release folder of mashed_re.exe for the mini-server (Mariano 2026-10-29,
modelled on SimGolf's scripts/lan_release, itself SimCity's and TD5RE's td5re-files).

The folder is what a player PC keeps:
  mashed_re.exe               the standalone (x86, static CRT; binds only System32 DLLs: d3d9, dsound, user32, gdi32)
  original\                   the game data the exe reads, by the same relative paths as in the repo
                              ("original/TOASTART/...", exe_main.cpp WinMain finds it beside the exe):
    TOASTART\, toastaudio\    the user's own Mashed: Fully Loaded install (TOASTART\PC\MOVIES\backup\ left out:
                              dev copies of the intro movies that skip_intro replaced;
                              the staged PC\MOVIES\<name>.mpg of each is that backup copy, the real movie, L-63)
    gamesave.bin              the save the exe replays at boot (never written by the standalone); a seed file:
                              the updater installs it once and never replaces it
    mashed_image.bin          data, not a binary: the file-backed .rdata/.data bytes of MASHED.exe.unpatched in the
                              re/tools/image format, read only on the MASHED_ORIG_TOP + MASHED_PORTSTATE_IMAGE path
                              (IMG-2-LOADER, L-149 a)
  Play Mashed.bat             starts the exe (the updater also makes a "Mashed (RE)" shortcut)
  mashed_update.ps1, update.bat   the updater (copies of the files beside this script)
  README.txt
  manifest.json               {label, generated, files: [{path, size, sha256, seed}]}

Left out of original\: every exe, dll, asi, shim, log, trace and backup the dev install carries (MASHED.exe and
its .unpatched / .pre* copies, launch.exe, d3d9/dinput8 proxies, mashed_re_dev.asi*, *_selftest.log, ...). The release
carries no original binary: the one resource the standalone took from MASHED.exe, the loading-disc texture
RWTEXDICTIONARY/404, is embedded in mashed_re.exe (mashedmod/assets/mashed_re.rc, D5-ICON-ASSET, L-64 (b)).
The dev harness (d3d9 shim, dinput8 loader, mashed_re_dev.asi) stays in the repo after D5 but is never shipped, so this
exclusion is permanent.
The exe writes its own files beside itself (videocfg.bin on first run, mashed_re_gamesave.bin, mashed_re.log, log\);
none is in the manifest, so the updater never touches them (and it removes a MASHED.exe an earlier release left in
original\, since the manifest no longer lists it).
Files are hard-linked from the sources when they share a volume (no extra disk), copied otherwise.
The stage folder lives under mashedmod/build (gitignored); never stage into a tracked folder.

  py -3.12 stage_lan_release.py --app <dir with mashed_re.exe> --install <original> --out <folder> [--label X]
"""
import argparse
import datetime
import hashlib
import json
import os
import shutil
import sys

HERE = os.path.dirname(os.path.abspath(__file__))

APP_FILES = ["mashed_re.exe"]
DATA_DIRS = ["TOASTART", "toastaudio"]
DATA_SKIP_DIRS = {"toastart/pc/movies/backup"}          # lower-case, relative to original\
MOVIES_DIR = "toastart/pc/movies"                        # its backup\<name>.mpg is the real movie
SKIP_SUFFIXES = (".log", ".bak", ".tmp", ".dl_tmp")
CLIENT_FILES = ["mashed_update.ps1", "update.bat", "Play Mashed.bat", "README.txt"]
SEED = {"original/gamesave.bin"}


def place(src, dst):
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    if os.path.exists(dst):
        os.remove(dst)
    try:
        os.link(src, dst)
    except OSError:
        shutil.copy2(src, dst)


def sha256(p):
    h = hashlib.sha256()
    with open(p, "rb") as f:
        for b in iter(lambda: f.read(1 << 20), b""):
            h.update(b)
    return h.hexdigest()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--app", required=True, help="folder holding mashed_re.exe")
    ap.add_argument("--install", required=True, help="the Mashed install (original\\)")
    ap.add_argument("--out", required=True)
    ap.add_argument("--label", default="")
    a = ap.parse_args()

    out = os.path.abspath(a.out)
    if os.path.isdir(os.path.join(out, ".git")):
        sys.exit("[!] refusing to stage into a git checkout: %s" % out)
    os.makedirs(out, exist_ok=True)
    wanted = set()

    for f in APP_FILES:
        src = os.path.join(a.app, f)
        if not os.path.isfile(src):
            sys.exit("[!] missing %s" % src)
        place(src, os.path.join(out, f)); wanted.add(f)

    marker = os.path.join(a.install, "TOASTART", "Common", "Frontend.piz")
    if not os.path.isfile(marker):
        sys.exit("[!] %s has no TOASTART\\Common\\Frontend.piz" % a.install)
    for top in DATA_DIRS:
        for root, dirs, files in os.walk(os.path.join(a.install, top)):
            rel_root = os.path.relpath(root, a.install).replace("\\", "/").lower()
            dirs[:] = sorted(d for d in dirs if (rel_root + "/" + d.lower()) not in DATA_SKIP_DIRS)
            for fn in sorted(files):
                if fn.lower().endswith(SKIP_SUFFIXES):
                    continue
                full = os.path.join(root, fn)
                rel = os.path.join("original", os.path.relpath(full, a.install))
                if rel_root == MOVIES_DIR:
                    real = os.path.join(root, "backup", fn)
                    if os.path.isfile(real):        # the real boot movie, not the skip_intro stub
                        full = real
                place(full, os.path.join(out, rel)); wanted.add(rel)

    save = os.path.join(a.install, "gamesave.bin")
    if os.path.isfile(save):
        place(save, os.path.join(out, "original", "gamesave.bin")); wanted.add(os.path.join("original", "gamesave.bin"))

    # IMG-2-LOADER (L-149 a): original\mashed_image.bin, the file-backed .rdata/.data image the MASHED_ORIG_TOP path
    # loads under MASHED_PORTSTATE_IMAGE=1 (Compat/PortStateImage.cpp). Built from <install>\MASHED.exe.unpatched by
    # re/tools/image/mashed_image.py (the extract_image.py format); a missing or non-anchor .unpatched is a hard stop.
    # Only the stage's own copy is written; nothing in <install> is touched. The exe itself stays the only binary.
    sys.path.insert(0, os.path.join(HERE, "..", "..", "re", "tools", "image"))
    import mashed_image as mi
    unp = os.path.join(a.install, "MASHED.exe.unpatched")
    if not os.path.isfile(unp):
        sys.exit("[!] missing %s (source of original\\mashed_image.bin)" % unp)
    with open(unp, "rb") as fh:
        ub = fh.read()
    if hashlib.sha256(ub).hexdigest().upper() != mi.ANCHOR_SHA256 or len(ub) != mi.ANCHOR_SIZE:
        sys.exit("[!] %s is not the anchor %s" % (unp, mi.ANCHOR_SHA256))
    img = mi.build(ub)
    mi.parse(img)
    rel = os.path.join("original", "mashed_image.bin")
    dst = os.path.join(out, rel)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    if os.path.exists(dst):
        os.remove(dst)          # the stage's previous copy (under mashedmod/build), never the install's
    with open(dst, "xb") as fh:
        fh.write(img)
    wanted.add(rel)
    print("[+] original\\mashed_image.bin %d bytes sha256 %s" % (len(img), hashlib.sha256(img).hexdigest().upper()))

    for f in CLIENT_FILES:
        place(os.path.join(HERE, f), os.path.join(out, f)); wanted.add(f)

    # drop what an earlier stage left that this one no longer ships (never the manifest)
    wanted_norm = {w.replace("\\", "/").lower() for w in wanted}
    for root, dirs, files in os.walk(out):
        for fn in files:
            rel = os.path.relpath(os.path.join(root, fn), out).replace("\\", "/")
            if rel.lower() != "manifest.json" and rel.lower() not in wanted_norm:
                os.remove(os.path.join(root, fn))

    entries = []
    for rel in sorted(wanted, key=lambda s: s.lower()):
        p = os.path.join(out, rel)
        r = rel.replace("\\", "/")
        e = {"path": r, "size": os.path.getsize(p), "sha256": sha256(p)}
        if r.lower() in SEED:
            e["seed"] = True
        entries.append(e)
    man = {"label": a.label or "Mashed RE", "generated": datetime.datetime.now().strftime("%Y-%m-%d %H:%M"),
           "files": entries}
    with open(os.path.join(out, "manifest.json"), "w", encoding="utf-8", newline="\n") as fh:
        json.dump(man, fh, indent=1)
    print("[+] staged %d files, %.1f MB -> %s" % (len(entries), sum(e["size"] for e in entries) / 1e6, out))


if __name__ == "__main__":
    main()
