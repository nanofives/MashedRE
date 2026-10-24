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


GAMESAVE_REF = MASHED_EXE.parent / 'gamesave.bin.refbak_20260830'

# 2026-10-11: original/gamesave.bin was found ALL ZEROS (151,456 bytes, mtime
# 2026-10-11 15:30:45; the pristine reference is gamesave.bin.refbak_20260830,
# magic 0xDEADBEEF). With a zero save the Quick Battle track-select screen
# (menu depth 5, "Challenge Select") ignores confirm, so drive_to_race never
# left the menu. The game re-opens gamesave.bin read-only ~180x during the
# menu walk via CreateFileA("C:gamesave.bin"). Pin those reads to the pristine
# reference and send any write to a per-pid temp file, so a race diff neither
# depends on nor mutates the shared save. Must load before menu depth 2 (the
# first read), i.e. right after attach.
_GAMESAVE_PIN_JS = r"""
const REF = %s, SCR = %s;
for (const nm of ['CreateFileA', 'CreateFileW']) {
  const W = nm.endsWith('W');
  const f = Process.getModuleByName('kernel32.dll').findExportByName(nm);
  Interceptor.attach(f, { onEnter(a) {
    const p = W ? a[0].readUtf16String() : a[0].readAnsiString();
    if (!p || !p.toLowerCase().endsWith('gamesave.bin')) return;
    const t = (a[1].toUInt32() === 0x80000000) ? REF : SCR;   // GENERIC_READ only
    this.buf = W ? Memory.allocUtf16String(t) : Memory.allocAnsiString(t);
    a[0] = this.buf;
  } });
}
"""


def pin_gamesave(session, pid):
    if not GAMESAVE_REF.exists():
        print(f"  [save] WARNING: {GAMESAVE_REF.name} missing; using live gamesave.bin")
        return None
    import tempfile
    scratch = Path(tempfile.gettempdir()) / f'run_diff_gamesave_{pid}.bin'
    js = _GAMESAVE_PIN_JS % (json.dumps(str(GAMESAVE_REF)), json.dumps(str(scratch)))
    scr = session.create_script(js)
    scr.on('message', lambda m, d: None)
    scr.load()
    live = MASHED_EXE.parent / 'gamesave.bin'
    try:
        drift = live.read_bytes() != GAMESAVE_REF.read_bytes()
    except OSError:
        drift = True
    print(f"  [save] reads pinned to {GAMESAVE_REF.name}"
          f"{' (live gamesave.bin DIFFERS from it)' if drift else ''}; writes -> {scratch}")
    return scr


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


