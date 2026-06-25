# Generic Frida A/B diff harness for any registered C3+ hook.
#
# Usage:  py -3.12 re/frida/run_diff.py <hook_name>
#   e.g.: py -3.12 re/frida/run_diff.py vec3_magnitude
#   e.g.: py -3.12 re/frida/run_diff.py fast_sqrt
#
# Reads hook config from re/frida/hooks_registry.py. To verify a new hook,
# add an entry to that registry — no new harness code required.
import csv
import json
import os
import shutil
import struct
import subprocess
import sys
import time
from pathlib import Path

import frida

_SCRIPT_ROOT = Path(__file__).resolve().parent.parent.parent

# When running from a worktree (.worktrees/<name>/), original/ is in the
# main repo root (two levels up from .worktrees/).
def _find_original(script_root: Path) -> Path:
    candidate = script_root / 'original' / 'MASHED.exe'
    if candidate.exists():
        return candidate
    # Walk up: worktree may be at <main>/.worktrees/<name>/ OR
    # <main>/.claude/worktrees/<name>/ (varying depth). Search ancestors.
    for parent in script_root.parents:
        candidate2 = parent / 'original' / 'MASHED.exe'
        if candidate2.exists():
            return candidate2
    return candidate  # let the later check produce a clear error

ROOT = _SCRIPT_ROOT

sys.path.insert(0, str(ROOT / 're' / 'frida'))
from hooks_registry import HOOKS

MASHED_EXE = _find_original(ROOT)
ASI_PATH   = ROOT / 'mashedmod' / 'build' / 'mashed_re_dev.asi'
AGENT_JS   = ROOT / 're' / 'frida' / 'diff_template.js'
NAV_AGENT_JS = ROOT / 're' / 'frida' / 'nav_agent.js'
LOG_DIR    = ROOT / 'log'

results_received = []
errors_received  = []
done_flag        = {'done': False}


def drive_to_race(session):
    """scenario:'race' support (2026-06-12, re/analysis/scenario_attach_lane.md):
    load the shared nav agent and drive the freshly spawned ORIGINAL into a
    Quick Battle race (race_refs.py recipe, fresh-save cursor layout) so the
    diff vectors run against populated live state instead of menu-attach
    zeros. Returns True when phase leaves the menu value (3). The nav agent's
    FUN_00497310 interceptor stays attached afterwards but is inert (override
    only fires inside a press window)."""
    nav = session.create_script(NAV_AGENT_JS.read_text(encoding='utf-8'))
    nav.on('message', lambda m, d: None)
    nav.load()
    nav.exports_sync.init()
    E = nav.exports_sync

    def wait(pred, timeout, what):
        end = time.time() + timeout
        while time.time() < end:
            if pred():
                return True
            time.sleep(0.1)
        print(f"  [nav] timeout: {what} depth={E.depth()} phase={E.phase()}")
        return False

    def press(c, ms=180):
        E.press(c, ms)
        time.sleep(ms / 1000.0 + 0.3)

    def confirm_to(target, tries=6):
        for _ in range(tries):
            if E.depth() >= target:
                return True
            press(4)
            if wait(lambda: E.depth() >= target, 2.0, f"d{target}"):
                return True
        return E.depth() >= target

    print("  [nav] booting to menu...")
    if not wait(lambda: E.phase() == 3 and E.depth() >= 1, 25, 'title'):
        return False
    time.sleep(1.5)
    confirm_to(2); time.sleep(0.4); press(4); time.sleep(0.8)
    confirm_to(3)
    E.setsel(1); time.sleep(0.3)        # Quick Battle (fresh-save cursor)
    confirm_to(4, 4)
    confirm_to(5, 4)
    press(4); time.sleep(1.5)
    for _ in range(5):
        if E.phase() != 3:
            break
        press(4); time.sleep(1.5)
    in_race = E.phase() != 3
    print(f"  [nav] in race? phase={E.phase()} ({'YES' if in_race else 'NO'})")
    if in_race:
        time.sleep(4)                   # countdown + state settle
    return in_race


