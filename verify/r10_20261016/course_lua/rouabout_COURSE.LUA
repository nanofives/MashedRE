-- course.lua
-- Roundabout

Course_Id(2)
-------------------------------------------
-- WORLD STUFF ----------------------------
-------------------------------------------
Texture_Dictionary_Filename("textures.txd")

World_Bsp_Filename("graph.bsp")
AI_Bsp_Filename("ai.bsp")
Collision_Bsp_Filename("coll.bsp")
Spline_Filename( 2, "rail.spl")
Lights_Filename("lights.dff")
Sky_Filename("skydome.dff")

--Occluder_Filename("Occluder.dff")
--------------------------------------------
-- Copter Stuff ----------------------------
--------------------------------------------
General_Anim_Filename(0,"H_Anim_1.anm")
General_Anim_Filename(1,"H_Anim_2.anm")
General_Anim_Filename(2,"H_Anim_3.anm")
General_Anim_Filename(3,"H_Anim_4.anm")
General_Anim_Filename(4,"H_Anim_5.anm")
General_Anim_Filename(5,"KTC1.anm")

SetCopter("Copter" , 0 )
SetCopter("CivHuey" ,1 )
SetCopter("JetRanger" ,2 )
SetCopter("CivHuey" ,3 )
SetCopter("CivHuey" ,4 )
-------------------------------------------
-- Intro Cam ------------------------------
-------------------------------------------
--Camera_Anim_Filename("Intro_Cam.anm")

-------------------------------------------
-- Dff Files For Graphics -----------------
-------------------------------------------
Clump_Filename( 0 , "Shrub01.dff" )
Clump_Filename( 1 , "crane.dff" )
Clump_Filename( 2 , "railcar.dff" )
Clump_Filename( 3 , "FatTree.dff" )
Clump_Filename( 4 , "Gib.dff" )
Clump_Filename( 5 , "Girders.dff" )

Clump_Filename( 7 , "Convayers.dff" )
Clump_Filename( 8 , "Handrail.dff" )
Clump_Filename( 9 , "Hrail.dff" )
Clump_Filename( 10 , "Hut01.dff" )
Clump_Filename( 11 , "Girders02.dff" )

Clump_Filename( 14 , "CamMan.dff" )
Clump_Filename( 15 , "RailTrack.dff" )
Clump_Filename( 16 , "Ads.dff" )

Clump_Filename( 17 , "Pylon_H.dff" )
Clump_Filename( 18 , "Pylon_L.dff" )
Clump_Filename( 19 , "Frame_H.dff" )
Clump_Filename( 20 , "Frame_L.dff" )
----------------------------------------------------
-- Excludeing from World Due to transparant textures
----------------------------------------------------
Clump_Exclude_From_World(0)
Clump_Exclude_From_World(2)
Clump_Exclude_From_World(3)
Clump_Exclude_From_World(5)

Clump_Exclude_From_World(8)
Clump_Exclude_From_World(9)
Clump_Exclude_From_World(10)
Clump_Exclude_From_World(11)

Clump_Exclude_From_World(17)
Clump_Exclude_From_World(18)
Clump_Exclude_From_World(19)
Clump_Exclude_From_World(20)
-------------------------------------------
-- Mts Files For Graphics -----------------
-------------------------------------------
Mts_Filename( 0 , "Shrub01.mts" )
Mts_Filename( 1 , "smoke.mts" )
Mts_Filename( 2 , "FatTree.mts" )
Mts_Filename( 3 , "Pylon.mts" )
Mts_Filename( 4 , "Frame.mts" )
-------------------------------------------
-- Dff & Mts File Matching command --------
-------------------------------------------
Clump_Mts_Pair( 0 , 0 ) 
Clump_Mts_Pair( 3 , 2 )
Clump_Mts_Pair( 17 , 3 )
Clump_Mts_Pair( 19 , 4 )
-------------------------------------------
-- Vehicle Commands -----------------------
-------------------------------------------
Vehicle_Shininess_Range( 0.5, 0.8 )
-------------------------------------------
-- ID VALUES FOR RW DFF'S -----------------
-------------------------------------------
--local DESK = 1000
local HUT = 2000
local HUT_WALL_1 = 3000
local HUT_WALL_2 = 4000
local HUT_WALL_3 = 5000
local HUT_WALL_4 = 6000
--local HUT_WALL_5 = 7000
local PLANK_1 = 8000
--local PLANK_2 = 9000
--local PLANK_3 = 10000
--local PLANK_4 = 11000
--local PLANK_5 = 12000
--local PLANK_6 = 13000
--local PLANK_7 = 14000
--local PLANK_8 = 15000
local WHITE_TYRE = 16000
local RED_TYRE = 17000
local CRATE = 18000
local SIGN_1 = 19000
local SIGN_2 = 20000
local TRUCK_TYRE = 21000
--local DOZER = 22000
local HUT_SIGNS = 23000
local BARREL_RED = 150000
-------------------------------------------
-- RWP MATERIAL ID ------------------------
-------------------------------------------
local SURFACE = 0
local WOOD = 1
local WOODSNAP = 2
local METAL = 3
local RUBBER = 4
local CONCRETE = 5
local PLASTIC = 6
local FOLIAGE = 7
-------------------------------------------
-- RWP OBJECT VALUES ----------------------
-------------------------------------------
local NO_FATNESS = 0.0
local GENERAL_FATNESS = 0.1
local SMALL_FATNESS = 0.05
local TYRE_FATNESS = 0.25
local BOUNCE = 0
local FRICTION = 1
-------------------------------------------------
-- RENDER WARE PHYSICS COMMANDS
-------------------------------------------------

