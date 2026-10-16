-- course.lua
--Egypt
Course_Id(3)

-------------------------------------------------
-- World Stuf -----------------------------------
-------------------------------------------------
AI_Bsp_Filename("lap.bsp")
World_Bsp_Filename("graphics.bsp")
Collision_Bsp_Filename("collisions.bsp")
Texture_Dictionary_Filename("textures.txd")
Lights_Filename("Lights.dff")
Sky_Filename(0,"sky.dff")
Setup_Fog(20,100,255,228,178)


--Occluder_Filename("Occluder.dff")
--Spline_Filename(index,"filename.spl")
--General_Anim_Filename(index,"filename.anm")
--Camera_Anim_Filename("filename.anm")
------------------------------------------------
-- Copter Stuff --------------------------------
------------------------------------------------
General_Anim_Filename(0,"H_Anim_1.anm")
General_Anim_Filename(1,"H_Anim_2.anm")
General_Anim_Filename(2,"H_Anim_3.anm")
General_Anim_Filename(3,"H_Anim_4.anm")

SetCopter("Copter" , 0 )
SetCopter("CivHuey" ,1 )
SetCopter("JetRanger" ,2 )
SetCopter("CivHuey" ,3 )
------------------------------------------------
-- Vheicle Stuff -------------------------------
------------------------------------------------
--Shine_Scale(x)

--Shadow_Scale(x)
------------------------------------------------
-- Dff's ---------------------------------------
------------------------------------------------
Clump_Filename( 0 , "trees.dff" )
Clump_Filename( 1 , "Jump.dff" )
Clump_Filename( 2 , "CamMan.dff" )

Clump_Exclude_From_World( 0 )
Clump_Exclude_From_World( 1 )

------------------------------------------------
-- Mts Stuff -----------------------------------
------------------------------------------------
--Mts_Filename(index,"filename.mts")

--Clump_Mts_Pair(clump_index,mts_index)
--Clump_Mts_Pair(clump_index,mts_index)
------------------------------------------------
-- ID VALUES FOR RW DFF'S ----------------------
------------------------------------------------
local COLUMN = 1000
local SIGN_2 = 2000
local BARREL_Y = 3000
local PLANK = 4000

local COLUMN_1 = 5000
local COLUMN_2 = 6000
local COLUMN_3 = 7000
local COLUMN_4 = 8000

--local COLUMN_FENCE_1 = 9000
--local COLUMN_FENCE_2 = 10000
--local COLUMN_FENCE_3 = 11000
--local COLUMN_FENCE_4 = 12000
-------------------------------------------------
-- RWP MATERIAL ID ------------------------------
-------------------------------------------------
local SURFACE = 0
local WOOD = 1
local WOODSNAP = 2
local METAL = 3
local RUBBER = 4
local CONCRETE = 5
local PLASTIC = 6
local FOLIAGE = 7
-------------------------------------------------
-- RWP OBJECT VALUES ----------------------------
-------------------------------------------------
local NO_FATNESS = 0.0
local GENERAL_FATNESS = 0.1
local BARREL_FATNESS =  0.25
local PLANK_FATNESS = 0.001

local BOUNCE = 0
local FRICTION = 1

------------------------------------------------
-- R W Phisics ---------------------------------
------------------------------------------------
-- 0
RWP_Object( 0 , "Stop Sign 2 " , "Sign02.dff" , "Sign02.mts" )
RWP_Object_Box( 0 , 4.275 , 1.54 , 0.21 )
RWP_Object_Properties( 0 , GENERAL_FATNESS , BOUNCE , FRICTION , 0.2 , WOOD )
RWP_Object_BaseLabel( 0 , SIGN_2 )
RWP_Object_ShatterVel( 0 , 6 )
-- 2
RWP_Object( 2 , "Barrel Yellow ", "YBarrel.dff", "YBarrel.mts" )
RWP_Object_Box( 2 , 0.77 , 1.28 , 0.77 )
RWP_Object_Properties( 2 , BARRE_FATNESS , BOUNCE , FRICTION , 1.2 , METAL )
RWP_Object_BaseLabel( 2 , BARREL_Y )
RWP_Object_ShatterVel( 2 , 2 )
-- 3
RWP_Object( 3 , "Wood Plank", "Plank01.dff", "Plank01.mts" )
RWP_Object_Box( 3 , 0.624 , 0.09 , 4.99 )
RWP_Object_Properties( 3 , PLANK_FATNESS , BOUNCE , FRICTION , 0.2 , WOOD )
RWP_Object_BaseLabel( 3 , PLANK )
RWP_Object_ShatterVel( 3 , 3 )

