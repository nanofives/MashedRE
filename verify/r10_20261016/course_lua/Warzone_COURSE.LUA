-- course.lua
-- WARZONE

Course_Id(33)
-------------------------------------------
-- WORLD STUFF ----------------------------
-------------------------------------------
Texture_Dictionary_Filename("Warzone.txd")
World_Bsp_Filename("Graphics.bsp")
AI_Bsp_Filename("Ai1.bsp")
Collision_Bsp_Filename("Collide.bsp")
Lights_Filename("Lights.dff")
Setup_Fog( 0 , 80 , 106 , 106 , 106 )
Sky_Filename( 0 ,"SkyDome.dff")
Sky_Filename( 1 ,"HorDome.dff")

--Occluder_Filename("Occluder.dff")
--------------------------------------------
-- Intro Fly in ----------------------------
--------------------------------------------
--Camera_Anim_Filename("Intro_Cam.anm")

--------------------------------------------
-- Anim's ----------------------------------
--------------------------------------------
-- Anim for the battel platform
General_Anim_Filename(0,"BoatAnim.anm")
--------------------------------------------
-- Copter Stuff ----------------------------
--------------------------------------------
General_Anim_Filename(1,"H_Anim_1.anm")
General_Anim_Filename(2,"H_Anim_2.anm")
General_Anim_Filename(3,"H_Anim_3.anm")
General_Anim_Filename(4,"H_Anim_4.anm")
General_Anim_Filename(5,"H_Anim_5.anm")

SetCopter("Copter" , 1 )
SetCopter("Copter" ,2 )
SetCopter("Copter" ,3 )
SetCopter("CivHuey" ,4 )
SetCopter("JetRanger" ,5 )
--------------------------------------------
-- Dff's -----------------------------------
--------------------------------------------
Clump_Filename( 0 ,"River.dff")
Clump_Filename( 1 ,"Boat.dff")
Clump_Filename( 2 ,"CrossTree.dff")
Clump_Filename( 3 ,"OakTrunk.dff")
Clump_Filename( 6 ,"CamMan.dff")

--Clump_Filename( 8 ,"SkyLine.dff")
Clump_Filename( 9 ,"Crater_1.dff")
Clump_Filename( 10 ,"Crater_2.dff")
Clump_Filename( 11 ,"Crater_3.dff")
Clump_Filename( 12 ,"Crater_4.dff")
Clump_Filename( 13 ,"Crater_5.dff")
Clump_Filename( 14 ,"Crater_6.dff")
Clump_Filename( 15 ,"Crater_7.dff")
Clump_Filename( 16 ,"Crater_8.dff")
Clump_Filename( 17 ,"Chunk.dff")

Clump_Filename( 18 ,"Ads.dff")
Clump_Filename( 19 ,"Fence_1.dff")
Clump_Filename( 20 ,"Pylon_H.dff")
Clump_Filename( 21 ,"Pylon_L.dff")
Clump_Filename( 22 ,"Fence_2.dff")


Clump_Exclude_From_World(0)
Clump_Exclude_From_World(1)
Clump_Exclude_From_World(2)
Clump_Exclude_From_World(3)

Clump_Exclude_From_World(9)
Clump_Exclude_From_World(10)
Clump_Exclude_From_World(11)
Clump_Exclude_From_World(12)
Clump_Exclude_From_World(13)
Clump_Exclude_From_World(14)
Clump_Exclude_From_World(15)
Clump_Exclude_From_World(16)
Clump_Exclude_From_World(17)

Clump_Exclude_From_World(19)
Clump_Exclude_From_World(20)
Clump_Exclude_From_World(21)
Clump_Exclude_From_World(22)
--------------------------------------------
-- Mts Files For Graphics ------------------
--------------------------------------------
Mts_Filename(0,"Crosstrees.mts")