def on_message(message, data):
    if message['type'] == 'error':
        print('AGENT ERROR:', message.get('description'), message.get('stack'))
        errors_received.append(message)
        done_flag['done'] = True
        return
    payload = message.get('payload', {})
    kind = payload.get('type')
    if kind == 'lut_ready':
        print(f"  [agent] LUT ready  base={payload['base']}  offset={payload['offset']}  "
              f"root={payload['root']}  delta={payload['delta']}  attempts={payload['attempts']}")
    elif kind == 'asi_loaded':
        print(f"  [agent] .asi loaded  base={payload['base']}  {payload['export_name']}@{payload['reimpl_addr']}")
    elif kind == 'results':
        results_received.extend(payload['data'])
        done_flag['done'] = True
    elif kind == 'error':
        print(f"  [agent] ERROR: {payload['msg']}")
        errors_received.append(payload)
        done_flag['done'] = True
    else:
        print('  [agent]', payload)


def value_bits(v, ret_kind):
    """Return raw bit representation as u32 for CSV display. For 'float' returns
    the IEEE-754 bit pattern (so 1.0f shows 0x3f800000). For integer returns
    (uint32, int32, etc) it's just the value masked to u32. Output-buffer
    arg_types (transform_point, matrix_scale, vec2_normalize, etc.) return
    comma-separated bit strings — those get None here so the CSV bits column
    stays empty while the value column shows the full string."""
    if v is None: return None
    if ret_kind == 'float':
        try:
            return struct.unpack('<I', struct.pack('<f', float(v)))[0]
        except (ValueError, TypeError):
            return None
    try:
        return int(v) & 0xffffffff
    except (ValueError, TypeError):
        return None  # comma-separated output-buffer string
def float_bits(f):
    """Return the IEEE-754 bit pattern of a scalar float result, or None.
    For non-numeric values (e.g. packed bit strings from output-buffer arg_types)
    this returns None so the CSV shows the raw string in the value column."""
    if f is None:
        return None
    try:
        return struct.unpack('<I', struct.pack('<f', float(f)))[0]
    except (ValueError, TypeError):
        return None  # output-buffer types return a comma-separated bit string


# arg_type handlers that seed NON-ZERO state (sentinels / registry inputs)
# into the observed region before calling the target. For these, an all-zero
# post-observation still discriminates (a no-op reimpl would have left the
# sentinel), so the non-degeneracy assertion must not fire. Derived by
# re/frida/audit_seeded_argtypes.py 2026-06-12 (heuristic + manual
# reconciliation — see that script's header); re-derive after any
# diff_template.js handler change. Unknown/new arg_types default to
# OBSERVE-ONLY: worst case is an explicit degenerate_ok per hook, never a
# silent false GREEN.
SEEDED_ARG_TYPES = {
    'cache_roundtrip', 'cache_setter_observe',
    'arena_block_free_predicate', 'audio_list_insert', 'audio_list_min_select',
    'audio_list_remove', 'audio_pool_free', 'audio_sub_struct_zero',
    'contact_history', 'count_header_list_ring', 'cursor_back',
    'dsound_secondary_init', 'endian_pack', 'fmt_desc_copy', 'fmt_desc_ptr',
    'fmt_global_scan', 'fmt_key_compare', 'fmt_table_search', 'font_ctx_float2',
    'font_matrix_push', 'guid_from_tag', 'idx_out2', 'int_copy_outbuf',
    'int_outbuf4', 'int_with_out_ptr', 'matrix_scale', 'music_vol_set',
    'ptr_nonnull_check', 'ptr_ptr_entity_set', 'ptr_zero_pair', 'read_global',
    'renderer_field3c_set', 'resource_loader_4arg',
    'scalars_to_scattered_globals', 'seed_field_read_field', 'semaphore_create',
    'sentinel_array_ptr', 'slot_block_zero', 'slot_quad_set',
    'sort_dispatch_out4', 'source_loop_set', 'sprite_table_dispatch',
    'state_machine_observe', 'struct_call_observe', 'struct_three_write',
    'structptr_seeded_array', 'thiscall_field_get', 'thread_desc_init',
    'track_record_deref', 'transform_vector', 'vec2_normalize', 'vec2_ptr',
    'vec3_global_mul_observe', 'vec3_ptr', 'void_write_observe',
    'write_global_call_int0',
}


