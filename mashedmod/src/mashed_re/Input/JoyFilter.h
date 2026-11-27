#pragma once
// [PLAYTEST-JOYFILTER] Which attached DirectInput game controllers the standalone's DEFAULT path counts as joypad
// slots. DELIBERATE DEPARTURE from the original (Mariano 2026-11-27, playtest item 3), not a parity fix.
//
// The original counts every attached game controller that opens, with no check on type or name: the enumerator
// 0x00496040 calls EnumDevices(4 = DI8DEVCLASS_GAMECTRL, 0x00495ee0, &DAT_00772fac, 1 = DIEDFL_ATTACHEDONLY) and the
// callback 0x00495ee0 counts a device once its CreateDevice (0x00495f15) succeeds (increment at 0x00495fcb). Slots
// below that count become joypads and the next one the keyboard (0x00498510), and the player colour select screen
// draws one row per slot (0x004332a0). A Keychron Link keyboard receiver reports two such controllers
// (dwDevType 0x00010318) and pushed the keyboard to the third row on MARIANO-PC.
//
// Kept: the four DI8 types that are steering controllers. Values from the Windows SDK 10.0.26100.0 um/dinput.h:
//   DI8DEVTYPE_JOYSTICK 0x14 (dinput.h:408), DI8DEVTYPE_GAMEPAD 0x15 (:409),
//   DI8DEVTYPE_DRIVING  0x16 (:410),         DI8DEVTYPE_FLIGHT  0x17 (:411).
// The type is the low byte of dwDevType, GET_DIDEVICE_TYPE = LOBYTE (dinput.h:522). Skipped: everything else in
// DI8DEVCLASS_GAMECTRL, among them DI8DEVTYPE_1STPERSON 0x18 (:412, the Keychron: 0x00010318 = DIDEVTYPE_HID
// 0x00010000 (:419) | subtype 0x03 DI8DEVTYPE1STPERSON_SIXDOF (:494) | type 0x18), DI8DEVTYPE_DEVICECTRL 0x19 (:413),
// DI8DEVTYPE_SCREENPOINTER 0x1A (:414), DI8DEVTYPE_REMOTE 0x1B (:415), DI8DEVTYPE_SUPPLEMENTAL 0x1C (:416).
//
// MASHED_ALL_CONTROLLERS=1 restores the original's count-everything rule. MASHED_ORIG_TOP=1 never filters (the
// top-down path keeps the original's behaviour).
namespace mashed_re { namespace Input {

inline bool JoyFilter_TypeIsSteering(unsigned long dwDevType) {
    const unsigned type = static_cast<unsigned>(dwDevType & 0xffu);   // GET_DIDEVICE_TYPE, dinput.h:522
    return type >= 0x14u && type <= 0x17u;                            // JOYSTICK..FLIGHT, dinput.h:408-411
}

} }  // namespace mashed_re::Input
