#include <windows.h>
#include <cstdio>
static LONG trySet(DWORD w, DWORD h, DWORD flags) {
    DEVMODEW dm = {}; dm.dmSize = sizeof(dm); int i=0; DEVMODEW pick={}; bool got=false;
    while (EnumDisplaySettingsW(NULL, i++, &dm)) {
        if (dm.dmPelsWidth==w && dm.dmPelsHeight==h && dm.dmBitsPerPel>=32) { pick=dm; got=true; if(dm.dmDisplayFrequency==60) break; }
    }
    if(!got){ printf("  mode %lux%lu not found\n",w,h); return -99; }
    pick.dmFields = DM_PELSWIDTH|DM_PELSHEIGHT|DM_BITSPERPEL|DM_DISPLAYFREQUENCY;
    LONG r = ChangeDisplaySettingsW(&pick, flags);
    printf("  set %lux%lu@%lu flags=0x%lx -> %ld\n", w,h,pick.dmDisplayFrequency,flags,r);
    return r;
}
int main() {
    // try a sequence: native-ish then safe modes, with and without CDS_RESET
    DWORD modes[][2] = {{1920,1080},{1600,900},{1366,768},{2560,1600},{1280,1024}};
    for (auto& m : modes) { if (trySet(m[0],m[1],CDS_UPDATEREGISTRY)==0) { printf("OK\n"); return 0; } }
    printf("-- retry with CDS_RESET --\n");
    for (auto& m : modes) { if (trySet(m[0],m[1],CDS_UPDATEREGISTRY|CDS_RESET)==0) { printf("OK\n"); return 0; } }
    printf("-- reset to registry default --\n");
    LONG r = ChangeDisplaySettingsW(NULL, 0); printf("  CDS default -> %ld\n", r);
    return r==0?0:1;
}
