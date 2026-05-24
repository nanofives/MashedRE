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
    # Walk up: worktree is at <main>/.worktrees/<name>/
    parent = script_root.parent.parent
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
LOG_DIR    = ROOT / 'log'

results_received = []
errors_received  = []
done_flag        = {'done': False}


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


def main():
    if len(sys.argv) < 2:
        sys.exit(f"usage: {sys.argv[0]} <hook_name>\n  registered: {', '.join(HOOKS.keys())}")
    name = sys.argv[1]
    if name not in HOOKS:
        sys.exit(f"unknown hook {name!r}; registered: {', '.join(HOOKS.keys())}")

    hook = HOOKS[name]
    config = {
        'asi_path':       str(ASI_PATH).replace('\\', '\\\\'),
        'target_rva':     f"0x{hook['rva']:08x}",
        'export':         hook['export'],
        'signature':      hook['signature'],
        'arg_type':       hook['arg_type'],
        'lut_root_delta': hook['lut_root_delta'],
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
    proc = subprocess.Popen([str(MASHED_EXE)], cwd=str(MASHED_EXE.parent), env=env)
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
        print("\nGREEN: every test value produced bit-identical output.")
        return 0
    print("\nRED: at least one mismatch.")
    for r in results_received:
        if not r['match']:
            print(f"  idx={r['idx']}  input={r['input']}  orig={r['original']}  reimpl={r['reimpl']}")
    return 1


if __name__ == '__main__':
    sys.exit(main())
