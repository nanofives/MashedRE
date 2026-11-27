# RO-AUDIOINIT-45D460-REACH-5: new re/match sources for the audio-init window (RO-AUDIOINIT-DEPTH window_functions.tsv).
# 0x005b8910 from REACH-4's draft (draft_005b8910.c, Ghidra decompilation of the main pool); 0x004ccef0, 0x005a66c0 and
# 0x005c6f50 from their listings (Ghidra pool slot Mashed_pool0, read-only).
import sys
from pathlib import Path
WT = Path(sys.argv[1])
D = Path(__file__).parent
pre = (D / "preamble.txt").read_text()


def head(sdir, rva):
    return "// FLAGS: /O2 /Ob1 /G5\n/* MATCH-%s 0x%s: RO-AUDIOINIT-45D460-REACH-5, see below. */\n" % (sdir.upper(), rva)


out = {}

d = (D / "draft_005b8910.c").read_text()
d = d.replace("RO-AUDIOINIT-45D460-REACH-4, see below.", "RO-AUDIOINIT-45D460-REACH-5, see below.", 1)
d = d.replace("/* RO-AUDIOINIT-45D460-REACH-4 (2026-11-22): new source",
              "/* RO-AUDIOINIT-45D460-REACH-5 (2026-11-27), from RO-AUDIOINIT-45D460-REACH-4's draft (2026-11-22): new source", 1)
out[("s27", "005b8910")] = d

out[("s20", "004ccef0")] = head("s20", "004ccef0") + pre + """/* RO-AUDIOINIT-45D460-REACH-5 (2026-11-27): new source from the listing 0x004ccef0..0x004ccefd (14 B, pc_funcs.csv;
   hooks.csv C1, no re/match source before this row): mov eax,[esp+4] / push eax / call 0x004a45fb / add esp,4 / ret.
   0x005aea60 stores its address at RwGlobals+0x108 (re/match/s26/005aea60.c). */
void *FUN_004a45fb(unsigned int n);

// MATCH: 0x004ccef0 FUN_004ccef0
void *FUN_004ccef0(unsigned int param_1)
{
  return FUN_004a45fb(param_1);
}
"""

out[("s25", "005a66c0")] = head("s25", "005a66c0") + pre + """/* RO-AUDIOINIT-45D460-REACH-5 (2026-11-27): new source from the listing 0x005a66c0..0x005a66ce (15 B, pc_funcs.csv;
   no hooks.csv row or re/match source before this row): stores 0 at *param_2 (0x005a66c4), returns param_1 (0x005a66ca).
   0x005a6340 takes its address (re/match/s25/005a6340.c). */

// MATCH: 0x005a66c0 FUN_005a66c0
int FUN_005a66c0(int param_1, undefined4 *param_2)
{
  *param_2 = 0;
  return param_1;
}
"""

