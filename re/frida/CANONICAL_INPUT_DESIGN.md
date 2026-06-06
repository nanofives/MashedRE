# Canonical-scenario input: IN-PROCESS only (never OS injection)

## Why this doc exists
The menu-navigate / in-race C3→C4 canonical scenarios need to drive MASHED's
input. The first attempt used OS-level injection (`keybd_event` / `SendInput` +
`SetForegroundWindow`) from the harness. **This is banned.** On the user's
interactive machine, Windows foreground-lock blocks a background process from
grabbing focus, so the injected keystrokes leaked into the user's *active* window
and affected the whole PC (2026-06-06 incident). The MCP `send_key_to_window`
(`use_sendinput=True`) has the identical foreground requirement; `use_sendinput=False`
(PostMessage) won't reach MASHED's DirectInput8 polling.

**Rule:** canonical scenarios drive input strictly IN-PROCESS, via Frida, touching
only memory MASHED itself reads — never the OS foreground/input queue.

## Recommended approach — patch the DirectInput keyboard state buffer
MASHED reads the keyboard through DirectInput8 immediate mode:
`IDirectInputDevice8::GetDeviceState(cbData=256, lpvData=&keystate[256])`, which
fills a 256-byte array indexed by DIK_* scancodes; a key is "down" when the high
bit (0x80) is set. The menu/game polls this each frame.

Plan (all inside `canonical_c4_verify.py`'s agent, no OS calls):
1. Resolve the keyboard device's `GetDeviceState` (vtable slot #9 on
   IDirectInputDevice8) — hook it in `dinput8_real.DLL` via Interceptor, OR hook
   MASHED's own input-poll wrapper that calls it.
2. `onLeave`: if a scripted "press window" is active, OR the desired DIK bytes
   into the returned buffer: `lpvData[DIK_DOWN]=0x80`, etc., for N frames, then
   release. Sequence Down/Up/Enter/Esc on a frame schedule.
   DIK codes: DIK_DOWN=0xD0, DIK_UP=0xC8, DIK_LEFT=0xCB, DIK_RIGHT=0xCD,
   DIK_RETURN=0x1C, DIK_ESCAPE=0x01.
3. The OFF-count (Interceptor on candidate originals) and the press-injection run
   together; the menu navigates because *MASHED's own* read sees the bits.

This never calls SetForegroundWindow / keybd_event / SendInput, so it cannot
affect anything outside the target process.

### Alternative (if the dinput8 boundary is awkward)
Find MASHED's *processed* input global (the normalized key/pad state the menu
logic reads after its own poll — the Lua joypad-remap layer suggests one exists),
and `Memory.write` the pressed bits for a frame, synced to the frame tick. Needs
RE of which global the menu reads; the dinput8-buffer patch above avoids that.

## Until this is built
- SAFE now: boot-to-menu **IDLE** scenarios (no input) — spawn + Interceptor-count
  + observe. Already banked VideoStateFlagGet (C3→C4) + 3 reconfirms + 22 demotes.
- SAFE now: **render-math** suspects (hot-path transforms) re-validate via
  install+survive (rendering obviously exercises them; their C3 is a full-domain
  leaf diff) — do NOT Interceptor-count them (>1000/s destabilizes).
- BLOCKED until in-process input: navigate getters / hud / save / in-race suspects.

See `[[feedback-no-global-input-injection]]` and the C4 harness in
`re/frida/canonical_c4_verify.py` / `canonical_install_observe.py`.
