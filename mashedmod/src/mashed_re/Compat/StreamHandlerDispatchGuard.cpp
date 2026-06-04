// Mashed RE — Compat: garbage-handler guard for the RW stream chunk-handler
// tail-dispatch at 0x005ab148  (JMP dword ptr [EAX+0x44]).
//
// 2026-06-01 DEV WORKAROUND. After the FUN_004277a0 NULL-out guard (see
// IntroTextNullGuard.cpp) cleared the ~90 s title/intro-text crash, the boot
// crash MOVED to a near-null code-pointer transfer: AV with eip=0x44, esi=0,
// eax=edi=<chunk-state-obj>, ecx=0x80a (log/crash_eip.txt). Crash-site recovery
// (caller chain boot 0x4028e0 -> FUN_004669b0 -> FUN_0045d460 [RW chunk 0x809
// loader] -> FUN_005a7b60) and a whole-image instruction search pinned the
// faulting instruction uniquely to:
//
//     0x005ab148   JMP dword ptr [EAX+0x44]
//
// This is the state==1 tail-dispatch inside the RW stream chunk-handler state
// machine (entry FUN_005ab100: switch on *(node+0x40); advance via *(node+0x48);
// tail-jmp the node handler *(node+0x44)). In the modded boot one node reaches
// this dispatch with *(EAX+0x44) == 0x44 (garbage — a handler that was never
// validly registered on this config), and the original has no validity check,
// so it jmps to 0x44 -> AV. This reproduces with NO .asi loaded (no-`.asi`
// control), i.e. a STOCK / modded-boot bug, not a port regression — but it
// blocks reaching the menu for runtime verification.
//
// Why patch the instruction (not the FUN_005ab100 entry): statically 0x5ab148
// is only reachable through FUN_005ab100, but hooking that entry (verified
// installed) did NOT stop the crash — the dispatch is reached at runtime via a
// COPIED handler pointer (no static ref / immediate to 0x5ab130/0x5ab148 exists
// in the image), bypassing the entry. Guarding the dispatch instruction itself
// catches every arrival path.
//
// Mechanism: the HookSystem 5-byte inline-JMP overwrites 0x5ab148..0x5ab14c
// (orig `FF 60 44 90 90` — the 3-byte JMP plus 2 of the 5 alignment NOPs that
// pad to 0x5ab150), redirecting into this stub. On entry EAX still holds the
// node (set up by 0x5ab130..0x5ab144 immediately before). The stub compares the
// handler in place (no register clobber — only EFLAGS) against MASHED's .text
// range [0x00401000, 0x00995000); if valid it performs the original tail-jmp
// byte-for-byte, otherwise it returns, aborting that one chunk so boot survives.
// For every legitimately-registered node the handler is a real MASHED code
// pointer, so behavior is unchanged; only the garbage case is diverted.
//
// Dev-only compat hook (mashed_re_dev.asi). NOT for the shipping greenfield exe.

#include "../Core/HookSystem.h"

// 0x005ab148 — dev-only guard for `JMP dword ptr [EAX+0x44]`. Reached with EAX =
// chunk-state node; non-standard (mid-function) entry, so naked + no prologue.
extern "C" __declspec(dllexport) __declspec(naked)
void __cdecl FUN_005ab148_DispatchGuard()
{
    __asm {
        cmp  dword ptr [eax+0x44], 0x00401000   // handler below .text?
        jb   skip
        cmp  dword ptr [eax+0x44], 0x00995000   // handler at/above image end?
        jae  skip
        jmp  dword ptr [eax+0x44]               // valid → orig 0x5ab148 tail-jmp
skip:
        ret                                     // garbage handler → abort chunk
    }
}

RH_ScopedInstall(FUN_005ab148_DispatchGuard, 0x005ab148);
