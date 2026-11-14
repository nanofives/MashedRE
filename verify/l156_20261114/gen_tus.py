"""L156-REGCONV-EVIDENCE: generate the VS2022 (vc19) translation units for the five register-convention units.

    py -3.12 verify/l156_20261114/gen_tus.py

Writes verify/l156_20261114/tu/:
  single_<rva>.cpp  the unit source alone, compiled as the port compiles it (C++, MATCH_PORT, extern "C"), every
                    callee left as its FUN_ name (= the original's address). Input for re/tools/match/equiv.py.
  unit_<rva>.cpp    the unit plus the VS2022 bodies the exe binds for its register-convention helpers
                    (re/match/s06/00427680.c, 004277a0.c, 00427840.c, renamed Port_<rva> so they are not mapped to
                    the original's address), or for 0x00458920 the exe's register adapter
                    (Match/MatchS09_00458920.cpp MatchS09_00458920_Call458570) calling the original 0x00458570.
                    Input for unit_cmp.py.
Flags: the exe build compiles the Match TUs with /EHa /W3 /O2 /DMASHED_STANDALONE plus /arch:IA32 (mashedmod/x87_tus.txt
lines 148, 162-165). The vc19 toolchain in match.py adds /GS- /arch:IA32. /GS- differs from the exe build (default
/GS): the security cookie check is left out here (it adds a cookie store and a __security_check_cookie call).
"""
import pathlib

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parent.parent
TU = HERE / "tu"
FLAGS = "// FLAGS: /O2 /EHa /DMASHED_STANDALONE"

UNITS = {
    0x00427e00: ("s06", ["00427680", "004277a0"]),
    0x00427f00: ("s06", ["00427680", "00427840"]),
    0x00427ff0: ("s06", ["00427680", "004277a0"]),
    0x00428140: ("s06", ["00427680", "004277a0"]),
    0x00458920: ("s09", []),
}

ADAPTER_458570 = r"""
// The exe's adapter (mashedmod/src/mashed_re/Match/MatchS09_00458920.cpp:40), with `call PK_458570_w` replaced by a
// call to the original 0x00458570 (EDX holds the target; the original helper's live-in registers are EAX and ESI,
// verify/ddkregconv7_20261113/RESULT_DDKOBJ-REGCONV-FALLBACK-7.md section 1).
extern "C" __declspec(naked) float Port_00458570(V3_00458920* /*out*/, int /*idx*/, int* /*rec*/) {
    __asm {
        push ebp
        push ebx
        push edi
        push esi
        mov  esi, dword ptr [esp + 20]
        mov  eax, dword ptr [esp + 24]
        push dword ptr [esp + 28]
        mov  edx, 0x00458570
        call edx
        add  esp, 4
        pop  esi
        pop  edi
        pop  ebx
        pop  ebp
        ret
    }
}
"""


def src(bucket, rva_hex):
    return (ROOT / "re" / "match" / bucket / f"{rva_hex}.c").as_posix()


def main():
    TU.mkdir(exist_ok=True)
    for rva, (bucket, helpers) in UNITS.items():
        h = f"{rva:08x}"
        single = ["// TOOLCHAIN vc19", "// LANG c++", FLAGS, f"// MATCH: 0x{h} _FUN_{h}",
                  "#define MATCH_PORT 1", 'extern "C" {', f'#include "{src(bucket, h)}"', "}", ""]
        (TU / f"single_{h}.cpp").write_text("\n".join(single))
        lines = ["// TOOLCHAIN vc19", "// LANG c++", FLAGS, f"// MATCH: 0x{h} _FUN_{h}", "#define MATCH_PORT 1"]
        for hp in helpers:
            lines.append(f"#define FUN_{hp} Port_{hp}")
        if rva == 0x00458920:
            lines.append("#define FUN_00458570 Port_00458570")
        lines += ["namespace U {", 'extern "C" {', f'#include "{src(bucket, h)}"', "}", "}"]
        for i, hp in enumerate(helpers):
            lines += [f"namespace H{i} {{", "using namespace U;", 'extern "C" {', f'#include "{src("s06", hp)}"', "}",
                      "}"]
        if rva == 0x00458920:
            lines += ["using namespace U;", ADAPTER_458570]
        (TU / f"unit_{h}.cpp").write_text("\n".join(lines) + "\n")
        print(f"wrote tu/single_{h}.cpp tu/unit_{h}.cpp")


if __name__ == "__main__":
    main()