Mts_Filename(1,"OakLeaf.mts")
Mts_Filename(2,"OakTrees_1.mts")
Mts_Filename(3,"OakTrees_2.mts")
Mts_Filename(4,"Guns.mts")
Mts_Filename(5,"Pylon.mts")
Mts_Filename(6,"fence_2.mts")
-------------------------------------------------------
-- Dff & Mts File Matching command --------------------
-------------------------------------------------------
Clump_Mts_Pair( 20 , 5 )  
-------------------------------------------
-- ID VALUES FOR RW DFF'S -----------------
-------------------------------------------
local TOWER = 1000
local DECK = 2000
local HSTONE = 3000
local CROSS_STONE = 4000
local BLOCK = 5000
local HUT_A = 6000
local HUT_B = 7000
local HUT_C = 8000
local HUT_WALL_1_A = 9000
local HUT_WALL_2_A = 10000
local HUT_WALL_3_A = 11000
local HUT_WALL_4_A = 12000
local HUT_WALL_1_B = 13000
local HUT_WALL_2_B = 14000
local HUT_WALL_3_B = 15000
local HUT_WALL_4_B = 16000
local HUT_WALL_1_C = 17000
local HUT_WALL_2_C = 18000
local HUT_WALL_3_C = 19000
local HUT_WALL_4_C = 20000
local POST = 21000

local BARREL_RED = 150000
-------------------------------------------
-- RWP OBJECT VALUES ----------------------
-------------------------------------------
local NO_FATNESS = 0
local GENERAL_FATNESS = 0.1
local SMALL_FATNESS = 0.05
local BOUNCE = 0
local FRICTION = 1
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
--------------------------------------------
-- RENDER WARE PHYSICS COMMANDS ------------
--------------------------------------------
-- 0
RWP_Object( 0, "Pontoon Tower" ,"Tower.dff", "Tower.mts" )
RWP_Object_Box( 0, 4.9 ,2.045, 14.15) 
RWP_Object_Properties( 0, GENERAL_FATNESS, BOUNCE, FRICTION, 200.0 , METAL )
RWP_Object_BaseLabel( 0, TOWER )
RWP_Object_ShatterVel( 0, 200 )

-- RWP_Object_CentreOfMassOffset(index,x,y,z)

-- 1
RWP_Object( 1, "Pontoon Deck", "Deck.dff", "Deck.mts")
RWP_Object_Box( 1, 39.9, 0.9, 39.9 )
RWP_Object_Properties( 1, GENERAL_FATNESS, BOUNCE, FRICTION, 2000.0 , METAL )
RWP_Object_BaseLabel( 1, DECK )
RWP_Object_ShatterVel( 1, 200 )
-- 2
RWP_Object( 2, "Head Stones", "Hstone.dff", "Hstone.mts")
RWP_Object_Box( 2, 0.71, 1.54, 0.14 )
RWP_Object_Properties( 2, SMALL_FATNESS, BOUNCE, FRICTION, 0.5 , CONCRETE )
RWP_Object_BaseLabel( 2, HSTONE )
RWP_Object_ShatterVel( 2, 10 )
-- 3
RWP_Object( 3, "Cross Stones", "Cross.dff", "Cross.mts")
RWP_Object_Box( 3, 0.15, 1.35, 0.71 )
RWP_Object_Properties( 3, SMALL_FATNESS, BOUNCE, FRICTION, 0.5 , CONCRETE )
RWP_Object_BaseLabel( 3, CROSS_STONE )
RWP_Object_ShatterVel( 3, 10 )

