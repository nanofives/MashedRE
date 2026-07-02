// Standalone resolution-restore helper: enumerate the highest mode on the
// primary adapter and apply it. Run as its own process so it has display access
// the (session-limited) shell P/Invoke lacks.
#include <windows.h>
#include <cstdio>
int main() {
    DEVMODEW best = {}; best.dmSize = sizeof(best);
    DEVMODEW dm = {}; dm.dmSize = sizeof(dm);
    DWORD bestArea = 0; int i = 0;
    while (EnumDisplaySettingsW(NULL, i, &dm)) {
        DWORD area = (DWORD)dm.dmPelsWidth * (DWORD)dm.dmPelsHeight;
        if (dm.dmBitsPerPel >= 32 && area > bestArea) { bestArea = area; best = dm; }
        i++;
    }
    printf("enumerated %d modes; best=%lux%lu@%lu bpp=%lu\n",
           i, best.dmPelsWidth, best.dmPelsHeight, best.dmDisplayFrequency, best.dmBitsPerPel);
    if (bestArea == 0) { printf("NO MODES (no display access)\n"); return 2; }
    best.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;
    LONG r = ChangeDisplaySettingsW(&best, CDS_UPDATEREGISTRY);
    printf("ChangeDisplaySettings = %ld (0=ok)\n", r);
    return r == 0 ? 0 : 1;
}
