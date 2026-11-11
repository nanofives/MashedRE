// FLAGS: /O2 /GS
/* TL-MATCH-REST 0x00440bc0 (no hooks.csv row before this slice): raw g2s.py output (decomp_pc.py --port --callees, Mashed_pool, 2026-11-10), body unchanged. Starting point only, not matched; score in verify/tlmatchrest_20261110/scores.tsv. */
// TOOLCHAIN ddk
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10; typedef int code();
typedef unsigned char bool; typedef unsigned int WPARAM; typedef long LPARAM; typedef long LRESULT; typedef void *HWND;
typedef void *HINSTANCE; typedef void *HMODULE; typedef void *HANDLE; typedef int BOOL; typedef unsigned int UINT;
typedef unsigned long DWORD; typedef char *LPSTR; typedef const char *LPCSTR; typedef void *LPVOID; typedef unsigned short WORD;
typedef unsigned char BYTE; typedef long LONG; typedef void *HDC; typedef void *HMENU; typedef void *HICON;
typedef unsigned short wchar_t; typedef wchar_t *LPWSTR; typedef const wchar_t *LPCWSTR; typedef int INT_PTR;
typedef unsigned int UINT_PTR; typedef long LONG_PTR; typedef unsigned long ULONG_PTR; typedef long HRESULT;
typedef long double unkbyte10; typedef unsigned int uintptr_t; typedef unsigned int size_t;
#define true 1
#define false 0
#define CONCAT11(a,b) ((ushort)(((uint)(uchar)(a)<<8)|(uchar)(b)))
#define CONCAT12(a,b) ((uint)(((uint)(uchar)(a)<<16)|(ushort)(b)))
#define CONCAT13(a,b) ((uint)(((uint)(uchar)(a)<<24)|((uint)(b)&0xffffff)))
#define CONCAT21(a,b) ((uint)(((uint)(ushort)(a)<<8)|(uchar)(b)))
#define CONCAT22(a,b) ((uint)(((uint)(ushort)(a)<<16)|(ushort)(b)))
#define CONCAT31(a,b) ((uint)(((uint)(a)<<8)|(uchar)(b)))
#define CONCAT44(a,b) ((ulonglong)(((ulonglong)(uint)(a)<<32)|(uint)(b)))
#define SUB41(a,n) ((uchar)((uint)(a)>>((n)*8)))
#define SUB42(a,n) ((ushort)((uint)(a)>>((n)*8)))
#define SUB81(a,n) ((uchar)((ulonglong)(a)>>((n)*8)))
#define SUB84(a,n) ((uint)((ulonglong)(a)>>((n)*8)))
#define ZEXT14(a) ((uint)(uchar)(a))
#define ZEXT24(a) ((uint)(ushort)(a))
#define ZEXT48(a) ((ulonglong)(uint)(a))
#define SEXT14(a) ((int)(signed char)(a))
#define SEXT24(a) ((int)(short)(a))
#define SEXT48(a) ((longlong)(int)(a))