def wait_race_sentinel(session, hook, timeout=30.0):
    """race_sentinel (bworld 2026-10-12): `phase != 3` only says the menu was left; the track may still be loading.
    A hook whose body reads a race-only object names that object's global here ({addr} or an int); vectors run only
    once the dword is non-zero, so a load still in progress reads as a refused run (exit 6), never as an AV on both
    arms. 0x00426dc0 names DAT_00656ee8 (0x00646e58 + 0x10090, read at 0x00479104). No key: no wait."""
    sent = hook.get('race_sentinel')
    if sent is None:
        return True
    addr = sent['addr'] if isinstance(sent, dict) else sent
    rd = session.create_script("rpc.exports = { rd: function (a) { return ptr(a).readU32() >>> 0; } };")
    rd.load()
    end = time.time() + timeout
    while time.time() < end:
        v = rd.exports_sync.rd(f"0x{addr:08x}")
        if v:
            print(f"  [race] sentinel 0x{addr:08x} = 0x{v:08x}: running vectors")
            rd.unload()
            return True
        time.sleep(0.25)
    print(f"  [race] sentinel 0x{addr:08x} still 0 after {timeout:.0f}s")
    rd.unload()
    return False


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
    'cache_roundtrip', 'cache_setter_observe', 'eax_state_machine_observe',
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
    # st0_ret_global — one writable input global (f32) seeded per test.
    if 'global_a' in hook:
        config['global_a'] = f"0x{hook['global_a']:08x}"
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
    if 'wrap_off' in hook:                      # queue_node_consume: fn(obj) with obj+wrap_off = node
        config['wrap_off'] = hook['wrap_off']
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
    # esi_row_update_rw (0x0041c410) — register-`this` selector, struct size,
    # the six callee RVAs the handler stubs, and the six globals it seeds.
    # These MUST be forwarded: build_config is a WHITELIST, so a registry entry
    # naming keys that are not listed here has them silently dropped and the
    # handler falls back to its defaults. That is a quiet failure mode — the
    # 0x0041c410 diff first went GREEN with all of these being ignored, purely
    # because the handler's defaults happened to match the entry.
    for _k in ('this_reg', 'struct_size',
               'callee_delta_str', 'callee_score_str', 'callee_max_str',
               'callee_mattrans_str', 'callee_frametf_str', 'callee_framescale_str',
               'flag_crown', 'flag_tied', 'flag_lowest', 'flag_zero',
               'row_order', 'pulse_tick'):
        if _k in hook:
            config[_k] = hook[_k]
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
    # ptr_seed_observe — multi-pointer pure-leaf differ (c3-batch-render-p2w1a-s1).
    if 'arg_layout' in hook:
        config['arg_layout'] = hook['arg_layout']
    if 'num_bufs' in hook:
        config['num_bufs'] = hook['num_bufs']
    if 'buf_size' in hook:
        config['buf_size'] = hook['buf_size']
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
               # render_state_seq_observe (area-vehicle r6): device-vtable-slot
               # RwRenderStateSet(state,value) sequence recorder.
               'device_global_str', 'slot_off', 'stub_nargs', 'stub_abi',
               # per_vehicle_record_seed (area-vehicle r4): strided per-vehicle
               # record seed+fingerprint+restore. slot_base_addr/slot_stride already
               # forwarded above; these carry the observe window + input width.
               'out_off', 'out_len', 'in_floats', 'slot_max',
               'idx_call_str', 'idx_arrays', 'prep_call_str', 'prep_arg_types',
               'pre_fill_byte', 'list_op', 'node_link_off', 'cmp_field_off',
               'object_size', 'init_rva_str', 'pushback_rva_str',
               # thiscall_nested_field_get (2026-06-25): nested struct-ptr getter.
               'outer_off', 'inner_off', 'struct_size', 'inner_size',
               # cache_setter_observe (2026-06-25): scattered output-global list.
               'obs_globals',
               # scalars_to_scattered_globals scratch buffer (negctl re-earn 2026-10-11).
               'scratch_size', 'scratch_fill', 'recorder',
               # real dialog window tree + EAX-delivered arg (reearn/settings 2026-10-12).
               'dialog', 'eax_arg',
               # named per-callee recorders + call-sequence fingerprint (followup/a2-stubat 2026-10-12).
               'call_log',
               # x87 control word loaded around each arm (drawstream 2026-10-12).
               'x87_cw',
               # raw observe windows + return in the fingerprint string (bworld 2026-10-12).
               'observe_raw'):
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
    # reg_this_callee_stub (orch-iter6): register-implicit-this ctor with live-RW
    # callees stubbed. Forward this_reg + atom_count + the callee RVAs as hex
    # strings (struct_size already forwarded above; tests come from path1_tests).
    if 'this_reg' in hook:
        config['this_reg'] = hook['this_reg']
    if 'atom_count' in hook:
        config['atom_count'] = hook['atom_count']
    for _ck in ('callee_fill', 'callee_index', 'callee_zero', 'callee_color'):
        if _ck in hook:
            config[_ck + '_str'] = f"0x{hook[_ck]:08x}"
    # reg_this_call_observe (orch-iter8): void single-call functions (e.g.
    # RpClumpDestroy destructors) — record the observed callee's first arg.
    if 'this_field_off' in hook:
        config['this_field_off'] = hook['this_field_off']
    if 'observe_callee' in hook:
        config['observe_callee_str'] = f"0x{hook['observe_callee']:08x}"
    # stub_dispatch_observe (orch-iter19): plant a recorder where the function
    # expects a callee — in a fake vtable, or passed straight as an argument.
    # observe_bufs (round 250): fold scratch-buffer bytes into the fingerprint,
    # for functions whose real output goes through an out-POINTER argument that
    # neither observe_ret nor observe_calls can see. MUST be forwarded here —
    # this builder is a whitelist, and an unforwarded key is dropped SILENTLY,
    # leaving a fingerprint that still goes GREEN while proving nothing.
    # stub_esi / stub_esi_seed (fwd494fd0 2026-10-16): record ESI at each stub_at entry (a register input the
    # stack-args recorder cannot see) and seed the global it is loaded from. Same whitelist rule.
    for _k in ('stub_nargs', 'stub_abi', 'stub_ret', 'observe_calls', 'observe_bufs', 'stub_esi', 'stub_esi_seed'):
        if _k in hook:
            config[_k] = hook[_k]
    # stub_at (orch-iter20): the same recorder, but planted at a callee reached
    # by a HARDCODED direct CALL, where there is no pointer to seed. Ints in the
    # registry, hex strings on the wire — ptr() takes either, but the string form
    # matches observe_callee_str and reads correctly in a dumped manifest.
    if 'stub_at' in hook:
        # G2 (L-INST 2026-10-11): an entry may be a dict {addr, nargs, abi,
        # ret, name}; its addr is normalised the same way, the rest passes through.
        config['stub_at'] = [dict(a, addr=(a['addr'] if isinstance(a['addr'], str)
                                           else f"0x{a['addr']:08x}"))
                             if isinstance(a, dict) else
                             (a if isinstance(a, str) else f"0x{a:08x}")
                             for a in hook['stub_at']]
    # vtable_table_dispatch (orch-iter15): table-of-8-byte-entries dispatch,
    # entry = *(holder + vtbl_ptr_offset) + idx*8.
    for _k in ('vtbl_ptr_offset', 'table_entries'):
        if _k in hook:
            config[_k] = hook[_k]
    # Heap / import stand-ins (plan item #3, 2026-10-11; re/frida/js/standins.js). Read by the generic
    # Orig/Reimpl wrapper in runDiff and by arg_type standin_object_call. Same whitelist rule as above:
    # run_verify_hook.py forwards the same keys, so path1 and path2 run the same fixture.
    for _k in STANDIN_KEYS:
        if _k in hook:
            config[_k] = hook[_k]
    return config


