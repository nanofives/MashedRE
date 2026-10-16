"""Remove the hooks_registry.py entries (and their own one-line header comment) for the exports that the
dupinstall lane deleted, plus the negctl fixture rows that would otherwise re-create them as orphan HOOKS
entries (hooks_registry._apply_negctl_fixtures adds any fixture key that is not already in HOOKS).

    py -3.12 prune_registry.py            # dry run, prints what would go
    py -3.12 prune_registry.py --apply
"""
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent.parent
REG = ROOT / "re" / "frida" / "hooks_registry.py"
KEYS = {
    "find_copter_by_key_index": "FindCopterByKeyIndex",
    "selected_copter_field60": "GetSelectedCopterField60",
    "table_8a9648_get": "Table8a9648Get",
    "get_led_entry_array_base": "GetLedEntryArrayBase",
    "frontend_slot_table_ptr_426cb0": "FrontendSlotTablePtr426cb0",
    "menu_quad_global_zero": "MenuQuadGlobalZero",
    "menu_readiness_check": "MenuReadinessCheck",
    "hud_rect_emitter": "HudRectEmitter",
    "create_dinput_object_bool": "CreateDInputObjectBool",
    "timer_table_773030_get": "TimerTable773030Get",
    "display_get_cursor_gate": "DisplayGetCursorGate",
    "font_atlas_slot_set": "FontAtlasSlotSet",
    "font_atlas_slot_get": "FontAtlasSlotGet",
    "audio_list_header_init": "AudioListHeaderInit",
}


def main() -> int:
    apply = "--apply" in sys.argv
    raw = REG.read_bytes().decode("utf-8")
    lines = raw.splitlines(keepends=True)
    drop = set()
    for key, export in KEYS.items():
        pat = re.compile(r"^    '" + re.escape(key) + r"': \{")
        idx = [i for i, l in enumerate(lines) if pat.match(l)]
        assert len(idx) == 1, (key, idx)
        i = idx[0]
        body = lines[i]
        j = i
        if not body.rstrip().endswith("},"):
            while not lines[j].startswith("    },"):
                j += 1
        text = "".join(lines[i:j + 1])
        assert f"'export': " in text.replace("'export':        ", "'export': ").replace("'export':         ", "'export': ") or "export" in text
        assert export in text, (key, export)
        drop.update(range(i, j + 1))
        # the single comment line directly above, only if it names this export or key
        k = i - 1
        while k >= 0 and lines[k].lstrip().startswith("#") and (export in lines[k] or key in lines[k]):
            drop.add(k)
            k -= 1
        print(f"{key:34s} lines {i+1}-{j+1}  (+{i-k-1} comment line(s))")
    out = "".join(l for n, l in enumerate(lines) if n not in drop)
    if apply:
        REG.write_bytes(out.encode("utf-8"))
        print("applied: registry entries removed =", len(KEYS))
    else:
        print("dry run; lines that would go:", len(drop))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