-- 5
RWP_Object( 5 , "Column", "Column.dff", "Column1.mts" )
RWP_Object_Capsule( 5 , 1.84 , 2.48  )
RWP_Object_Properties( 5 , NO_FATNESS , BOUNCE , FRICTION , 3 , CONCRETE )
RWP_Object_BaseLabel( 5 , COLUMN_1 )
RWP_Object_ShatterVel( 5 , 10 )
-- 6
RWP_Object( 6 , "Column", "Column.dff", "Column2.mts" )
RWP_Object_Capsule( 6 , 1.84 , 2.48  )
RWP_Object_Properties( 6 , NO_FATNESS , BOUNCE , FRICTION , 3 , CONCRETE )
RWP_Object_BaseLabel( 6 , COLUMN_2 )
RWP_Object_ShatterVel( 6 , 10 )
-- 7
RWP_Object( 7 , "Column", "Column.dff", "Column3.mts" )
RWP_Object_Capsule( 7 , 1.84 , 2.48  )
RWP_Object_Properties( 7 , NO_FATNESS , BOUNCE , FRICTION , 3 , CONCRETE )
RWP_Object_BaseLabel( 7 , COLUMN_3 )
RWP_Object_ShatterVel( 7 , 10 )
-- 8
RWP_Object( 8 , "Column", "Column.dff", "Column4.mts" )
RWP_Object_Capsule( 8 , 1.84 , 2.48  )
RWP_Object_Properties( 8 , NO_FATNESS , BOUNCE , FRICTION , 3 , CONCRETE )
RWP_Object_BaseLabel( 8 , COLUMN_4 )
RWP_Object_ShatterVel( 8 , 10 )

--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
--Triggers
--Need to work out what to do with these defines
local ON_BLAST = 0
local ON_SHATTER = 1
local ON_MOVE = 2

local IMMEDIATE = 0
local WAIT_FOR_NEW_POINT = 1

local JUMP_1_DISABLE = 10
local JUMP_1_ENABLE = 11

local JUMP_2_DISABLE = 20
local JUMP_2_ENABLE = 21

--Collapsing Column Scenarios---------------------------------------------------
local PUSH_COLUMN_1_FUNC = Physics_Scenario_Get_Function("EgyptPushColumn1");
local PUSH_COLUMN_2_FUNC = Physics_Scenario_Get_Function("EgyptPushColumn2");

local COLUMN_PAIR1 = Physics_Scenario_Create( "Column Pair 1" );
--Physics_Scenario_Add_Objects( COLUMN_PAIR1, 9 );
--Physics_Scenario_Add_Objects( COLUMN_PAIR1, 10 );

local COLUMN_PAIR1_TRIGGER = Physics_Scenario_Create_Trigger( COLUMN_PAIR1, ON_MOVE, IMMEDIATE );
Physics_Scenario_Trigger_Set_AI_Poly( COLUMN_PAIR1_TRIGGER, 0, 36 );

local COLUMN_PAIR2 = Physics_Scenario_Create( "Column Pair 2" );
--Physics_Scenario_Add_Objects( COLUMN_PAIR2, 11 );
--Physics_Scenario_Add_Objects( COLUMN_PAIR2, 12 );

local COLUMN_PAIR2_TRIGGER = Physics_Scenario_Create_Trigger( COLUMN_PAIR2, ON_MOVE, IMMEDIATE );
Physics_Scenario_Trigger_Set_AI_Poly( COLUMN_PAIR2_TRIGGER, 1, 36 );

Physics_Scenario_Set_Sequence( COLUMN_PAIR1, PUSH_COLUMN_1_FUNC, COLUMN_PAIR2, PUSH_COLUMN_2_FUNC );