-- 4
RWP_Object( 4 , " Maconry Blocks", "Block.dff", "Blocks.mts")
RWP_Object_Capsule( 4 , 1.05, 0.56 )
RWP_Object_Properties( 4 , NO_FATNESS , BOUNCE , FRICTION , 0.4 , CONCRETE )
RWP_Object_BaseLabel( 4 , BLOCK )
RWP_Object_ShatterVel( 4 , 2 )
--------------------------------------------
-- Hut Stuf --------------------------------
--------------------------------------------
-- 5
RWP_Object( 5, "Hut A", "Hut01.dff", "Hut01_A.mts" )
RWP_Object_Box( 5, 4.27, 3.36, 6.14 )
RWP_Object_Properties( 5, GENERAL_FATNESS, BOUNCE, FRICTION, 9.0 , WOOD )
RWP_Object_BaseLabel( 5, HUT_A )
RWP_Object_ShatterVel( 5, 0.5 )
-- 6
RWP_Object( 6, "Hut B", "Hut01.dff", "Hut01_B.mts" )
RWP_Object_Box( 6, 4.27, 3.36, 6.14 )
RWP_Object_Properties( 6, GENERAL_FATNESS, BOUNCE, FRICTION, 9.0 , WOOD )
RWP_Object_BaseLabel( 6, HUT_B )
RWP_Object_ShatterVel( 6, 0.5 )
-- 7
RWP_Object( 7, "Hut C", "Hut01.dff", "Hut01_C.mts" )
RWP_Object_Box( 7, 4.27, 3.36, 6.14 )
RWP_Object_Properties( 7, GENERAL_FATNESS, BOUNCE, FRICTION, 9.0 , WOOD )
RWP_Object_BaseLabel( 7, HUT_C )
RWP_Object_ShatterVel( 7, 0.5 )
-------------------------------------------------------------------------
-- Destroyed Hut Bits ---------------------------------------------------
-------------------------------------------------------------------------
-- HUT A
-- 8
RWP_Object( 8,"Wall01 A", "Wall01.dff", "Wall01_A.mts" )
RWP_Object_Box( 8, 4.31, 2.75, 0.05 )
RWP_Object_Properties( 8, SMALL_FATNESS, BOUNCE, FRICTION, 0.15 , WOOD )
RWP_Object_BaseLabel( 8, HUT_WALL_1_A )
RWP_Object_ShatterVel(8, 4 )
-- 9
RWP_Object( 9,"Wall02 A", "Wall02.dff", "Wall02_A.mts" )
RWP_Object_Box( 9, 6.19, 2.75, 0.05 )
RWP_Object_Properties( 9, SMALL_FATNESS, BOUNCE, FRICTION, 0.15 , WOOD )
RWP_Object_BaseLabel( 9, HUT_WALL_2_A )
RWP_Object_ShatterVel( 9, 4 )
-- 10
RWP_Object( 10,"Wall03 A", "Wall03.dff", "Wall03_A.mts" )
RWP_Object_Box( 10, 4.48, 2.75, 0.05 )
RWP_Object_Properties( 10, SMALL_FATNESS, BOUNCE, FRICTION, 0.15 , WOOD )
RWP_Object_BaseLabel( 10, HUT_WALL_3_A )
RWP_Object_ShatterVel( 10, 4 )
-- 11
RWP_Object( 11,"Wall04 A", "Wall04.dff", "Wall04_A.mts" )
RWP_Object_Box( 11, 2.1, 2.35, 0.05 )
RWP_Object_Properties( 11, SMALL_FATNESS, BOUNCE, FRICTION, 0.15 , WOOD )
RWP_Object_BaseLabel( 11, HUT_WALL_4_A )
RWP_Object_ShatterVel( 11, 4 )
-- HUT B -----------------------------------------------------------------
-- 12
RWP_Object( 12,"Wall01 B", "Wall01.dff", "Wall01_B.mts" )
RWP_Object_Box( 12, 4.31, 2.75, 0.05 )
RWP_Object_Properties( 12, SMALL_FATNESS, BOUNCE, FRICTION, 0.15 , WOOD )
RWP_Object_BaseLabel( 12, HUT_WALL_1_B )
RWP_Object_ShatterVel(12, 4 )
-- 13
RWP_Object( 13,"Wall02 B", "Wall02.dff", "Wall02_B.mts" )
RWP_Object_Box( 13, 6.19, 2.75, 0.05 )
RWP_Object_Properties( 13, SMALL_FATNESS, BOUNCE, FRICTION, 0.15 , WOOD )
RWP_Object_BaseLabel( 13, HUT_WALL_2_B )
RWP_Object_ShatterVel( 13, 4 )
-- 14
RWP_Object( 14,"Wall03 B", "Wall03.dff", "Wall03_B.mts" )
RWP_Object_Box( 14, 4.48, 2.75, 0.05 )
RWP_Object_Properties( 14, SMALL_FATNESS, BOUNCE, FRICTION, 0.15 , WOOD )
RWP_Object_BaseLabel( 14, HUT_WALL_3_B )
RWP_Object_ShatterVel( 14, 4 )
-- 15
RWP_Object( 15,"Wall04 B", "Wall04.dff", "Wall04_B.mts" )
RWP_Object_Box( 15, 2.1, 2.35, 0.05 )
RWP_Object_Properties( 15, SMALL_FATNESS, BOUNCE, FRICTION, 0.15 , WOOD )
RWP_Object_BaseLabel( 15, HUT_WALL_4_B )
RWP_Object_ShatterVel( 15, 4 )
-- HUT C -----------------------------------------------------------------
-- 16
RWP_Object( 16,"Wall01 C", "Wall01.dff", "Wall01_C.mts" )
RWP_Object_Box( 16, 4.31, 2.75, 0.05 )
RWP_Object_Properties( 16, SMALL_FATNESS, BOUNCE, FRICTION, 0.15 , WOOD )
RWP_Object_BaseLabel( 16, HUT_WALL_1_C )
RWP_Object_ShatterVel(16, 4 )
-- 17
RWP_Object( 17,"Wall02 C", "Wall02.dff", "Wall02_C.mts" )
RWP_Object_Box( 17, 6.19, 2.75, 0.05 )
RWP_Object_Properties( 17, SMALL_FATNESS, BOUNCE, FRICTION, 0.15 , WOOD )
RWP_Object_BaseLabel( 17, HUT_WALL_2_C )
RWP_Object_ShatterVel( 17, 4 )
-- 18
RWP_Object( 18,"Wall03 C", "Wall03.dff", "Wall03_C.mts" )
RWP_Object_Box( 18, 4.48, 2.75, 0.05 )
RWP_Object_Properties( 18, SMALL_FATNESS, BOUNCE, FRICTION, 0.15 , WOOD )
RWP_Object_BaseLabel( 18, HUT_WALL_3_C )
RWP_Object_ShatterVel( 18, 4 )
-- 19
RWP_Object( 19,"Wall04 C", "Wall04.dff", "Wall04_C.mts" )
RWP_Object_Box( 19, 2.1, 2.35, 0.05 )
RWP_Object_Properties( 19, SMALL_FATNESS, BOUNCE, FRICTION, 0.15 , WOOD )
RWP_Object_BaseLabel( 19, HUT_WALL_4_C )
RWP_Object_ShatterVel( 19, 4 )
--------------------------------------------------------------------------
-- Barrel ----------------------------------------------------------------
--------------------------------------------------------------------------
-- 20
RWP_Object( 20, "Barrel Red", "RBarrel.dff", "RBarrel.mts" )
RWP_Object_Cylinder( 20 , 1.02 , 1.53 )
RWP_Object_Properties( 10 , NO_FATNESS , BOUNCE , FRICTION , 1.5 , METAL )
RWP_Object_BaseLabel( 20 , BARREL_RED )
RWP_Object_ShatterVel( 20 , 2 )
--------------------------------------------------------------------------
-- POST ------------------------------------------------------------------
--------------------------------------------------------------------------
-- 21
RWP_Object( 21,"POST", "Post_A.dff", "Post_A.mts" )
RWP_Object_Box( 21, 0.58, 1.85, 0.58 )
RWP_Object_Properties( 21, SMALL_FATNESS, BOUNCE, FRICTION, 0.3 , METAL )
RWP_Object_BaseLabel( 21, POST )
RWP_Object_ShatterVel( 21, 3 )
--------------------------------------------------------------------------
-- Hut Scenarios ---------------------------------------------------------
--------------------------------------------------------------------------
local ON_BLAST = 0
local ON_SHATTER = 1
local ON_MOVE = 2

