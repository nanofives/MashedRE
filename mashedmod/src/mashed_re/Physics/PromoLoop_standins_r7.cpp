// Mashed RE: stand-in promotion round 7 (plan item #3 step 2, 2026-10-11).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// 0x004218b0  PhysBodyCreateAttach  (FUN_004218b0, 31 B; caller 0x004218d0)
//   ABI: EAX = src (pushed as the only argument of 0x0057c300 at 0x004218b1), [esp+4] = target; ESI preserved.
//   0x004218b2  body = 0x0057c300(src)               RWP37BodyCreate
//   0x004218b9  [body+8] &= 0xfffffff3
//   0x004218c3  0x0057c220(target, body)             RWP37WorldSlotAssign: stores body at target + *(0x007dc8d8)
//   0x004218cb  return body
//
// 0x004218d0  PhysWorldBodyBuild  (FUN_004218d0, 87 B, cdecl(a1, a2, a3, a4, a5); caller 0x00421a90)
//   0x004218d2  world = 0x0057c500()                 RWP37WorldCreate;  prim = [world] (0x004218dd)
//   0x004218e1  0x0055c540(prim, a2)
//   0x004218ec  0x0055c4f0(prim, a5)                 PhysicsBodySetFriction
//   0x004218f7  0x0055c4a0(prim, a4)                 PhysicsBodySetRestitution
//   0x00421904  0x0055b940(world, a3, 0)             PhysicsBodySetMass; its x87 result is discarded (fstp st(0))
//   0x00421912  body = 0x004218b0(EAX = world, a1)
//   0x0042191a  0x0057c550(world)
//   0x00421922  return body  (add esp, 0x2c: all eleven pushes caller-cleaned)

#include "../Core/HookSystem.h"

// MATCH-S05: the body of 0x004218b0 (PhysBodyCreateAttach) moved to Match/MatchS05_004218b0.cpp,
// which compiles the byte-matched source re/match/s05/004218b0.c.

// MATCH-S05: the body of 0x004218d0 (PhysWorldBodyBuild) moved to Match/MatchS05_004218b0.cpp,
// which compiles the byte-matched source re/match/s05/004218b0.c.