out[("s28", "005c6f50")] = head("s28", "005c6f50") + pre + """/* RO-AUDIOINIT-45D460-REACH-5 (2026-11-27): new source from the Ghidra decompilation and the listing 0x005c6f50..0x005c7065
   (278 B, pc_funcs.csv; no hooks.csv row or re/match source before this row). 0x005c6cc0 takes its address
   (re/match/s28/005c6cc0.c). Field stores of param_1 from 0x005c6f73..0x005c6ff3 (1.0f = 0x3f800000 at 0x005c6f63, 0xff at
   0x005c6f89, 0x461c4000 at 0x005c6f91, 3 at 0x005c6faa), (float) of the uint at rec+0x18 through fild qword (0x005c6f76),
   rec = *(param_2+0x20) (0x005c6f5d); calls 0x005ab040 (0x005c6ff6), 0x005c6b40 twice (0x005c7008 / 0x005c7014);
   WaitForSingleObject(*(rec+4), -1) (0x005c7028), list insert of param_1+0xd0 at rec+0x1c (0x005c703e..0x005c7051),
   ReleaseSemaphore(*(rec+4), 1, 0) (0x005c7057), returns param_1. */
__declspec(dllimport) unsigned long __stdcall WaitForSingleObject(void *hHandle, unsigned long dwMilliseconds);
__declspec(dllimport) int __stdcall ReleaseSemaphore(void *hSemaphore, long lReleaseCount, long *lpPreviousCount);
undefined4 FUN_005ab040(void);
void FUN_005c6b40(int param_1);

// MATCH: 0x005c6f50 FUN_005c6f50
int FUN_005c6f50(int param_1, int param_2)
{
  int *piVar1;
  int *piVar2;
  int iVar3;

  iVar3 = *(int *)(param_2 + 0x20);
  *(float *)(param_1 + 0x3c) = 1.0f;
  *(float *)(param_1 + 0x40) = 1.0f;
  *(float *)(param_1 + 0x38) = 1.0f;
  *(float *)(param_1 + 0x44) = 1.0f;
  *(float *)(param_1 + 100) = 1.0f;
  *(float *)(param_1 + 0x34) = (float)*(uint *)(iVar3 + 0x18);
  *(undefined4 *)(param_1 + 0x48) = 0;
  *(undefined4 *)(param_1 + 0x68) = 0x461c4000;
  *(undefined4 *)(param_1 + 0x4c) = 0;
  *(undefined4 *)(param_1 + 0x50) = 0;
  *(undefined4 *)(param_1 + 0x54) = 0;
  *(undefined4 *)(param_1 + 0x58) = 0;
  *(undefined4 *)(param_1 + 0x5c) = 0;
  *(undefined4 *)(param_1 + 0x60) = 0;
  *(undefined4 *)(param_1 + 0x28) = 3;
  *(undefined4 *)(param_1 + 0x2c) = 0;
  *(int *)(param_1 + 0x9c) = iVar3;
  *(undefined4 *)(param_1 + 0x98) = 0;
  *(undefined4 *)(param_1 + 0x88) = 0;
  *(undefined4 *)(param_1 + 0x8c) = 0;
  *(undefined4 *)(param_1 + 0x90) = 0;
  *(undefined4 *)(param_1 + 0x94) = 0;
  *(undefined4 *)(param_1 + 0x80) = 0;
  *(undefined4 *)(param_1 + 0x84) = 0;
  *(undefined4 *)(param_1 + 0x6c) = 0xff;
  *(undefined4 *)(param_1 + 0x70) = 0xff;
  *(undefined4 *)(param_1 + 0x74) = 0xff;
  *(undefined4 *)(param_1 + 0x78) = 0;
  *(undefined4 *)(param_1 + 0x7c) = *(undefined4 *)(iVar3 + 0x30);
  *(undefined4 *)(param_1 + 0xa0) = FUN_005ab040();
  FUN_005c6b40(param_1 + 0xa4);
  FUN_005c6b40(param_1 + 0xb8);
  *(undefined4 *)(param_1 + 0xcc) = 0;
  WaitForSingleObject(*(void **)(iVar3 + 4), 0xffffffff);
  piVar2 = (int *)(param_1 + 0xd0);
  piVar1 = (int *)(iVar3 + 0x1c);
  *(undefined4 *)(param_1 + 0xd4) = 0;
  *piVar2 = 0;
  *piVar2 = *piVar1;
  *(int **)(param_1 + 0xd4) = piVar1;
  *(int **)(*piVar1 + 4) = piVar2;
  *piVar1 = (int)piVar2;
  ReleaseSemaphore(*(void **)(iVar3 + 4), 1, (long *)0x0);
  return param_1;
}
"""

for (sdir, rva), s in out.items():
    p = WT / "re" / "match" / sdir / (rva + ".c")
    if p.exists() and "--force" not in sys.argv:
        print("exists, skip", p)
        continue
    p.write_text(s, newline="\n")
    print("wrote", p)