def is_trivial(v):
    """True when an observed value carries no discriminating information:
    None/empty, numeric zero, an all-zero hex/fingerprint string, or a
    list/dict whose elements are all trivial. Used by the non-degeneracy
    assertion: a run where EVERY observation on BOTH sides is trivial proves
    nothing about the reimpl (live-state inputs still zero at attach time
    produce exactly this — the batch_ah menu-attach false-GREEN class)."""
    if v is None:
        return True
    if isinstance(v, bool):
        return not v
    if isinstance(v, (int, float)):
        return v == 0
    if isinstance(v, (list, tuple)):
        return all(is_trivial(x) for x in v)
    if isinstance(v, dict):
        return all(is_trivial(x) for x in v.values())
    s = str(v).strip().lower()
    if s in ('', 'null', 'none', 'false', 'undefined'):
        return True
    # All-zero scalar, fingerprint, or packed-bit-string forms: "0", "0.0",
    # "0x00000000", "0x0,0x0,0x0", "00000000|00000000", "[0, 0.0]", ...
    if all(c in '0x:,;|[]{}"\' .\t-' for c in s):
        return True
    try:
        return float(s) == 0.0
    except ValueError:
        return False


def build_config(hook, asi_path=None):
    """Map a hooks_registry entry to the Frida agent CONFIG dict.

    Single source of truth shared by run_diff.py (one spawn per hook) and
    run_diff_warm.py (one warm spawn per slot, many hooks). asi_path defaults
    to the module-level ASI_PATH; the warm pool may pass a per-slot path.
    """
    _asi = str(asi_path if asi_path is not None else ASI_PATH).replace('\\', '\\\\')
    config = {
        'asi_path':       _asi,
        'target_rva':     f"0x{hook['rva']:08x}",
        'export':         hook['export'],
        'signature':      hook['signature'],
        'arg_type':       hook['arg_type'],
        'lut_root_delta': hook.get('lut_root_delta', 0),  # LUT-hook-only; default 0 polls the (already-built) sqrt LUT root for non-LUT hooks
        'tests':          hook['path1_tests'],
    }
    # Optional fields — only forwarded if present in the registry entry.
    if 'target_global' in hook:
        config['target_global'] = f"0x{hook['target_global']:08x}"
    if 'entity_byte_stride' in hook:
        config['entity_byte_stride'] = hook['entity_byte_stride']
    if 'calling_convention' in hook:
        config['calling_convention'] = hook['calling_convention']
    if 'orig_calling_convention' in hook:
        config['orig_calling_convention'] = hook['orig_calling_convention']
    if 'reimpl_calling_convention' in hook:
        config['reimpl_calling_convention'] = hook['reimpl_calling_convention']
    if 'alloc_tag' in hook:
        config['alloc_tag'] = hook['alloc_tag']
    if 'alloc_rva' in hook:
        config['alloc_rva_str'] = f"0x{hook['alloc_rva']:08x}"
    if 'crash_equal_ok' in hook:
        config['crash_equal_ok'] = hook['crash_equal_ok']
    # fastcall_reg — register-convention force-call (ECX[+EDX]) for __fastcall/
    # __thiscall leaves. The diff_template.js handler reads these; without
    # forwarding them here, ecx_ptr defaults off and ECX gets the raw test marker
    # (deref AVs). (Plumbing gap caught by the 0x004a1790 canary, c3_batch_ab s6.)
    if 'fastcall_nargs' in hook:
        config['fastcall_nargs'] = hook['fastcall_nargs']
    if 'fastcall_ecx_ptr' in hook:
        config['fastcall_ecx_ptr'] = hook['fastcall_ecx_ptr']
    if 'fastcall_edx_ptr' in hook:
        config['fastcall_edx_ptr'] = hook['fastcall_edx_ptr']
    if 'pool_addr' in hook:
        config['pool_addr_str'] = f"0x{hook['pool_addr']:08x}"
    if 'insert_rva' in hook:
        config['insert_rva_str'] = f"0x{hook['insert_rva']:08x}"
    # vec3_global_mul_observe — base addr + stride for the per-index vec3.
    if 'target_global_base' in hook:
        config['target_global_base'] = f"0x{hook['target_global_base']:08x}"
    if 'target_global_stride' in hook:
        config['target_global_stride'] = hook['target_global_stride']
    # draw_quad_observe — buffer addr+len for the Im2D vertex array.
    if 'vbuf_addr_str' in hook:
        config['vbuf_addr_str'] = hook['vbuf_addr_str']
    if 'vbuf_len' in hook:
        config['vbuf_len'] = hook['vbuf_len']
    # out_buf_fmt_2 — per-output-buffer size override.
    if 'out_buf_size' in hook:
        config['out_buf_size'] = hook['out_buf_size']
    # trig_text_draw — hex addr of the draw callee to Interceptor.replace.
    if 'draw_callee_rva_str' in hook:
        config['draw_callee_rva_str'] = hook['draw_callee_rva_str']
    # sprite_table_dispatch — hex addr of the callee to Interceptor.replace.
    if 'callee_rva_str' in hook:
        config['callee_rva_str'] = hook['callee_rva_str']
    # spin_angle_observe — optional spin-angle global addr override.
    if 'angle_global_str' in hook:
        config['angle_global_str'] = hook['angle_global_str']
    # ptr_ptr_entity_set — field_offset within the dereferenced struct.
    if 'field_offset' in hook:
        config['field_offset'] = hook['field_offset']
    # track_record_deref — field offset, getter flag, record global override.
    if 'is_getter' in hook:
        config['is_getter'] = hook['is_getter']
    if 'record_global_str' in hook:
        config['record_global_str'] = hook['record_global_str']
    # struct_call_observe — out-pointer count + observable descriptor list.
    if 'out_ptrs' in hook:
        config['out_ptrs'] = hook['out_ptrs']
    if 'observe' in hook:
        config['observe'] = hook['observe']
    if 'observe_ret' in hook:
        config['observe_ret'] = hook['observe_ret']
    # audio_sub_struct_zero — struct layout for sentinel + observe range.
    if 'struct_size' in hook:
        config['struct_size'] = hook['struct_size']
    if 'observe_offset' in hook:
        config['observe_offset'] = hook['observe_offset']
    if 'observe_length' in hook:
        config['observe_length'] = hook['observe_length']
    # struct_three_write — optional observe_offsets list and struct_size.
    if 'observe_offsets' in hook:
        config['observe_offsets'] = hook['observe_offsets']
    # slot_quad_set — per-slot quad-write globals.
    if 'slot_base_addr' in hook:
        config['slot_base_addr'] = hook['slot_base_addr']
    if 'slot_stride' in hook:
        config['slot_stride'] = hook['slot_stride']
    if 'slot_field_count' in hook:
        config['slot_field_count'] = hook['slot_field_count']
    # teardown_call_pair — engine-state global to NULL before each call pair.
    if 'state_global_str' in hook:
        config['state_global_str'] = hook['state_global_str']
    # large_buffer_save_restore — buffer snapshot params.
    if 'buffer_addr' in hook:
        config['buffer_addr'] = hook['buffer_addr']
    if 'buffer_size_dwords' in hook:
        config['buffer_size_dwords'] = hook['buffer_size_dwords']
    # slot_block_zero — optional sentinel value (default 0xDEADBEEF in template).
    if 'sentinel_value' in hook:
        config['sentinel_value'] = hook['sentinel_value']
    # state_machine_observe — input/output global descriptor arrays.
    if 'input_globals' in hook:
        config['input_globals'] = [
            {'addr': f"0x{g['addr']:08x}" if isinstance(g.get('addr'), int) else g['addr'],
             'type': g.get('type', 'u32')}
            for g in hook['input_globals']
        ]
    if 'output_globals' in hook:
        config['output_globals'] = [
            {'addr': f"0x{g['addr']:08x}" if isinstance(g.get('addr'), int) else g['addr'],
             'type': g.get('type', 'u32')}
            for g in hook['output_globals']
        ]
    # multi_arg_global_write — guard + contiguous output block.
    if 'guard_global' in hook:
        config['guard_global'] = f"0x{hook['guard_global']:08x}"
    if 'out_base' in hook:
        config['out_base'] = f"0x{hook['out_base']:08x}"
    if 'out_count' in hook:
        config['out_count'] = hook['out_count']
    # c3_batch_ag harness-ext (2026-06-08): forward custom keys for the 4 new
    # arg_type handlers verbatim (seed_field_read_field / structptr_seeded_array /
    # scalars_to_scattered_globals / count_header_list_ring). Addresses are already
    # hex strings in-registry, so pass them through unchanged.
    # thiscall_field_get (2026-06-08 harness/scenario-attach): synthetic struct
    # field-getter — seeds its own scratch struct, works at plain menu-attach.
    if 'field_off' in hook:
        config['field_off'] = hook['field_off']
    if 'ret_kind' in hook:
        config['ret_kind'] = hook['ret_kind']
    for _k in ('seed_off', 'read_off', 'read_size', 'read_offs', 'fold_ret',
               'idx_call_str', 'idx_arrays', 'prep_call_str', 'prep_arg_types',
               'pre_fill_byte', 'list_op', 'node_link_off', 'cmp_field_off',
               'object_size', 'init_rva_str', 'pushback_rva_str',
               # thiscall_nested_field_get (2026-06-25): nested struct-ptr getter.
               'outer_off', 'inner_off', 'struct_size', 'inner_size',
               # cache_setter_observe (2026-06-25): scattered output-global list.
               'obs_globals'):
        if _k in hook:
            config[_k] = hook[_k]
    # void_write_observe scenario-attach extension (c3-batch-sa2 s2, 2026-06-16):
    #   seed_globals — list of {addr, val} written before EACH Orig/Reimpl call so
    #     a one-shot guard / accumulator slot is reset for both sides.
    #   call_args    — fixed integer args passed to the void_write_observe call so
    #     the write address is deterministic for index-by-param_1 functions.
    if 'seed_globals' in hook:
        config['seed_globals'] = [
            {'addr': f"0x{g['addr']:08x}" if isinstance(g.get('addr'), int) else g['addr'],
             'val':  g['val']}
            for g in hook['seed_globals']
        ]
    if 'call_args' in hook:
        config['call_args'] = hook['call_args']
    return config