-- 0
RWP_Object( 0 , "White Crash Tyres" , "WhiteTyre.dff" , "WhiteTyre.mts" )
RWP_Object_Capsule( 0 , 1.25 , 0.45 )  
RWP_Object_Properties( 0 , NO_FATNESS , BOUNCE , FRICTION , 0.5 , RUBBER )
RWP_Object_BaseLabel( 0 , WHITE_TYRE )
RWP_Object_ShatterVel( 0 , 100 )

-- RWP_Object_CentreOfMassOffset(index,x,y,z)

-- 1
RWP_Object( 1 , "Red Crash Tyres" , "RedTyre.dff" , "RedTyre.mts" )
RWP_Object_Capsule( 1 , 1.25 , 0.45 )
RWP_Object_Properties( 1 , NO_FATNESS , BOUNCE , FRICTION , 0.5 , RUBBER )
RWP_Object_BaseLabel( 1 , RED_TYRE )
RWP_Object_ShatterVel( 1 , 100 )
-- 2
RWP_Object( 2 , "Crate Type 1" , "Crate01.dff" , "Crate01.mts" )
RWP_Object_Box( 2 , 1.4 , 1.4 , 1.4 )
RWP_Object_Properties( 2 , GENERAL_FATNESS , BOUNCE , FRICTION , 0.2 , WOOD )
RWP_Object_BaseLabel( 2 , CRATE )
RWP_Object_ShatterVel( 2 , 5 )
-- 3
RWP_Object( 3 , "Sign Type 1" , "Sign01.dff" , "Sign01.mts" )
RWP_Object_Box( 3 , 2.01 , 2.4 , 0.06 )
RWP_Object_Properties( 3 , GENERAL_FATNESS , BOUNCE , FRICTION , 0.1 , WOOD )
RWP_Object_BaseLabel( 3 , SIGN_1 )
RWP_Object_ShatterVel( 3 , 4 )
-- 4
RWP_Object( 4 , "Stop Sign 2 " , "Sign02.dff" , "Sign02.mts" )
RWP_Object_Box( 4 , 4.275 , 1.54 , 0.21 )
RWP_Object_Properties( 4 , GENERAL_FATNESS , BOUNCE , FRICTION , 0.2 , WOOD )
RWP_Object_BaseLabel( 4 , SIGN_2 )
RWP_Object_ShatterVel( 4 , 6 )
-- 5
RWP_Object( 5, "Truck Tyres", "Trucktyre.dff", "Trucktyre.mts" )
RWP_Object_Box( 5, 2.02, 0.445, 2.02 )
RWP_Object_Properties( 5, TYRE_FATNESS, BOUNCE, FRICTION, 0.5 , RUBBER )
RWP_Object_BaseLabel( 5, TRUCK_TYRE )
RWP_Object_ShatterVel( 5, 100 )
-- 6
--RWP_Object( 6 , "Bulldozer type 2" , "Dozer02.dff" , "Dozer02.mts" )
--RWP_Object_Box( 6 , 4.17 , 3.995 , 8.05 )
--RWP_Object_Properties( 6 , GENERAL_FATNESS , BOUNCE , FRICTION , 5.0 , METAL )
--RWP_Object_BaseLabel( 6 , DOZER )
RWP_Object_ShatterVel( 6 , 100 )
-- 7
RWP_Object( 7 , "Hut" , "Hut01.dff" , "Hut01.mts" )
RWP_Object_Box( 7 , 4.27 , 3.36 , 6.14 )
RWP_Object_Properties( 7 , GENERAL_FATNESS , BOUNCE , FRICTION , 9.0 , WOOD )
RWP_Object_BaseLabel( 7 , HUT )
RWP_Object_ShatterVel( 7 , 0.5 )
------------------------------------------------------------------
-- Destroyed Hut Bits --------------------------------------------
------------------------------------------------------------------
-- 8
--RWP_Object( 8 , "Desk" , "Desk.dff" , "Desk.mts" )
--RWP_Object_Box( 8 , 0.857 , 1.12 , 1.908 )
--RWP_Object_Properties( 8 , GENERAL_FATNESS , BOUNCE , FRICTION , 0.1 , WOOD )
--RWP_Object_BaseLabel( 8 , DESK )
--RWP_Object_ShatterVel( 8 , 4 )
-- 9
RWP_Object( 9 , "Wall01" , "Wall01.dff" , "Wall01.mts" )
RWP_Object_Box( 9 , 4.31 , 2.75 , 0.05 )
RWP_Object_Properties( 9 , SMALL_FATNESS , BOUNCE , FRICTION , 0.15 , WOOD )
RWP_Object_BaseLabel( 9 , HUT_WALL_1 )
RWP_Object_ShatterVel(9 , 4 )
-- 10
RWP_Object( 10 , "Wall02" , "Wall02.dff" , "Wall02.mts" )
RWP_Object_Box( 10 , 6.19 , 2.75 , 0.05 )
RWP_Object_Properties( 10 , SMALL_FATNESS , BOUNCE , FRICTION , 0.15 , WOOD )
RWP_Object_BaseLabel( 10 , HUT_WALL_2 )
RWP_Object_ShatterVel( 10 , 4 )
-- 11
RWP_Object( 11 , "Wall03" , "Wall03.dff" , "Wall03.mts" )
RWP_Object_Box( 11 , 4.48 , 2.75 , 0.05 )
RWP_Object_Properties( 11 , SMALL_FATNESS , BOUNCE , FRICTION , 0.15 , WOOD )
RWP_Object_BaseLabel( 11 , HUT_WALL_3 )
RWP_Object_ShatterVel( 11 , 4 )
-- 12
RWP_Object( 12 , "Wall04" , "Wall04.dff" , "Wall04.mts" )
RWP_Object_Box( 12 , 2.1 , 2.35 , 0.05 )
RWP_Object_Properties( 12 , SMALL_FATNESS , BOUNCE , FRICTION , 0.15 , WOOD )
RWP_Object_BaseLabel( 12 , HUT_WALL_4 )
RWP_Object_ShatterVel( 12 , 4 )
-- 13
--RWP_Object( 13 , "Wall05" , "Wall05.dff" , "Wall05.mts" )
--RWP_Object_Box( 13 , 0.57 , 1.75 , 0.05 )
--RWP_Object_Properties( 13 , SMALL_FATNESS , BOUNCE , FRICTION , 0.15 , WOOD )
--RWP_Object_BaseLabel( 13 , HUT_WALL_5 )
--RWP_Object_ShatterVel( 13 , 4 )
-----------------------------------------------------------------
-- Bridge Bits
-- 14
RWP_Object( 14 , "plank 1" , "Bridge.dff" , "Bridge01.mts" )
RWP_Object_Box( 14 , 3.16 , 0.73 , 5.07 )
RWP_Object_Properties( 14 , SMALL_FATNESS , BOUNCE , FRICTION , 0.3 , SURFACE )
RWP_Object_BaseLabel( 14 , PLANK_1 )
RWP_Object_ShatterVel( 14 , 100 )

