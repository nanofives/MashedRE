r"""contcfg_ui.py — a rebinding configurator for Mashed's controller-config files.

Writes `contcfg<slot>.bin`, which the stock loader consumes directly: the on-disk
file is a raw memcpy of the in-memory record at DAT_007e95c0 + slot*0x200, with
no header, no version field and no checksum (FUN_004971b0 fread / FUN_00497230
fwrite, both 0x200). The whole chain -- hand-written file -> loader -> device-name
validation -> binding table -> ReadInputForAction -> descriptor -> frontend
transition -- was verified end-to-end on 2026-08-29 (re/frida/verify_rebind.py,
3/3 including a negative control).

Record layout and every action name come from re/analysis/structs/contcfg_record.md.
This module reuses contcfg_edit.py rather than restating the format.

TWO THINGS THAT BITE, both enforced in the UI:

1. DEVICE NAME VALIDATION. FUN_00498510's second pass accepts a loaded record only
   if record[+0x000] == 6 AND strcmp(record+0x004, default+0x004) == 0 -- the
   device NAME must match the device the slot was enumerated for. A wrong name
   does not error: the load "succeeds", is silently overwritten with defaults, and
   looks exactly like the file never being read. So the name is not cosmetic, and
   this tool refuses to write a record with an empty name unless you force it.

2. SLOT ASSIGNMENT IS NOT FIXED. FUN_00498510 gives slots 0..joycount-1 to joypads
   and slot joycount to the keyboard. On the dev machine a Keychron keyboard
   enumerates as TWO DirectInput GAMECTRL devices, so the keyboard landed in slot
   2 and contcfg0.bin was a JOYPAD config. Never assume slot 0. Use "Detect from
   running game" to read the live table.

JOYPAD AXES ARE NOT REBINDABLE HERE. For a joypad record (type 1) actions 9..12
ignore the stored value entirely -- the axis floats come from a hardcoded
per-joypad table at 0x0077311c / 0x00773120 (FUN_00497310 at 0x0049733d /
0x0049734e). Changing which stick steers needs a code change, not a config change.
Keyboard axis rebinding IS expressible. The UI disables those rows for joypads.

Usage:
    py -3.12 re\tools\contcfg_ui.py
    py -3.12 re\tools\contcfg_ui.py --dir original
"""
import argparse
import ctypes
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import contcfg_edit as cc  # noqa: E402

try:
    import tkinter as tk
    from tkinter import messagebox, ttk
except ImportError:                                   # pragma: no cover
    print("tkinter is unavailable; use contcfg_edit.py (CLI) instead.", file=sys.stderr)
    raise

ROOT = Path(__file__).resolve().parents[2]
DEFAULT_DIR = ROOT / "original"
LIVE_TABLE = 0x007E95C0          # DAT_007e95c0, 4 records, stride 0x200
N_SLOTS = 4

MAPVK_VK_TO_VSC_EX = 4
_user32 = ctypes.WinDLL("user32") if sys.platform == "win32" else None


# Virtual-keys that DirectInput encodes as extended (scancode | 0x80).
# MAPVK_VK_TO_VSC_EX is DOCUMENTED to return the 0xE0 prefix in the high byte, but
# on the dev machine it does NOT -- VK_LEFT came back 0x4B, not 0xE04B, which would
# have written 0x4B where the stock default for action 9 is DIK_LEFT 0xCB. Caught by
# the round-trip test against FUN_00498510's defaults. So the extended set is listed
# explicitly and the high-byte check is kept only as a fallback.
EXTENDED_VK = {
    0x21,  # PRIOR / PageUp
    0x22,  # NEXT / PageDown
    0x23,  # END
    0x24,  # HOME
    0x25,  # LEFT
    0x26,  # UP
    0x27,  # RIGHT
    0x28,  # DOWN
    0x2C,  # SNAPSHOT / PrintScreen
    0x2D,  # INSERT
    0x2E,  # DELETE
    0x90,  # NUMLOCK
    0x6F,  # DIVIDE (numpad /)
    0xA3,  # RCONTROL
    0xA5,  # RMENU / RightAlt
}