extern undefined1 LAB_0047ad30;
extern undefined1 LAB_004917a0;
extern undefined1 FUN_0047a320;
extern undefined1 FUN_0047ade0;
extern undefined1 FUN_0047ae80;
extern undefined1 FUN_0047aef0;
extern undefined1 FUN_0047af50;
extern undefined1 FUN_0047afa0;
extern undefined1 FUN_0047b000;
extern undefined1 FUN_0047b0a0;
extern undefined1 FUN_0047b110;
extern undefined1 FUN_0047b160;
extern undefined1 FUN_0047b420;
extern undefined1 FUN_0047b480;
extern undefined1 FUN_0047b5b0;
extern undefined1 FUN_0047b6b0;
extern undefined1 FUN_0047a180;
extern undefined1 FUN_0047a1b0;
extern undefined1 FUN_0047a1e0;
extern undefined1 FUN_0047a280;
extern undefined1 FUN_0047a3a0;
extern undefined1 FUN_0047a4a0;
extern undefined1 FUN_0047a540;
extern undefined1 FUN_0047a580;
extern undefined1 FUN_0047a5b0;
extern undefined1 FUN_0047a5e0;
extern undefined1 FUN_0047a610;
extern undefined1 FUN_0047a6b0;
extern undefined1 FUN_0047a6f0;
extern undefined1 FUN_0047a720;
extern undefined1 FUN_0047a790;
extern undefined1 FUN_0047a880;
extern undefined1 FUN_0047a8b0;
extern undefined1 FUN_0047aa20;
extern undefined1 FUN_0047aa50;
extern undefined1 FUN_0047aaa0;
extern undefined1 FUN_0047aad0;
extern undefined1 FUN_0047ab30;
extern undefined1 FUN_0047abd0;
extern undefined1 FUN_0047acd0;
extern undefined1 FUN_0047ad00;
extern undefined1 FUN_0047b1b0;
extern undefined1 FUN_0047a440;
extern undefined1 FUN_0047b200;
extern undefined1 FUN_0047a750;
extern undefined1 FUN_0047b230;
extern undefined1 FUN_0047a7d0;
extern undefined1 FUN_0047a7f0;
extern undefined1 FUN_0047a810;
extern undefined1 FUN_0047a950;
extern undefined1 FUN_0047a9b0;
extern undefined1 FUN_0047ac80;
extern undefined1 FUN_0047ad40;
extern undefined1 FUN_0047b250;
extern undefined1 FUN_0047b280;
extern undefined1 FUN_0047b350;
extern undefined1 FUN_0047b3c0;
extern undefined1 FUN_0047b500;
extern undefined1 FUN_0047b630;
extern undefined1 FUN_0047b720;
extern undefined1 FUN_0047b7d0;
extern undefined1 FUN_004917c0;
extern undefined1 FUN_00491860;
extern undefined1 FUN_00491900;
extern undefined1 FUN_004919b0;
extern undefined1 FUN_00491a10;
extern undefined1 FUN_00491a70;
extern undefined1 FUN_005c9d00;
void FUN_0047b980(undefined4 p0, undefined4 p1);
// MATCH: 0x00440bc0 FUN_00440bc0


void FUN_00440bc0(void)



