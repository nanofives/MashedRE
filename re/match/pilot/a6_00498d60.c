// FLAGS: /O2 /GS /Ob1
/* 0x00498d60 (219 B). Best 100.0% exact (regs 100, ops 100), plain cl 13.00.9176, no /GL.
   Needed: (1) FUN_00498a00 written as a `static` function before this one. The original passes it
   Mode* m in EBX (0x00498a14 reads [ebx+0xc]), which is cl's static-function register convention.
   (2) FUN_00498d60 itself `static`, with its two callers (0x00499137, 0x004991cf) after it in the same
   file. caller_00499100 is a stand-in. With an extern FUN_00498d60 the allocator keeps wnd in EBP and
   reloads the SendMessageA import per call (84.1%, unchanged by 149 permuter compiles). The static
   form hoists the import into EBP and reloads wnd into EBX, as the original does.
   (3) The `xor eax,eax / sete al / test al,1` at 0x00498db3 needs `!((m->ok == 0) & 1)`. `!= 0`,
   plain `!`, bool and inline-bool all fold it away (60.3%).
   History: 61.8% Ghidra shape -> 70.6% (&1 test) -> 84.1% (static callee) -> 100% (static self).
   FUN_00498a00 (bonus, not in the sample): 96.7%. The 4 extra instructions are its epilogue past
   the 344-B Ghidra size.
   Residual: none. */

typedef void *HWND; typedef unsigned int UINT; typedef unsigned int WPARAM; typedef long LPARAM; typedef long LRESULT;
__declspec(dllimport) LRESULT __stdcall SendMessageA(HWND w, UINT msg, WPARAM wp, LPARAM lp);
typedef struct Mode { int w; int h; int bpp; int ok; int r10; int r14; } Mode;
extern int  *DAT_007731f8;
extern Mode **DAT_00773408;
extern int   DAT_0077340c;
extern int   DAT_00773418[];
char *strcat(char *, const char *);
void *memset(void *, int, unsigned int);
#pragma intrinsic(strcat, memset)
int sprintf(char *o, const char *f, ...);

/* FUN_00498a00 (344 B, not in this sample): mode label. Strings: 0x005d0200 0x005d01f4 0x005d01e4
   0x005d01c8 0x005d01a4 0x005d0180, format 0x005d0170. */
// MATCH: 0x00498a00 _FUN_00498a00
static void FUN_00498a00(Mode *m, char *out)
{
    char buf[128] = "";
    if (m->ok & 1)
        strcat(buf, "[Fullscreen]");
    else
        strcat(buf, "[Window]");
    if (m->ok & 2)
        strcat(buf, "[Interlaced]");
    if (m->ok & 4)
        strcat(buf, "[Flicker Free Interlaced]");
    if (m->ok & 0x100)
        strcat(buf, "[Fullscreen antialiasing mode 0]");
    if (m->ok & 0x200)
        strcat(buf, "[Fullscreen antialiasing mode 1]");
    sprintf(out, "%d x %d x %d %s", m->w, m->h, m->bpp, buf);
}

// MATCH: 0x00498d60 _FUN_00498d60
static void FUN_00498d60(HWND wnd)
{
    char text[64];
    int i;
    for (i = 0; i < DAT_007731f8[DAT_0077340c]; i++) {
        Mode *m = &DAT_00773408[DAT_0077340c][i];
        if (!((m->ok == 0) & 1) && m->w >= 0x280 && m->h >= 0x1e0)
        {
            WPARAM idx;
            FUN_00498a00(m, text);
            idx = SendMessageA(wnd, 0x143, 0, (LPARAM)text);
            SendMessageA(wnd, 0x151, idx, i);
            DAT_00773418[i] = idx;
        }
    }
}

int caller_00499100(HWND dlg, HWND combo, int which)
{
    if (which) {
        SendMessageA(combo, 0x14b, 0, 0);
        FUN_00498d60(combo);
    } else {
        SendMessageA(dlg, 0x14e, DAT_0077340c, 0);
        FUN_00498d60(dlg);
    }
    return 1;
}
