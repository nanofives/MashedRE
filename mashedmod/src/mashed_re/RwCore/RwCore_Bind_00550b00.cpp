// RWC-FRAME-TITLE-LOGO (run 50, L-124 (C)): the top-down binding entry of the RtFS file-exists call 0x00550b00. EXE ONLY.
//
// Not a body of 0x00550b00. The body is the byte-matched source re/match/s24/00550b00.c (100%), compiled for the RW core
// path as RwCore_00550b00 (RwCore/RwCore_00550b00.cpp, generated; a DDK object in the object build). It picks the RtFS
// file system that owns the name (a "<fs>:" prefix, else the default one at DAT_007dc76c) when the RtFS manager is open
// (DAT_007dc75c != 0) and returns that file system's +0x4c file-exists result, else calls the error callback DAT_007dc768
// with 6 and returns 0.
//
// The exe has no RtFSManager (PORT-TOP-04F: the piz mounts of 0x00495280 feed the exe's piz reader, not an RtFS file
// system), so on the RW core DAT_007dc75c is 0 and the body answers 0 for every name. Its core callers are the image
// file-type search (0x004ce6c0 -> 0x004ce480 -> callback 0x004ce6f0, lifted) and the image read callback 0x004ce690:
// with every answer 0, RwImageFindFileType finds no extension and RwTextureRead 0x004c5cb0 returns 0 for the
// font36.piz logos (verify/rwcframetitlelogo_20261115/RESULT_RWC-FRAME-TITLE-LOGO.md).
//
// gen_rwcore_tus.py CALL_VIA / LIFT_VIA route both callers here. RtFS manager open: the matched body. Not open: the same
// answer as the 0x004b5350 shim (PortTop_FsFileExists_004b5350, Match/PortTop_RwStream04F.cpp): 1 when the name is in
// a mounted piz or is a file on the disk under "original/", else 0, the two places the RwStreamOpen binding 0x004cc230
// reads from. Declared deviation: no error callback call on a miss.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE

extern "C" {
void RwCore_Log(const char* fmt, ...);
int RwCore_00550b00(char* path);
int PortTop_FsFileExists_004b5350(const char* name);

int RwCoreBind_FsFileExists_00550b00(char* path) {
    static unsigned s_calls = 0;
    const int open = *reinterpret_cast<const volatile int*>(static_cast<unsigned long>(0x007dc75cu));
    const int r = open ? RwCore_00550b00(path) : PortTop_FsFileExists_004b5350(path);
    if (++s_calls <= 24)
        RwCore_Log("[RWC-FRAME-TITLE-LOGO] 0x00550b00(\"%s\") call %u: RtFS manager [0x007dc75c] = %d -> %s = %d",
                   path ? path : "(null)", s_calls, open, open ? "matched body" : "piz/disk (0x004b5350 shim)", r);
    return r;
}
}  // extern "C"

#endif  // MASHED_STANDALONE