def main():
    if len(sys.argv) < 2:
        sys.exit(f"usage: {sys.argv[0]} <hook_name> [--allow-degenerate]\n"
                 f"  registered: {', '.join(HOOKS.keys())}")
    name = sys.argv[1]
    if name not in HOOKS:
        sys.exit(f"unknown hook {name!r}; registered: {', '.join(HOOKS.keys())}")

    hook = HOOKS[name]
    config = build_config(hook)

    LOG_DIR.mkdir(parents=True, exist_ok=True)
    csv_out = LOG_DIR / f'diff_{name}.csv'

    if not MASHED_EXE.exists():
        sys.exit(f"MASHED.exe not found at {MASHED_EXE}")
    if not ASI_PATH.exists():
        sys.exit(f"build artifact not found at {ASI_PATH} — run mashedmod\\build.bat first")
    _shim = MASHED_EXE.parent / 'd3d9.dll'
    if not _shim.exists():
        sys.exit(f"FATAL: {_shim} missing (d3d9 windowed shim). "
                 f"Run `mashedmod\\build_d3d9_shim.bat`, then retry. "
                 f"Refusing to spawn MASHED without the shim — it would go fullscreen.")

    # Pre-flight (2026-06-12): a registry arg_type with no handler in
    # diff_template.js silently falls through to a default path that can pass
    # trivially (feedback_worker_invented_arg_types; 4 such hooks found in the
    # 2026-06-12 degenerate-GREEN audit). Refuse to run an undefined arg_type.
    _at = hook.get('arg_type')
    if _at and _at not in ('none', 'void') and f"'{_at}'" not in AGENT_JS.read_text(encoding='utf-8'):
        if '--allow-unknown-argtype' in sys.argv[2:]:
            print(f"WARNING: arg_type {_at!r} has no handler in {AGENT_JS.name} "
                  f"— continuing on --allow-unknown-argtype; result is NOT "
                  f"promotion evidence.")
        else:
            sys.exit(f"FATAL: arg_type {_at!r} (hook {name!r}) has no handler in "
                     f"{AGENT_JS.name} — the diff would fall through to a default "
                     f"path and could pass without testing anything. Add the "
                     f"handler or fix the registry entry. Override (NOT for "
                     f"promotions): --allow-unknown-argtype.")

    print(f"hook: {name}  rva={config['target_rva']}  export={config['export']}")
    print(f"spawning {MASHED_EXE} via subprocess (hook BYPASSED)")
    # AppCompat shim is set in HKCU\...\AppCompatFlags\Layers — see scripts/
    # setup_mashed_compat.ps1 / README. The RUNASINVOKER token in that layer
    # suppresses the elevation that WIN98RTM/HIGHDPIAWARE would otherwise
    # trigger, so subprocess.Popen works from a non-elevated shell.
    # Always enforce windowed 800x600 before spawn so concurrent sessions don't
    # fight over fullscreen and cause flickering on the host display.
    _canonical_cfg = ROOT / 'scripts' / 'canonical' / 'videocfg_windowed.bin'
    if _canonical_cfg.exists():
        shutil.copy2(str(_canonical_cfg), str(MASHED_EXE.parent / 'videocfg.bin'))

    env = {**os.environ, 'MASHED_RE_NO_AUTO_HOOK': '1'}
    # Detach from the parent's console + give each instance its own process
    # group. Without this, a Ctrl-C / Ctrl-Break in the terminal (or the parent
    # being signaled) broadcasts a console signal to EVERY MASHED spawned in
    # this terminal's group — i.e. all pooled instances die at once. proc.kill()
    # (TerminateProcess by PID) still tears down THIS instance surgically.
    # Matches launch_with_hooks.py / diag_smoke.py.
    _CREATE_NEW_PROCESS_GROUP = 0x00000200
    _DETACHED_PROCESS         = 0x00000008
    proc = subprocess.Popen(
        [str(MASHED_EXE)], cwd=str(MASHED_EXE.parent), env=env,
        stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
        creationflags=_CREATE_NEW_PROCESS_GROUP | _DETACHED_PROCESS)
    print(f"  pid = {proc.pid}")

    time.sleep(0.2)
    device = frida.get_local_device()
    try:
        session = device.attach(proc.pid)
    except Exception as e:
        print(f"attach failed: {e}")
        try: proc.kill()
        except Exception: pass
        return 4

    if hook.get('scenario') == 'race':
        print("scenario='race': driving to a live race before running vectors")
        if not drive_to_race(session):
            print("FAILED to reach race state — aborting (no vectors run).")
            try: session.detach()
            except Exception: pass
            try: proc.kill()
            except Exception: pass
            return 6

    script_text = AGENT_JS.read_text(encoding='utf-8').replace('$CONFIG$', json.dumps(config))
    script = session.create_script(script_text)
    script.on('message', on_message)
    script.load()

    deadline = time.time() + 60
    while not done_flag['done'] and time.time() < deadline:
        time.sleep(0.1)

    try: session.detach()
    except Exception: pass
    try: proc.kill()
    except Exception: pass
    try: proc.wait(timeout=3)
    except Exception: pass

    if errors_received and not results_received:
        print("\nNO RESULTS — agent reported errors above.")
        return 2
    if not results_received:
        print("\nTIMEOUT.")
        return 3

    ret_kind = hook['signature']['ret']  # 'float', 'uint32', etc.
    mismatches = 0
    with csv_out.open('w', newline='', encoding='utf-8') as fh:
        w = csv.writer(fh)
        w.writerow(['idx', 'input',
                    'original', 'original_bits',
                    'reimpl',   'reimpl_bits',
                    'match', 'err_original', 'err_reimpl'])
        for r in results_received:
            ob, rb = value_bits(r['original'], ret_kind), value_bits(r['reimpl'], ret_kind)
            ob_s = f"0x{ob:08x}" if ob is not None else ''
            rb_s = f"0x{rb:08x}" if rb is not None else ''
            inp = r['input']
            inp_repr = json.dumps(inp) if isinstance(inp, (list, dict)) else inp
            w.writerow([r['idx'], inp_repr,
                        r['original'], ob_s,
                        r['reimpl'],   rb_s,
                        r['match'],
                        r.get('err_original') or '',
                        r.get('err_reimpl') or ''])
            if not r['match']:
                mismatches += 1

    print(f"\nResults written to {csv_out}")
    print(f"  total cases: {len(results_received)}")
    print(f"  mismatches:  {mismatches}")
    if mismatches == 0:
        # Non-degeneracy assertion (2026-06-12, batch_ah post-mortem): a clean
        # match over observations that are ALL trivial on BOTH sides has no
        # discriminating power — typical when live-state inputs are still zero
        # at attach time. Such a run is NOT C3 evidence and must not say GREEN.
        # Exception: SEEDED arg_types pre-fill the observed region with a
        # non-zero sentinel/input, so their all-zero result proves the target
        # actively wrote — that GREEN stands.
        all_trivial = all(is_trivial(r['original']) and is_trivial(r['reimpl'])
                          for r in results_received)
        allow = (hook.get('arg_type') in SEEDED_ARG_TYPES
                 or hook.get('degenerate_ok')
                 or '--allow-degenerate' in sys.argv[2:])
        if all_trivial and not allow:
            print("\nINCONCLUSIVE-DEGENERATE: 0 mismatches, but every observed value")
            print("on both sides is trivial (zero/empty/all-zero fingerprint). The")
            print("diff had no discriminating power. Re-run at a state where the")
            print("inputs are populated (scenario-attach), or — only if all-zero")
            print("output is genuinely this function's behavior on every test")
            print("vector — pass --allow-degenerate or set degenerate_ok=True in")
            print("hooks_registry.py.")
            return 5
        if all_trivial:
            why = ('seeded arg_type ' + repr(hook.get('arg_type'))
                   if hook.get('arg_type') in SEEDED_ARG_TYPES
                   else 'degenerate_ok/--allow-degenerate override')
            print(f"\nNOTE: all observations trivial; GREEN allowed by {why}.")
        print("\nGREEN: every test value produced bit-identical output.")
        return 0
    print("\nRED: at least one mismatch.")
    for r in results_received:
        if not r['match']:
            print(f"  idx={r['idx']}  input={r['input']}  orig={r['original']}  reimpl={r['reimpl']}")
    return 1


if __name__ == '__main__':
    sys.exit(main())