NEGCTL_CLASSES = ('null', 'ret_int', 'ret_float', 'field_first', 'field_last', 'heap_extra', 'import_extra',
                  'field_none')

STANDIN_KEYS = ('heap_standin', 'import_standin', 'freeze_main', 'object_size', 'object_alloc',
                'observe_ret_len', 'observe_globals', 'reg_args', 'callee_pops',
                # fake function pointers a stand-in can return (no-return imports, bnoreturn 2026-10-12).
                'standin_fakes')
STANDINS_JS = ROOT / 're' / 'frida' / 'js' / 'standins.js'


def agent_source(agent_js, config):
    """Agent text: the shared stand-in library, then the template with CONFIG substituted."""
    return (STANDINS_JS.read_text(encoding='utf-8') + '\n'
            + agent_js.read_text(encoding='utf-8').replace('$CONFIG$', json.dumps(config)))


def _split_call_log(v):
    """A call_log arm value is '<fnv hex> <call;call;..>' (diff_template.js snapFp); return the calls."""
    if not isinstance(v, str) or ' ' not in v:
        return None
    body = v.split(' ', 1)[1]
    return body.split(';') if body else []


def _write_drawstream(csv_out, results):
    """Draw-stream view of a call_log run (drawstream 2026-10-12): one row per recorded call per vector, both arms
    side by side, so a RED names the first call that differs instead of two opaque fingerprints. Written next to the
    diff CSV as drawstream_<rest of its name>; prints the first divergent call of every mismatching vector."""
    out = csv_out.with_name('drawstream_' + (csv_out.name[len('diff_'):] if csv_out.name.startswith('diff_')
                                             else csv_out.name))
    firsts = []
    with out.open('w', newline='', encoding='utf-8') as fh:
        w = csv.writer(fh)
        w.writerow(['idx', 'call', 'original', 'reimpl', 'match'])
        for r in results:
            o, m = _split_call_log(r.get('original')), _split_call_log(r.get('reimpl'))
            if o is None and m is None:
                w.writerow([r['idx'], '', r.get('err_original') or '', r.get('err_reimpl') or '', False])
                continue
            o, m = o or [], m or []
            first = None
            for k in range(max(len(o), len(m))):
                a = o[k] if k < len(o) else ''
                b = m[k] if k < len(m) else ''
                w.writerow([r['idx'], k, a, b, a == b])
                if a != b and first is None:
                    first = (k, a, b)
            if first is not None:
                firsts.append((r['idx'], len(o), len(m)) + first)
    print(f"  draw stream written to {out}")
    for idx, no, nm, k, a, b in firsts:
        print(f"  idx={idx}  calls orig={no} reimpl={nm}  first divergence at call {k}:")
        print(f"      orig:   {a or '(none)'}")
        print(f"      reimpl: {b or '(none)'}")


