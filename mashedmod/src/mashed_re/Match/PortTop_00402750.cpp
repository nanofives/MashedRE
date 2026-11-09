// PORT-TOP-04E (L-96 top-down lane): exe-only port body of rva 0x00402750 AppInitialiseOnBootup = the matched source
// re/match/s01/00402750.c (match.py BYTE-IDENTICAL 739 B after the MGLOBAL conversion, relocheck OK 93/93). Reached only
// on the MASHED_ORIG_TOP path (WinMain 0x00492370 -> 0x00402750). Every callee binds by name, no raw-address call:
// a matched or existing exe body where one runs safely, else a hand piece (HandTop_InitRest split at this body's call
// points), the piz file system on the exe's piz reader, or a stand-in (Match/PortTop_00402750_Pieces.cpp).
#ifdef MASHED_STANDALONE

#define MATCH_PORT 1
#define FUN_00402750 Orig_00402750
// exe bodies
#define FUN_004d8560 UtilReturnOne4d8560                     // exe body, Util/UtilLeaves_ab6.cpp (returns 1)
#define FUN_004963e0 PortTop_CfgLogDebug                     // log stand-in, Match/PortTop_00492370.cpp
#define FUN_004c2ed0 MashedRw_EngineGetModeInfo_004c2ed0     // RW: RwEngineGetModeInfo -> librw
#define FUN_004c2f00 MashedRw_EngineGetCurrentMode_004c2f00  // RW: RwEngineGetCurrentMode -> librw
#define FUN_00498bc0 VideoGetRenderWidth                     // matched, Match/MatchS16_00498bc0.cpp
#define FUN_00498bd0 VideoGetRenderHeight                    // matched, Match/MatchS16_00498bd0.cpp
#define FUN_004671a0 BackBuffRasterOpen                      // matched, Match/MatchS11_004671a0.cpp
#define FUN_0045b350 RwInitNullStub                          // matched, Match/MatchS09_0045b350.cpp (bare RET)
#define FUN_00499ce0 Orig_00499ce0                           // matched, Match/PortTop_00499ce0.cpp (identity matrix 0x00773928)
#define FUN_004274d0 LangIndexSeedFromCli                    // matched, Match/MatchS06_004274d0.cpp (DAT_007f0f60 = DAT_007719e8), exe list since PORT-TOP-04E
#define FUN_00431b40 Set67eaa8                               // matched, Match/MatchS07_00431b40.cpp (DAT_0067eaa8 = 0.3f), exe list since PORT-TOP-04E
// matched bodies moved off their stand-ins in PORT-TOP-04F (fonts, pc.txd, perm families)
#define FUN_004283a0 Orig_004283a0                           // matched, Match/PortTop_004283a0.cpp (four logo textures)
#define FUN_004274e0 LocalizationFileLoad                    // matched, Match/MatchS06_004274e0.cpp (english.dat -> DAT_0066d828), exe list since PORT-TOP-04F
#define FUN_004b3d80 Orig_004b3d80                           // matched, Match/PortTop_004b3d80.cpp (pc.txd -> DAT_00636ac8)
#define FUN_00425bc0 Orig_00425bc0                           // matched, Match/PortTop_00425bc0.cpp (Copters.txd -> DAT_00644150)
#define FUN_0040bb50 SpriteLookupC                          // matched, Match/MatchS02_0040bb50.cpp (BADGES dict DAT_0063b8fc), both targets since PORT-TOP-04G
#define FUN_0040bb30 Search40bb30                           // matched, Match/MatchS02_0040bb30.cpp (FX dict DAT_0063b8f8), both targets since PORT-TOP-04G
// hand pieces
#define FUN_00496e40 PortTop_Piece_00496e40                  // piece 1: data wedge, boot chain, DirectInput
#define FUN_00495350 PortTop_Piece_00495350                  // piece 2: dsound.dll guard, boot movies
#define FUN_004669b0 PortTop_Piece_004669b0                  // piece 3: exe audio, permdict.rws, menu music
#define FUN_00403640 PortTop_Piece_00403640                  // piece 4: texture upload, B15 bridge, B16 thunks
#define FUN_00428390 PortTop_Piece_00428390                  // piece 5: frontend assets
// piz file system on the exe's piz reader
#define FUN_004b6540 PortTop_PizFs_Init_004b6540
#define FUN_004b6560 PortTop_PizFs_SetIdle_004b6560
#define FUN_00495280 PortTop_PizFs_Mount_00495280
#define FUN_004952f0 PortTop_PizFs_Unmount_004952f0
// device stand-in
#define FUN_004caea0 PortTop_StandIn04E_004caea0
// stand-ins (loader families not moved yet)
#define FUN_00429290 PortTop_StandIn04E_00429290
#define FUN_00494c80 PortTop_StandIn04E_00494c80
#define FUN_00427ca0 PortTop_StandIn04E_00427ca0
#define FUN_004275d0 PortTop_StandIn04E_004275d0
#define FUN_004113b0 PortTop_StandIn04E_004113b0
#define FUN_00558240 PortTop_StandIn04E_00558240
#define FUN_004841d0 PortTop_StandIn04E_004841d0
#define FUN_00484170 PortTop_StandIn04E_00484170
#define FUN_0040bbb0 PortTop_StandIn04E_0040bbb0
#define FUN_004723d0 PortTop_StandIn04E_004723d0
#define FUN_004881d0 PortTop_StandIn04E_004881d0
#define FUN_0047ba00 PortTop_StandIn04E_0047ba00
#define FUN_0045bae0 PortTop_StandIn04E_0045bae0
#define FUN_00418980 PortTop_StandIn04E_00418980
#define FUN_0041b450 PortTop_StandIn04E_0041b450
#define FUN_0041bec0 PortTop_StandIn04E_0041bec0
#define FUN_0041cb10 PortTop_StandIn04E_0041cb10
#define FUN_0041d6e0 PortTop_StandIn04E_0041d6e0
#define FUN_0041def0 PortTop_StandIn04E_0041def0
#define FUN_0041d8b0 PortTop_StandIn04E_0041d8b0
#define FUN_0041c100 PortTop_StandIn04E_0041c100
#define FUN_0041db90 PortTop_StandIn04E_0041db90
#define FUN_0041a1e0 PortTop_StandIn04E_0041a1e0
#define FUN_0041eaa0 PortTop_StandIn04E_0041eaa0
#define FUN_00420d00 PortTop_StandIn04E_00420d00
#define FUN_00412890 PortTop_StandIn04E_00412890
#define FUN_00471eb0 PortTop_StandIn04E_00471eb0
#define FUN_00404830 PortTop_StandIn04E_00404830
#define FUN_004025f0 PortTop_StandIn04E_004025f0
#define FUN_00402f50 PortTop_StandIn04E_00402f50
#define FUN_004026d0 PortTop_StandIn04E_004026d0
extern "C" {
#include "../../../../re/match/s01/00402750.c"
}
#endif  // MASHED_STANDALONE