local IMMEDIATE = 0
local WAIT_FOR_NEW_POINT = 1
-------------------------------------------------------------------------------------------------------------------------
-- HUT A ----------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------
local HUT_A_SCENARIO = Physics_Scenario_Create( "Hut_A Scenario" );
Physics_Scenario_Add_Objects( HUT_A_SCENARIO, 5 );

local HUT_A_DESTROYED_SCENARIO = Physics_Scenario_Create( "Hut_A Destroyed" );

Physics_Scenario_Add_Objects( HUT_A_DESTROYED_SCENARIO, 8 );
Physics_Scenario_Add_Objects( HUT_A_DESTROYED_SCENARIO, 9 );
Physics_Scenario_Add_Objects( HUT_A_DESTROYED_SCENARIO, 10 );
Physics_Scenario_Add_Objects( HUT_A_DESTROYED_SCENARIO, 11 );


--local HUT_A_SIGNS_SCENARIO = Physics_Scenario_Create( "Hut Signs" );
--Physics_Scenario_Add_Objects( HUT_A_SIGNS_SCENARIO, 23 );

--Sequences
Physics_Scenario_Set_Sequence( HUT_A_SCENARIO, HUT_A_DESTROYED_SCENARIO, HUT_A_DESTROYED_SCENARIO );