def vk_to_dik(vk):
    """Windows virtual-key -> DirectInput DIK scancode.

    DIK codes ARE PS/2 set-1 scancodes, which is what MapVirtualKeyW returns, and
    DirectInput encodes an extended key as scancode | 0x80. Verified against the
    stock defaults in FUN_00498510: S->0x1f, X->0x2d, RETURN->0x1c, ESCAPE->0x01,
    LEFT->0xcb, RIGHT->0xcd, UP->0xc8, DOWN->0xd0.
    """
    if _user32 is None:
        return None
    sc = _user32.MapVirtualKeyW(vk, MAPVK_VK_TO_VSC_EX)
    if not sc:
        return None
    hi, lo = (sc >> 8) & 0xFF, sc & 0xFF
    if hi in (0xE0, 0xE1) or vk in EXTENDED_VK:
        return lo | 0x80
    return lo


# Display names beyond the small set contcfg_edit carries. Values are DIK.
DIK_NAMES = dict(cc.DIK_REV)
DIK_NAMES.update({
    0x0F: "TAB", 0x39: "SPACE", 0x0E: "BACK", 0x1D: "LCONTROL", 0x9D: "RCONTROL",
    0x2A: "LSHIFT", 0x36: "RSHIFT", 0x38: "LALT", 0xB8: "RALT",
    0xC7: "HOME", 0xCF: "END", 0xC9: "PGUP", 0xD1: "PGDN",
    0xD2: "INSERT", 0xD3: "DELETE",
    0x3B: "F1", 0x3C: "F2", 0x3D: "F3", 0x3E: "F4", 0x3F: "F5", 0x40: "F6",
    0x41: "F7", 0x42: "F8", 0x43: "F9", 0x44: "F10", 0x57: "F11", 0x58: "F12",
    0x0C: "MINUS", 0x0D: "EQUALS", 0x1A: "LBRACKET", 0x1B: "RBRACKET",
    0x27: "SEMICOLON", 0x28: "APOSTROPHE", 0x33: "COMMA", 0x34: "PERIOD",
    0x35: "SLASH", 0x2B: "BACKSLASH", 0x29: "GRAVE",
    0x06: "6", 0x07: "7", 0x08: "8", 0x09: "9", 0x0A: "0",
    0x16: "U", 0x17: "I", 0x18: "O", 0x19: "P", 0x24: "J", 0x25: "K", 0x26: "L",
    0x31: "N", 0x32: "M",
})


def dik_label(v):
    return DIK_NAMES.get(v, f"0x{v:02x}")


# Which actions are meaningless on a joypad record.
JOYPAD_DEAD = {9, 10, 11, 12}


def read_live_table(pid=None):
    """Read the 4 live records out of a running MASHED via Frida.

    This is the authoritative source for slot->device-name mapping, which is the
    thing that decides whether a written file is accepted or silently discarded.
    """
    import frida
    dev = frida.get_local_device()
    procs = [p for p in dev.enumerate_processes() if p.name.upper().startswith("MASHED")]
    if not procs:
        raise RuntimeError("no running MASHED.exe found")
    if pid is None:
        if len(procs) > 1:
            raise RuntimeError(
                "several MASHED processes are running: "
                + ", ".join(str(p.pid) for p in procs)
                + ". Pass --pid; this tool refuses to guess (CLAUDE.md PID hygiene).")
        pid = procs[0].pid
    sess = dev.attach(pid)
    total = N_SLOTS * cc.RECORD_SIZE
    js = ("rpc.exports.read = function () {"
          " var buf = ptr(%d).readByteArray(%d);"
          " return Array.prototype.slice.call(new Uint8Array(buf));"
          "};" % (LIVE_TABLE, total))
    scr = sess.create_script(js)
    scr.load()
    raw = bytes(scr.exports_sync.read())
    sess.detach()
    if len(raw) != total:
        raise RuntimeError(f"short read: {len(raw)} bytes, expected {total}")
    return [raw[i * cc.RECORD_SIZE:(i + 1) * cc.RECORD_SIZE] for i in range(N_SLOTS)], pid