-----------------------------------------------------------------

-- 22
RWP_Object( 22 , "Barrel Red" , "RBarrel.dff" , "RBarrel.mts" )
RWP_Object_Box( 22 , 0.77 , 1.28 , 0.77 )
RWP_Object_Properties( 22 , TYRE_FATNESS , BOUNCE , FRICTION , 1.5 , METAL )
RWP_Object_BaseLabel( 22 , BARREL_RED )
RWP_Object_ShatterVel( 22 , 2 )
-- 23
RWP_Object( 23 , "Sign Type 2" , "Sign02.dff" , "Hut_Signs.mts" )
RWP_Object_Box( 23 , 4.275 , 1.54 , 0.21 )
RWP_Object_Properties( 23 , GENERAL_FATNESS , BOUNCE , FRICTION , 0.1 , WOOD )
RWP_Object_BaseLabel( 23 , HUT_SIGNS )
RWP_Object_ShatterVel( 23 , 4 )

------------------------------------------------------------------
-- Hut Scenarios -------------------------------------------------
------------------------------------------------------------------
local HUT_SCENARIO = Physics_Scenario_Create( "Hut Scenario" );
Physics_Scenario_Add_Objects( HUT_SCENARIO, 7 );

local HUT_DESTROYED_SCENARIO = Physics_Scenario_Create( "Hut Destroyed" );
--Physics_Scenario_Add_Objects( HUT_DESTROYED_SCENARIO, 8 );
Physics_Scenario_Add_Objects( HUT_DESTROYED_SCENARIO, 9 );
Physics_Scenario_Add_Objects( HUT_DESTROYED_SCENARIO, 10 );
Physics_Scenario_Add_Objects( HUT_DESTROYED_SCENARIO, 11 );
Physics_Scenario_Add_Objects( HUT_DESTROYED_SCENARIO, 12 );
--Physics_Scenario_Add_Objects( HUT_DESTROYED_SCENARIO, 13 );

