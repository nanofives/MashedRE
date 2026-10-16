// Which MSVC inline-asm spellings of "load the dword AT 0x00771A0C" assemble to 8B 35 0C 1A 77 00 (memory) and which
// to BE 0C 1A 77 00 (mov esi, imm32)?  Each function is a separate naked stub; bytes are read back from the /FAcs listing.
#define NK extern "C" __declspec(naked) void
NK v_hex_suffix_bare() {         // spelling used in PromoLoop_round256.cpp
    __asm mov esi, dword ptr [771A0Ch]
    __asm ret
}
NK v_hex_prefix_bare() {
    __asm mov esi, dword ptr [0x771A0C]
    __asm ret
}
NK v_ds_override() {
    __asm mov esi, dword ptr ds:[771A0Ch]
    __asm ret
}
NK v_ds_override_0x() {
    __asm mov esi, dword ptr ds:[0x771A0C]
    __asm ret
}
NK v_no_ptr_ds() {
    __asm mov esi, ds:[771A0Ch]
    __asm ret
}
NK v_offset_plain() {            // control: immediate
    __asm mov esi, 771A0Ch
    __asm ret
}
NK v_via_register() {            // the house pattern: load the address into a register, then dereference it
    __asm mov ecx, 771A0Ch
    __asm mov esi, dword ptr [ecx]
    __asm ret
}