local HUT_A_TRIGGER1 = Physics_Scenario_Create_Trigger( HUT_A_SCENARIO, ON_BLAST, IMMEDIATE );
Physics_Scenario_Trigger_Add_Objects( HUT_A_TRIGGER1, 5, 100 );

local HUT_A_TRIGGER2 = Physics_Scenario_Create_Trigger( HUT_A_SCENARIO, ON_SHATTER, IMMEDIATE );
Physics_Scenario_Trigger_Add_Objects( HUT_A_TRIGGER2, 5, 100 );


local HUT_A_DESTROYED_TRIGGER1 = Physics_Scenario_Create_Trigger( HUT_A_DESTROYED_SCENARIO, ON_SHATTER, WAIT_FOR_NEW_POINT );
Physics_Scenario_Trigger_Add_Objects( HUT_A_DESTROYED_TRIGGER1, 8, 100.0 );
Physics_Scenario_Trigger_Add_Objects( HUT_A_DESTROYED_TRIGGER1, 9, 100.0 );
Physics_Scenario_Trigger_Add_Objects( HUT_A_DESTROYED_TRIGGER1, 10, 100.0 );
Physics_Scenario_Trigger_Add_Objects( HUT_A_DESTROYED_TRIGGER1, 11, 100.0 );


--local HUT_A_SIGNS_TRIGGER1 = Physics_Scenario_Create_Trigger( HUT_A_SIGNS_SCENARIO, ON_SHATTER, WAIT_FOR_NEW_POINT );
--Physics_Scenario_Trigger_Add_Objects( HUT_A_SIGNS_TRIGGER1, 23, 100.0 );
--------------------------------------------------------------------------------------------------------------------------
-- HUT B -----------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------
local HUT_B_SCENARIO = Physics_Scenario_Create( "Hut_B Scenario" );
Physics_Scenario_Add_Objects( HUT_B_SCENARIO, 6 );

local HUT_B_DESTROYED_SCENARIO = Physics_Scenario_Create( "Hut_B Destroyed" );

Physics_Scenario_Add_Objects( HUT_B_DESTROYED_SCENARIO, 12 );
Physics_Scenario_Add_Objects( HUT_B_DESTROYED_SCENARIO, 13 );
Physics_Scenario_Add_Objects( HUT_B_DESTROYED_SCENARIO, 14 );
Physics_Scenario_Add_Objects( HUT_B_DESTROYED_SCENARIO, 15 );


--local HUT_B_SIGNS_SCENARIO = Physics_Scenario_Create( "Hut Signs" );
--Physics_Scenario_Add_Objects( HUT_B_SIGNS_SCENARIO, 23 );

--Sequences
Physics_Scenario_Set_Sequence( HUT_B_SCENARIO, HUT_B_DESTROYED_SCENARIO, HUT_B_DESTROYED_SCENARIO );


local HUT_B_TRIGGER1 = Physics_Scenario_Create_Trigger( HUT_B_SCENARIO, ON_BLAST, IMMEDIATE );
Physics_Scenario_Trigger_Add_Objects( HUT_B_TRIGGER1, 6, 100 );