local HUT_SIGNS_SCENARIO = Physics_Scenario_Create( "Hut Signs" );
Physics_Scenario_Add_Objects( HUT_SIGNS_SCENARIO, 23 );

local START_CRANE_ANIM_FUNC = Physics_Scenario_Get_Function("RoundaboutStartCrane");
local STOP_CRANE_ANIM_FUNC = Physics_Scenario_Get_Function("RoundaboutStopCrane");

--Sequences
Physics_Scenario_Set_Sequence( HUT_SCENARIO, START_CRANE_ANIM_FUNC, HUT_DESTROYED_SCENARIO, HUT_DESTROYED_SCENARIO, STOP_CRANE_ANIM_FUNC, HUT_SIGNS_SCENARIO );

--Triggers
--Need to work out what to do with these defines
local ON_BLAST = 0
local ON_SHATTER = 1
local ON_MOVE = 2

local IMMEDIATE = 0
local WAIT_FOR_NEW_POINT = 1

local HUT_TRIGGER1 = Physics_Scenario_Create_Trigger( HUT_SCENARIO, ON_BLAST, IMMEDIATE );
Physics_Scenario_Trigger_Add_Objects( HUT_TRIGGER1, 7, 100 );

local HUT_TRIGGER2 = Physics_Scenario_Create_Trigger( HUT_SCENARIO, ON_SHATTER, IMMEDIATE );
Physics_Scenario_Trigger_Add_Objects( HUT_TRIGGER2, 7, 100 );

local HUT_DESTROYED_TRIGGER1 = Physics_Scenario_Create_Trigger( HUT_DESTROYED_SCENARIO, ON_SHATTER, WAIT_FOR_NEW_POINT );
--Physics_Scenario_Trigger_Add_Objects( HUT_DESTROYED_TRIGGER1, 8, 100.0 );
Physics_Scenario_Trigger_Add_Objects( HUT_DESTROYED_TRIGGER1, 9, 100.0 );
Physics_Scenario_Trigger_Add_Objects( HUT_DESTROYED_TRIGGER1, 10, 100.0 );
Physics_Scenario_Trigger_Add_Objects( HUT_DESTROYED_TRIGGER1, 11, 100.0 );
Physics_Scenario_Trigger_Add_Objects( HUT_DESTROYED_TRIGGER1, 12, 100.0 );
--Physics_Scenario_Trigger_Add_Objects( HUT_DESTROYED_TRIGGER1, 13, 100.0 );

local HUT_SIGNS_TRIGGER1 = Physics_Scenario_Create_Trigger( HUT_SIGNS_SCENARIO, ON_SHATTER, WAIT_FOR_NEW_POINT );
Physics_Scenario_Trigger_Add_Objects( HUT_SIGNS_TRIGGER1, 23, 100.0 );