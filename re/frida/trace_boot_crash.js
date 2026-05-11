// Frida tracer: instrument the candidate functions in the boot path around
// the FONT36.PIZ open/search/close to identify where execution crashes.
//
// For each candidate: Interceptor.attach with onEnter+onLeave loggers.
// The Python harness watches the message stream and reports the call
// sequence; the last `enter` without a matching `leave` is where the
// process died.
'use strict';

// Candidates: every function between piz-open and piz-close, plus the
// close itself and its inner callees.
const CANDIDATES = [
    { name: 'FUN_00495280_open_piz',          rva: 0x00495280 },
    { name: 'FUN_004283a0_load_splash_bmps',  rva: 0x004283a0 },
    { name: 'FUN_00427ca0_render_state_init', rva: 0x00427ca0 },
    { name: 'FUN_0042a6b0_search_load_tex',   rva: 0x0042a6b0 },
    { name: 'FUN_0042a530_search_piz',        rva: 0x0042a530 },
    { name: 'FUN_004b3d80_txd_load_wrap',     rva: 0x004b3d80 },
    { name: 'FUN_004cc230_open_file_chunk',   rva: 0x004cc230 },
    { name: 'FUN_004cc5e0_read_chunk_header', rva: 0x004cc5e0 },
    { name: 'FUN_0054f8d0_txd_parse',         rva: 0x0054f8d0 },
    { name: 'FUN_004cc160_close_file_chunk',  rva: 0x004cc160 },
    { name: 'FUN_00556cc0_apply_tex',         rva: 0x00556cc0 },
    { name: 'FUN_004275d0_render_apply',      rva: 0x004275d0 },
    { name: 'FUN_004274d0_lang_set',          rva: 0x004274d0 },
    { name: 'FUN_004274e0_lang_load_dat',     rva: 0x004274e0 },
    { name: 'FUN_004952f0_close_piz',         rva: 0x004952f0 },
    { name: 'FUN_004b65c0_get_piz_name',      rva: 0x004b65c0 },
    { name: 'FUN_004b67a0_close_piz_inner',   rva: 0x004b67a0 },
    { name: 'FUN_00402b70_path_normalize',    rva: 0x00402b70 },
    { name: 'FUN_004b6570_piz_open_inner',    rva: 0x004b6570 },
];

let seqCounter = 0;

function attachAll() {
    for (let i = 0; i < CANDIDATES.length; i++) {
        const c = CANDIDATES[i];
        try {
            Interceptor.attach(ptr(c.rva), {
                onEnter: function () {
                    send({ kind: 'enter', name: c.name, seq: seqCounter++ });
                },
                onLeave: function () {
                    send({ kind: 'leave', name: c.name, seq: seqCounter++ });
                }
            });
        } catch (e) {
            send({ kind: 'attach_fail', name: c.name, err: e.message });
        }
    }
    send({ kind: 'ready', count: CANDIDATES.length });
}

attachAll();
