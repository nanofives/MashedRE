// Mashed RE - Vehicle physics leaf cluster (c3_batch_j4).
//
// This file used to hold VehicleSpinoutStateSet, a second implementation of
// 0x0046cbe0. It was removed 2026-10-12 (reearn/fstate): two RH_ScopedInstall
// rows for one RVA meant the copy installed in the .asi (this one, registered
// first) was not the copy run_diff measured (VehicleCarStateSet), and the two
// bodies had drifted (memory: duplicate-rva-implementations-drift).
//
// The single implementation of 0x0046cbe0 is VehicleCarStateSet in
// Vehicle/SmallLeaves_o5.cpp.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