class App(ttk.Frame):
    def __init__(self, master, cfgdir):
        super().__init__(master, padding=10)
        self.grid(sticky="nsew")
        master.columnconfigure(0, weight=1)
        master.rowconfigure(0, weight=1)
        self.cfgdir = Path(cfgdir)
        self.slot = tk.IntVar(value=0)
        self.records = [None] * N_SLOTS       # bytes or None
        self.capturing = None                 # action index awaiting a key
        self._build()
        self.load_from_disk()

    # ---------------- layout ----------------
    def _build(self):
        top = ttk.Frame(self)
        top.grid(row=0, column=0, sticky="ew")
        ttk.Label(top, text="Slot:").pack(side="left")
        self.slotbox = ttk.Combobox(top, width=44, state="readonly",
                                    values=[f"{i}: (empty)" for i in range(N_SLOTS)])
        self.slotbox.pack(side="left", padx=(4, 10))
        self.slotbox.bind("<<ComboboxSelected>>", self.on_slot)
        ttk.Button(top, text="Reload from disk", command=self.load_from_disk).pack(side="left")
        ttk.Button(top, text="Detect from running game",
                   command=self.detect_live).pack(side="left", padx=4)

        self.info = ttk.Label(self, text="", foreground="#444")
        self.info.grid(row=1, column=0, sticky="w", pady=(8, 4))

        cols = ("action", "name", "value", "bound")
        self.tree = ttk.Treeview(self, columns=cols, show="headings", height=13)
        for c, w, t in (("action", 55, "#"), ("name", 170, "action"),
                        ("value", 70, "raw"), ("bound", 210, "bound to")):
            self.tree.heading(c, text=t)
            self.tree.column(c, width=w, anchor="w")
        self.tree.grid(row=2, column=0, sticky="nsew")
        self.rowconfigure(2, weight=1)
        self.columnconfigure(0, weight=1)
        self.tree.bind("<Double-1>", lambda e: self.start_capture())

        btns = ttk.Frame(self)
        btns.grid(row=3, column=0, sticky="ew", pady=6)
        ttk.Button(btns, text="Rebind selected", command=self.start_capture).pack(side="left")
        ttk.Button(btns, text="Restore stock defaults",
                   command=self.restore_defaults).pack(side="left", padx=4)
        ttk.Button(btns, text="Save to file", command=self.save).pack(side="right")

        self.status = ttk.Label(self, text="", foreground="#065")
        self.status.grid(row=4, column=0, sticky="w")
        self.warn = ttk.Label(self, text="", foreground="#a30", wraplength=560,
                              justify="left")
        self.warn.grid(row=5, column=0, sticky="w", pady=(4, 0))

    # ---------------- data ----------------
    def load_from_disk(self):
        found = 0
        for i in range(N_SLOTS):
            p = self.cfgdir / f"contcfg{i}.bin"
            try:
                data = p.read_bytes()
                if len(data) == cc.RECORD_SIZE:
                    self.records[i] = data
                    found += 1
                else:
                    self.records[i] = None
            except OSError:
                self.records[i] = None
        self.refresh_slots()
        if found == 0:
            self.set_status(
                f"No contcfg*.bin in {self.cfgdir}. That is the stock state -- the game "
                "then uses FUN_00498510's hardcoded defaults. Use 'Detect from running "
                "game' to get the correct device names before writing.")
        else:
            self.set_status(f"Loaded {found} record(s) from {self.cfgdir}.")

    def detect_live(self):
        try:
            recs, pid = read_live_table()
        except Exception as e:                        # noqa: BLE001
            messagebox.showerror("Detect failed", str(e))
            return
        self.records = list(recs)
        self.refresh_slots()
        self.set_status(f"Read the live table from pid {pid}. Device names are now "
                        "authoritative -- a record saved with these will be accepted.")

    def refresh_slots(self):
        labels = []
        for i, data in enumerate(self.records):
            if data is None:
                labels.append(f"{i}: (empty)")
                continue
            r = cc.parse(data)
            t = {0: "inactive", 1: "joypad", 2: "keyboard"}.get(r["devtype"], "?")
            nm = r["name"].strip() or "<no name>"
            labels.append(f"{i}: {t} — {nm}")
        self.slotbox["values"] = labels
        self.slotbox.current(self.slot.get())
        self.refresh_rows()

    def cur(self):
        return self.records[self.slot.get()]

    def on_slot(self, _evt=None):
        self.slot.set(self.slotbox.current())
        self.refresh_rows()

    def refresh_rows(self):
        self.tree.delete(*self.tree.get_children())
        data = self.cur()
        if data is None:
            self.info.config(text="Slot is empty. 'Restore stock defaults' creates a record.")
            self.warn.config(text="")
            return
        r = cc.parse(data)
        t = r["devtype"]
        tname = {0: "inactive", 1: "joypad", 2: "keyboard"}.get(t, f"?({t})")
        self.info.config(
            text=f"type {t} ({tname})   joypad index {r['joyidx']}   "
                 f"name {r['name']!r}   const +0x000 = {r['const']}"
                 + ("" if r["const"] == 6 else "  <-- must be 6 or the load is discarded"))
        for i, v in enumerate(r["bindings"]):
            if t == cc.DEV_KEYBOARD:
                bound = dik_label(v)
            elif t == cc.DEV_JOYPAD:
                bound = "(ignored — axis comes from a hardcoded table)" \
                    if i in JOYPAD_DEAD else f"button {v}"
            else:
                bound = str(v)
            self.tree.insert("", "end", iid=str(i),
                             values=(i, cc.ACTIONS[i], f"0x{v:02x}", bound))
        msgs = []
        if t == cc.DEV_JOYPAD:
            msgs.append("Joypad record: actions 9-12 are NOT rebindable. FUN_00497310 "
                        "ignores their stored value and reads the axis from a fixed table "
                        "at 0x0077311c / 0x00773120.")
        if not r["name"].strip():
            msgs.append("This record has NO device name. FUN_00498510 compares the name "
                        "against the enumerated device and silently replaces the record "
                        "with defaults when it differs, so an empty name will look like "
                        "the file was ignored.")
        self.warn.config(text="  ".join(msgs))

    # ---------------- editing ----------------
    def selected_action(self):
        sel = self.tree.selection()
        return int(sel[0]) if sel else None

    def start_capture(self):
        data = self.cur()
        if data is None:
            return
        a = self.selected_action()
        if a is None:
            self.set_status("Select an action row first.")
            return
        r = cc.parse(data)
        if r["devtype"] == cc.DEV_JOYPAD:
            if a in JOYPAD_DEAD:
                messagebox.showinfo(
                    "Not rebindable",
                    f"Action {a} ({cc.ACTIONS[a]}) is an axis. On a joypad record the "
                    "stored value is ignored entirely -- the axis is read from a "
                    "hardcoded table. Rebinding it here would silently do nothing, so "
                    "the UI will not pretend otherwise.")
                return
            self.ask_button(a)
            return
        self.capturing = a
        self.set_status(f"Press a key to bind to '{cc.ACTIONS[a]}'  (Esc cancels the capture)")
        self.winfo_toplevel().bind("<Key>", self.on_key)
        self.tree.focus_set()

    def on_key(self, event):
        if self.capturing is None:
            return
        a, self.capturing = self.capturing, None
        self.winfo_toplevel().unbind("<Key>")
        if event.keysym == "Escape" and event.keycode == 27:
            self.set_status("Capture cancelled — nothing changed.")
            return
        dik = vk_to_dik(event.keycode)
        if not dik:
            self.set_status(f"Could not map that key (vk={event.keycode}); nothing changed.")
            return
        self.set_binding(a, dik)
        self.set_status(f"action {a} ({cc.ACTIONS[a]}) -> {dik_label(dik)} (0x{dik:02x}). "
                        "Not saved yet.")

    def ask_button(self, a):
        win = tk.Toplevel(self)
        win.title(f"Button for '{cc.ACTIONS[a]}'")
        ttk.Label(win, text="Joypad button index (bit index into the per-pad\n"
                            "button bitmap at DAT_007730d4):").pack(padx=10, pady=8)
        var = tk.StringVar(value="0")
        ttk.Spinbox(win, from_=0, to=31, textvariable=var, width=6).pack(pady=4)

        def ok():
            try:
                v = int(var.get())
            except ValueError:
                return
            self.set_binding(a, v)
            self.set_status(f"action {a} ({cc.ACTIONS[a]}) -> button {v}. Not saved yet.")
            win.destroy()
        ttk.Button(win, text="OK", command=ok).pack(pady=8)
        win.transient(self.winfo_toplevel())
        win.grab_set()

    def set_binding(self, action, value):
        import struct
        data = bytearray(self.cur())
        struct.pack_into("<I", data, cc.OFF_BINDINGS + action * 4, value & 0xFFFFFFFF)
        self.records[self.slot.get()] = bytes(data)
        self.refresh_rows()
        self.tree.selection_set(str(action))

    def restore_defaults(self):
        i = self.slot.get()
        old = self.records[i]
        name, joyidx, devtype = "", 0, cc.DEV_KEYBOARD
        if old is not None:
            r = cc.parse(old)
            name, joyidx = r["name"], r["joyidx"]
            devtype = r["devtype"] if r["devtype"] in (cc.DEV_JOYPAD, cc.DEV_KEYBOARD) \
                else cc.DEV_KEYBOARD
        self.records[i] = cc.build(devtype=devtype, name=name, joyidx=joyidx)
        self.refresh_slots()
        self.set_status(f"Slot {i} reset to stock defaults (device name preserved).")

    def save(self):
        i = self.slot.get()
        data = self.cur()
        if data is None:
            return
        r = cc.parse(data)
        if not r["name"].strip():
            if not messagebox.askyesno(
                    "No device name",
                    "This record has an empty device name.\n\n"
                    "FUN_00498510 accepts a loaded record only when the name matches the "
                    "enumerated device; otherwise it silently overwrites it with defaults. "
                    "Saving this will most likely look like the file is being ignored.\n\n"
                    "Save anyway?"):
                return
        if r["devtype"] == cc.DEV_INACTIVE:
            if not messagebox.askyesno(
                    "Inactive slot",
                    "Device type is 0 (inactive). The game will not read bindings from "
                    "this slot.\n\nSave anyway?"):
                return
        out = self.cfgdir / f"contcfg{i}.bin"
        try:
            self.cfgdir.mkdir(parents=True, exist_ok=True)
            out.write_bytes(data)
        except OSError as e:
            messagebox.showerror("Save failed", str(e))
            return
        self.set_status(f"Wrote {out} ({len(data)} bytes). Verify with "
                        f"re/frida/verify_rebind.py before trusting it.")

    def set_status(self, msg):
        self.status.config(text=msg)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--dir", default=str(DEFAULT_DIR),
                    help="directory holding contcfg<slot>.bin (default: original/)")
    ap.add_argument("--detect", action="store_true",
                    help="headless: print the live slot table from a running MASHED and exit")
    ap.add_argument("--pid", type=int, default=None,
                    help="explicit MASHED pid for --detect (never guessed when several run)")
    a = ap.parse_args()

    if a.detect:
        recs, pid = read_live_table(a.pid)
        print("live controller table from pid", pid, "(DAT_007e95c0, 4 x 0x200)")
        print("")
        for i, data in enumerate(recs):
            r = cc.parse(data)
            t = {0: "inactive", 1: "joypad", 2: "keyboard"}.get(
                r["devtype"], "?(%d)" % r["devtype"])
            print("  slot %d: type %d (%-8s) joyidx %d name %r"
                  % (i, r["devtype"], t, r["joyidx"], r["name"]))
        print("")
        print("The NAME is load-bearing: FUN_00498510 discards a record whose name")
        print("does not match the enumerated device, silently and with no error.")
        return 0
    root = tk.Tk()
    root.title("Mashed — controller rebinding")
    root.geometry("620x520")
    App(root, a.dir)
    root.mainloop()
    return 0


if __name__ == "__main__":
    sys.exit(main())