local HUT_B_TRIGGER2 = Physics_Scenario_Create_Trigger( HUT_B_SCENARIO, ON_SHATTER, IMMEDIATE );
Physics_Scenario_Trigger_Add_Objects( HUT_B_TRIGGER2, 6, 100 );


local HUT_B_DESTROYED_TRIGGER1 = Physics_Scenario_Create_Trigger( HUT_B_DESTROYED_SCENARIO, ON_SHATTER, WAIT_FOR_NEW_POINT );
Physics_Scenario_Trigger_Add_Objects( HUT_B_DESTROYED_TRIGGER1, 12, 100.0 );
Physics_Scenario_Trigger_Add_Objects( HUT_B_DESTROYED_TRIGGER1, 13, 100.0 );
Physics_Scenario_Trigger_Add_Objects( HUT_B_DESTROYED_TRIGGER1, 14, 100.0 );
Physics_Scenario_Trigger_Add_Objects( HUT_B_DESTROYED_TRIGGER1, 15, 100.0 );


--local HUT_B_SIGNS_TRIGGER1 = Physics_Scenario_Create_Trigger( HUT_B_SIGNS_SCENARIO, ON_SHATTER, WAIT_FOR_NEW_POINT );
--Physics_Scenario_Trigger_Add_Objects( HUT_B_SIGNS_TRIGGER1, 23, 100.0 );

--------------------------------------------------------------------------------------------------------------------------
-- HUT C -----------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------
local HUT_C_SCENARIO = Physics_Scenario_Create( "Hut_C Scenario" );
Physics_Scenario_Add_Objects( HUT_C_SCENARIO, 7 );

local HUT_C_DESTROYED_SCENARIO = Physics_Scenario_Create( "Hut_C Destroyed" );

Physics_Scenario_Add_Objects( HUT_B_DESTROYED_SCENARIO, 16 );
Physics_Scenario_Add_Objects( HUT_B_DESTROYED_SCENARIO, 17 );
Physics_Scenario_Add_Objects( HUT_B_DESTROYED_SCENARIO, 18 );
Physics_Scenario_Add_Objects( HUT_B_DESTROYED_SCENARIO, 19 );


--local HUT_C_SIGNS_SCENARIO = Physics_Scenario_Create( "Hut Signs" );
--Physics_Scenario_Add_Objects( HUT_C_SIGNS_SCENARIO, 23 );

--Sequences
Physics_Scenario_Set_Sequence( HUT_C_SCENARIO, HUT_C_DESTROYED_SCENARIO, HUT_C_DESTROYED_SCENARIO );


local HUT_C_TRIGGER1 = Physics_Scenario_Create_Trigger( HUT_C_SCENARIO, ON_BLAST, IMMEDIATE );
Physics_Scenario_Trigger_Add_Objects( HUT_C_TRIGGER1, 7, 100 );

local HUT_C_TRIGGER2 = Physics_Scenario_Create_Trigger( HUT_C_SCENARIO, ON_SHATTER, IMMEDIATE );
Physics_Scenario_Trigger_Add_Objects( HUT_C_TRIGGER2, 7, 100 );


local HUT_B_DESTROYED_TRIGGER1 = Physics_Scenario_Create_Trigger( HUT_C_DESTROYED_SCENARIO, ON_SHATTER, WAIT_FOR_NEW_POINT );
Physics_Scenario_Trigger_Add_Objects( HUT_C_DESTROYED_TRIGGER1, 16, 100.0 );
Physics_Scenario_Trigger_Add_Objects( HUT_C_DESTROYED_TRIGGER1, 17, 100.0 );
Physics_Scenario_Trigger_Add_Objects( HUT_C_DESTROYED_TRIGGER1, 18, 100.0 );
Physics_Scenario_Trigger_Add_Objects( HUT_C_DESTROYED_TRIGGER1, 19, 100.0 );


--local HUT_C_SIGNS_TRIGGER1 = Physics_Scenario_Create_Trigger( HUT_C_SIGNS_SCENARIO, ON_SHATTER, WAIT_FOR_NEW_POINT );
--Physics_Scenario_Trigger_Add_Objects( HUT_C_SIGNS_TRIGGER1, 23, 100.0 );