def main():
    if len(sys.argv) < 2:
        sys.exit(f"usage: {sys.argv[0]} <hook_name> [--allow-degenerate]\n"
                 f"  registered: {', '.join(HOOKS.keys())}")
    name = sys.argv[1]
    if name not in HOOKS:
        sys.exit(f"unknown hook {name!r}; registered: {', '.join(HOOKS.keys())}")

    hook = HOOKS[name]
    config = build_config(hook)
    # --self-baseline: bind the reimpl arm to the ORIGINAL (zero-hook control). A fixture that cannot pass
    # original-vs-original is a broken verifier. Writes its own CSV so it never overwrites the real diff.
    self_baseline = '--self-baseline' in sys.argv[2:]
    if self_baseline:
        config['self_baseline'] = True

    # --negctl <class>: run the reimpl arm through the dev .asi's mutation forwarder (Harness/NegCtl.cpp) so the
    # handler is shown able to read RED. Classes: null ret_int ret_float field_first field_last heap_extra
    # import_extra. Driven by run_negctl.py; a negctl CSV is NEVER promotion evidence and never overwrites one.
    negctl = None
    if '--negctl' in sys.argv[2:]:
        _i = sys.argv.index('--negctl')
        negctl = sys.argv[_i + 1] if _i + 1 < len(sys.argv) else None
        if negctl not in NEGCTL_CLASSES:
            sys.exit(f"--negctl needs one of {', '.join(NEGCTL_CLASSES)}")
        if self_baseline:
            sys.exit("--negctl and --self-baseline are mutually exclusive")
        config['negctl'] = negctl

    # --x87-cw=0xNNNN (drawstream 2026-10-12): run both arms at that x87 control word (diff_template.js `x87_cw`),
    # e.g. 0x7f, the PC_24 word the game's thread runs at. Overrides a fixture's own x87_cw; the CSV gets _cw<hex>.
    cw_tag = ''
    for _a in sys.argv[2:]:
        if _a.startswith('--x87-cw='):
            config['x87_cw'] = int(_a.split('=', 1)[1], 0) & 0xffff
            cw_tag = f"_cw{config['x87_cw']:x}"

    LOG_DIR.mkdir(parents=True, exist_ok=True)
    csv_out = LOG_DIR / (f'diff_{name}_selfbaseline{cw_tag}.csv' if self_baseline
                         else f'diff_{name}_negctl_{negctl}{cw_tag}.csv' if negctl else f'diff_{name}{cw_tag}.csv')

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
    if negctl:
        env['MASHED_NEGCTL'] = '1'      # NegCtl_Arm refuses without it
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
        _save_pin = pin_gamesave(session, proc.pid)   # keep a reference alive
        if not drive_to_race(session) or not wait_race_sentinel(session, hook):
            print("FAILED to reach race state — aborting (no vectors run).")
            try: session.detach()
            except Exception: pass
            try: proc.kill()
            except Exception: pass
            return 6

    # boot_settle_s (2026-10-12): wait this long after attach before loading the agent. Frontend renderers
    # (0x0042fe90 & co) need the menu sprite dictionaries, which load after LUT-ready; without the wait every
    # drawing vector AVs at 0x8 on BOTH arms. run_negctl's warm lane already waits 6 s after spawn.
    if hook.get('boot_settle_s'):
        print(f"  boot_settle_s={hook['boot_settle_s']}: waiting before loading the agent")
        time.sleep(float(hook['boot_settle_s']))

    script_text = agent_source(AGENT_JS, config)
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
    if hook.get('call_log'):
        _write_drawstream(csv_out, results_received)
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
