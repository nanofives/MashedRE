#pragma once
// Vehicle -> engine class (eng1..eng4), data only. No caller yet (D4-WSJ-ENGCLASS).
// Source: FUN_00463640 @ 0x00463640 (per-car engine tick). It reads the vehicle-record
// id FUN_0041efc0(car) = *(DAT_0063dc48[car*0x2ac] + 0x80) (0x0041efc0), computes
// id/6 with the multiply at 0x00463653..0x00463662 (0x2aaaaaab, vehicle index 0..12),
// and dispatches `CMP EDI,0xb / JA 0x004636d9 / JMP [EDI*4 + 0x463c34]` (0x0046367f..0x00463689).
// The jump table holds 12 dwords at 0x00463c34; each target writes DAT_0069055c (class):
//   0x00463690 -> 3, 0x004636a6 -> 1, 0x004636ad -> 2, 0x004636c3 -> 2, 0x004636d9 -> 0.
// The permdict row is class*2 (row stride 0x94 = 0x25 dwords; class stride 0x4a dwords at 0x0046370b
// IMUL EAX,EAX,0x128), so class 0..3 = eng1..eng4 (rows 0,2,4,6) and the `d` row is row+1.
// The voice is slot (DAT_00604570[class*0x4a] + car) (0x00463725, 0x00463731), one of the
// row's 4 voices (+0x84 = 4), and the `d` voice is that slot + 4 (0x0045daf0(iVar5+4) path).
namespace Audio {
// index = vehicle index (id/6), 13 vehicles, record table 0x005f37a8 stride 0x84, count DAT_005f5fe0 = 78.
// value = DAT_0069055c class: 0=eng1 1=eng2 2=eng3 3=eng4.
constexpr unsigned char kEngineClassByVehicle[13] = {
    3, // 0 Shuriken
    1, // 1 Advantage
    2, // 2 Atmos
    1, // 3 Stallion
    3, // 4 Bullet
    3, // 5 Sputter
    2, // 6 Cooler
    0, // 7 Shorty
    2, // 8 Crusader
    2, // 9 Creeper
    0, // 10 BamBam
    3, // 11 Kustom
    0, // 12 Formula (EDI > 0xb -> 0x004636d9, JA at 0x00463687)
};
// Per class: pitch base added to the rpm term (row +0x90 read at 0x0046372b), min rpm
// DAT_006130c4 (written 0x0046369a/0x004636b7/0x004636cd/0x004636e3), volume bias DAT_00613020[class].
constexpr float kEngineBasePitch[4] = {2000.0f, 0.0f, 2000.0f, 2000.0f};
constexpr float kEngineMinRpm[4]    = {2000.0f, 2000.0f, 1500.0f, 1500.0f};
constexpr float kEngineVolBias[4]   = {0.2f, 0.2f, 0.2f, 0.3f};
} // namespace Audio
