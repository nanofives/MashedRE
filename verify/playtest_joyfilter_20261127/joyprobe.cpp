// PLAYTEST-JOYFILTER probe: prints the predicate's verdict for each DI8 game-controller type, then enumerates the
// live DI8DEVCLASS_GAMECTRL devices (DIEDFL_ATTACHEDONLY, as 0x00496040 does) and applies the same predicate.
// Build: cl /nologo /EHsc /I ..\..\mashedmod\src\mashed_re joyprobe.cpp dinput8.lib dxguid.lib
#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <dinput.h>
#include <cstdio>
#include "Input/JoyFilter.h"

static BOOL CALLBACK Cb(LPCDIDEVICEINSTANCEA inst, LPVOID ref) {
    ++*static_cast<int*>(ref);
    std::printf("live  \"%s\" dwDevType 0x%08lx type 0x%02lx subtype 0x%02lx -> %s\n", inst->tszProductName,
                static_cast<unsigned long>(inst->dwDevType), static_cast<unsigned long>(GET_DIDEVICE_TYPE(inst->dwDevType)),
                static_cast<unsigned long>(GET_DIDEVICE_SUBTYPE(inst->dwDevType)),
                mashed_re::Input::JoyFilter_TypeIsSteering(inst->dwDevType) ? "kept" : "skipped");
    return DIENUM_CONTINUE;
}

int main() {
    struct { const char* name; unsigned long t; } rows[] = {
        {"DI8DEVTYPE_DEVICE", DI8DEVTYPE_DEVICE},         {"DI8DEVTYPE_MOUSE", DI8DEVTYPE_MOUSE},
        {"DI8DEVTYPE_KEYBOARD", DI8DEVTYPE_KEYBOARD},     {"DI8DEVTYPE_JOYSTICK", DI8DEVTYPE_JOYSTICK},
        {"DI8DEVTYPE_GAMEPAD", DI8DEVTYPE_GAMEPAD},       {"DI8DEVTYPE_DRIVING", DI8DEVTYPE_DRIVING},
        {"DI8DEVTYPE_FLIGHT", DI8DEVTYPE_FLIGHT},         {"DI8DEVTYPE_1STPERSON", DI8DEVTYPE_1STPERSON},
        {"DI8DEVTYPE_DEVICECTRL", DI8DEVTYPE_DEVICECTRL}, {"DI8DEVTYPE_SCREENPOINTER", DI8DEVTYPE_SCREENPOINTER},
        {"DI8DEVTYPE_REMOTE", DI8DEVTYPE_REMOTE},         {"DI8DEVTYPE_SUPPLEMENTAL", DI8DEVTYPE_SUPPLEMENTAL},
        {"HID gamepad 0x00010215", 0x00010215ul},         {"HID joystick 0x00010114", 0x00010114ul},
        {"Keychron Link 0x00010318", 0x00010318ul},
    };
    for (auto& r : rows)
        std::printf("table %-26s 0x%08lx -> %s\n", r.name, r.t,
                    mashed_re::Input::JoyFilter_TypeIsSteering(r.t) ? "kept" : "skipped");
    IDirectInput8A* di = nullptr;
    HRESULT hr = DirectInput8Create(GetModuleHandleA(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8A,
                                    reinterpret_cast<void**>(&di), nullptr);
    if (FAILED(hr) || !di) { std::printf("DirectInput8Create failed 0x%08lx\n", static_cast<unsigned long>(hr)); return 1; }
    int n = 0;
    di->EnumDevices(DI8DEVCLASS_GAMECTRL, Cb, &n, DIEDFL_ATTACHEDONLY);
    std::printf("live devices enumerated: %d\n", n);
    di->Release();
    return 0;
}
