// Mashed RE — the Warzone (course 0x21) track object at 0x00684d18: the moving battle platform ("pontoon").
// Exe only. Source map and arms: verify/pontoon_20261016/PREREG_pontoon.md, RESULT_pontoon.md.
#pragma once

#include <cstddef>
#include <cstdint>

namespace mashed_re {
namespace Track {

// Knobs, all default OFF (PREREG section 1). STATE implies ANIM, CARRY implies STATE.
bool Pontoon_AnimOn();    // MASHED_PONTOON_ANIM=1: object init + tick + animated draw (words not written)
bool Pontoon_StateOn();   // MASHED_PONTOON_STATE=1: trigger + the words 0x0088ff50 / 0x00684de0 / 0x00684dac live
bool Pontoon_CarryOn();   // MASHED_PONTOON_CARRY=1: in-box carry + exit velocity (FUN_0046bf50 record branch)

// What the object needs from the exe's car physics (the original's FUN_0040e340 / FUN_0046d4a0 / record fields).
struct PontoonHost {
    int    (*car_count)() = nullptr;                          // FUN_0040e340 = [0x008a94d0]
    float* (*car_matrix)(int v) = nullptr;                    // FUN_0046d4a0: car v's current RwMatrix (16 floats)
    void   (*car_carried)(int v, const float* m) = nullptr;   // the in-box carry rewrote car v's matrix to m
    unsigned char* (*car_record)(int v) = nullptr;            // record 0x008815a0 + v*0xd04 (+0x14, +0x9b0, +0xbfc, +0xc7c)
    int    (*sphere_visible)(const float sphere[4]) = nullptr; // FUN_004671a0(0) + FUN_004c1b40 (state 5 only)
};
void Pontoon_SetHost(const PontoonHost& h);

// Slot 0 0x004527f0 (the object part): Boat.dff + BoatAnim.anm blobs and the Tower / Deck RWP bodies' matrices.
bool Pontoon_Init(const std::uint8_t* dff, std::size_t dlen, const std::uint8_t* anm, std::size_t alen,
                  const float tower[16], const float deck[16]);
void Pontoon_Reset();          // slots 6 / 7 (0x00452af0 / 0x00452ad0): FUN_00451cc0(&obj)
void Pontoon_Tick(int units);  // slot 1 0x00452b20 (the object part): FUN_00451d80 + FUN_00451a50 Tower / Deck
bool Pontoon_Active();
// Draw: DFF stream atomic 0..2 -> its frame LTM (16 floats, RwMatrix layout). False when inactive.
bool Pontoon_AtomicLtm(int stream_atomic, float out[16]);
void Pontoon_Shutdown();

}  // namespace Track
}  // namespace mashed_re