{

  FUN_0047b980("Course_Id",FUN_0047a180);

  FUN_0047b980("AI_Bsp_Filename",FUN_0047a540);

  FUN_0047b980("AI_Data_Filename",FUN_0047a580);

  FUN_0047b980("World_Bsp_Filename",FUN_0047a5b0);

  FUN_0047b980("Hoppy_Bsp_Filename",FUN_0047a5e0);

  FUN_0047b980("Object_Bsp_Filename",FUN_0047a610);

  FUN_0047b980("Collision_Bsp_Filename",FUN_0047a6b0);

  FUN_0047b980("Texture_Dictionary_Filename",FUN_0047a6f0);

  FUN_0047b980("Occluder_Filename",FUN_0047a1b0);

  FUN_0047b980("Sky_Filename",FUN_0047a1e0);

  FUN_0047b980("Mts_Filename",FUN_0047a280);

  FUN_0047b980("Clump_Filename",FUN_0047a3a0);

  FUN_0047b980("UVA_Filename",FUN_0047a440);

  FUN_0047b980("Spline_Filename",FUN_0047a4a0);

  FUN_0047b980("Camera_Anim_Filename",FUN_0047a880);

  FUN_0047b980("General_Anim_Filename",FUN_0047a8b0);

  FUN_0047b980("Clump_Mts_Pair",FUN_0047a950);

  FUN_0047b980("Physics_Object",FUN_0047a9b0);

  FUN_0047b980("Course_Name",FUN_005c9d00);

  FUN_0047b980("Clump_Exclude_From_World",FUN_0047aa20);

  FUN_0047b980("Powerup_Filename",FUN_0047aa50);

  FUN_0047b980("Lights_Filename",FUN_0047aaa0);

  FUN_0047b980("Ambient_RGB",FUN_0047aad0);

  FUN_0047b980("Setup_Fog",FUN_0047ab30);

  FUN_0047b980("Modify_Fog",FUN_0047abd0);

  FUN_0047b980("Vehicle_Shininess_Range",FUN_0047ac80);

  FUN_0047b980("Shine_Scale",FUN_0047acd0);

  FUN_0047b980("Shadow_Scale",FUN_0047ad00);

  FUN_0047b980("Vehicle_Body_Dirt_Texture",&LAB_0047ad30);

  FUN_0047b980("Vehicle_Wheel_Dirt_Texture",&LAB_0047ad30);

  FUN_0047b980("SetClumpAnim",FUN_0047ad40);

  FUN_0047b980("Lap_Variations",FUN_0047a720);

  FUN_0047b980("Lap_Line",FUN_0047a790);

  FUN_0047b980("Lap_Line_End",FUN_0047a7d0);

  FUN_0047b980("Lap_Line_Change",FUN_0047a7f0);

  FUN_0047b980("Safe_Start_Lines",FUN_0047a810);

  FUN_0047b980("Split_Sector",FUN_0047a750);

  FUN_0047b980("RWP_Object",FUN_0047ade0);

  FUN_0047b980("RWP_Object_Box",FUN_0047ae80);

  FUN_0047b980("RWP_Object_Cylinder",FUN_0047aef0);

  FUN_0047b980("RWP_Object_Sphere",FUN_0047af50);

  FUN_0047b980("RWP_Object_Capsule",FUN_0047afa0);

  FUN_0047b980("RWP_Object_Properties",FUN_0047b000);

  FUN_0047b980("RWP_Object_CentreOfMassOffset",FUN_0047b0a0);

  FUN_0047b980("RWP_Object_BaseLabel",FUN_0047b110);

  FUN_0047b980("RWP_Object_ShatterVel",FUN_0047b160);

  FUN_0047b980("Physics_Scenario_Create",FUN_0047b350);

  FUN_0047b980("Physics_Scenario_Set_Sequence",FUN_0047b3c0);

  FUN_0047b980("Physics_Scenario_Add_Objects",FUN_0047b420);

  FUN_0047b980("Physics_Scenario_Add_Animated_Object",FUN_0047b480);

  FUN_0047b980("Physics_Scenario_Create_Trigger",FUN_0047b500);

  FUN_0047b980("Physics_Scenario_Trigger_Add_Objects",FUN_0047b5b0);

  FUN_0047b980("Physics_Scenario_Get_Function",FUN_0047b630);

  FUN_0047b980("Physics_Scenario_Trigger_Set_AI_Poly",FUN_0047b6b0);

  FUN_0047b980("Set_Current_Position",FUN_0047b1b0);

  FUN_0047b980("Set_Current_Type",FUN_0047b200);

  FUN_0047b980("Set_Current_Respawn_Time",FUN_0047b230);

  FUN_0047b980("Place_Powerup",FUN_0047b250);

  FUN_0047b980("Set_Track_Powerups",FUN_0047b280);

  FUN_0047b980("RainEnable",&LAB_004917a0);

  FUN_0047b980("RainSetHeadColour",FUN_004917c0);

  FUN_0047b980("RainSetTailColour",FUN_00491860);

  FUN_0047b980("RainSetPosition",FUN_00491900);

  FUN_0047b980("RainSetScale",FUN_004919b0);

  FUN_0047b980("RainSetCameraScale",FUN_00491a10);

  FUN_0047b980("RainSetDirection",FUN_00491a70);

  FUN_0047b980("SetCopter",FUN_0047a320);

  FUN_0047b980("KTC_NewCopter",FUN_0047b720);

  FUN_0047b980("KTC_AddPickUp",FUN_0047b7d0);

  return;

}



