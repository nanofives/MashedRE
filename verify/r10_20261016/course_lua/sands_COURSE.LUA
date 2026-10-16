-- course.lua
-- SANDS

Course_Id(11)
------------------------------------------
-- WORLD STUFF ---------------------------
------------------------------------------
Texture_Dictionary_Filename("Sands.txd")
World_Bsp_Filename("Graphics.bsp")
AI_Bsp_Filename("Ai1.bsp")
Collision_Bsp_Filename("Collide.bsp")
Occluder_Filename("Occluder.dff")
Lights_Filename("Lights.dff")
Sky_Filename(0,"skydome.dff")

--Spline_Filename("spl1.spl")
Setup_Fog(250,255,128,128,128)
-------------------------------------------
-- Copter Stuff ---------------------------
-------------------------------------------
General_Anim_Filename(0,"H_Anim_1.anm")
General_Anim_Filename(1,"H_Anim_2.anm")
General_Anim_Filename(2,"H_Anim_3.anm")

SetCopter("CivHuey" , 0 )
SetCopter("Copter" , 1 )
SetCopter("CivHuey" , 2 )
------------------------------------------
-- Dff's ---------------------------------
------------------------------------------
Clump_Filename(1,"Water.dff")

Clump_Filename(4,"Road.dff")
Clump_Filename(5,"OakTrunk.dff")

Clump_Filename(7,"Jump.dff")

--Clump_Filename(8,"BridgeCen")
Clump_Filename(9,"Gantrys")
Clump_Filename(10,"Debris.dff")

Clump_Filename(14,"Newglow1.dff")
Clump_Filename(15,"Newglow2.dff")
Clump_Filename(16,"Newglow3.dff")
Clump_Filename(17,"Camman.dff")
Clump_Filename(18,"Bridgehafbroke.dff")
Clump_Filename(19,"BridgeBroke.dff")
Clump_Filename(20,"BridgeWhole.dff")
----------------------------------------------------------------------------------------------------------------------------------


Clump_Exclude_From_World(1)

Clump_Exclude_From_World(5)

Clump_Exclude_From_World(7)

--Clump_Exclude_From_World(8)

Clump_Exclude_From_World(9)




----------------------------------------------------------------------------------------------------------------------------------
--The new bridge bits

Clump_Exclude_From_World(10)

Clump_Exclude_From_World(18)
Clump_Exclude_From_World(19)
Clump_Exclude_From_World(20)

Clump_Exclude_From_World(14)
Clump_Exclude_From_World(15)
Clump_Exclude_From_World(16)
-------------------------------------------
-- Mts Files For Graphics -----------------
-------------------------------------------
Mts_Filename(0,"trees.mts")
Mts_Filename(1,"OakLeaf.mts")

-- Dff & Mts File Matching command

Clump_Mts_Pair(5,0)
-------------------------------------------
-- Vehicle Commands -----------------------
-------------------------------------------
Vehicle_Shininess_Range(0.4,0.7)
-------------------------------------------------------------------
-- UV ANIMATION DICTIONARIES --------------------------------------
-------------------------------------------------------------------
UVA_Filename(0,"Water.uva")
-------------------------------------------
-- ID VALUES FOR RW DFF'S -----------------
-------------------------------------------
local WHITE_TYRE = 1000
local RED_TYRE = 2000
local SIGN_2 = 3000
local W_FENCE = 4000
local BLOCK_L = 5000
local BLOCK_R = 6000
local BLOCK_ML = 7000
local BLOCK_MR = 8000

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
------------------------------------------
-- RWP OBJECT VALUES ---------------------
------------------------------------------
local NO_FATNESS = 0.0
local GENERAL_FATNESS = 0.1
local TYRE_FATNESS = 0.25
local SMALL_FATNESS = 0.05
local BOUNCE = 0
local FRICTION = 1
-------------------------------------------
-- RENDER WARE PHYSICS COMMANDS -----------
-------------------------------------------
-- 0
RWP_Object( 0 , "White Crash Tyres" , "WhiteTyre.dff" , "WhiteTyre.mts" )
RWP_Object_Capsule( 0 , 1.25, 0.45 )
RWP_Object_Properties( 0 , NO_FATNESS , BOUNCE , FRICTION , 0.5 , RUBBER )
RWP_Object_BaseLabel( 0 , WHITE_TYRE )
-- 1
RWP_Object( 1, "Red Crash Tyres", "RedTyre.dff", "RedTyre.mts" )
RWP_Object_Capsule(1 , 1.25 , 0.45 )
RWP_Object_Properties( 1 , NO_FATNESS , BOUNCE , FRICTION , 0.5 , RUBBER )
RWP_Object_BaseLabel( 1 , RED_TYRE )
-- 2
RWP_Object( 2, "Stop Sign 2 ", "Sign02.dff", "Sign02.mts" )
RWP_Object_Box( 2, 4.275, 1.54, 0.21 )
RWP_Object_Properties( 2, GENERAL_FATNESS, BOUNCE, FRICTION, 0.2 , WOOD )
RWP_Object_BaseLabel( 2, SIGN_2 )
-- 3
--RWP_Object( 3, "Barrel Red", "RBarrel.dff", "RBarrel.mts" )
--RWP_Object_Box( 3, 0.772, 1.27, 0.772 )
--RWP_Object_Properties( 3, TYRE_FATNESS, BOUNCE, FRICTION, 2.0 , METAL )
-- 4
RWP_Object( 4, "Wood Fence Type 1", "Fence.dff", "Fence01.mts" )
RWP_Object_Box( 4, 2.382, 1.698, 0.219 )
RWP_Object_Properties( 4, GENERAL_FATNESS, BOUNCE, FRICTION, 1.0 , WOODSNAP )
RWP_Object_BaseLabel( 4, W_FENCE )

-------------------------------------------
--RENDER WARE COLLAPSING BRIDGE------------
-------------------------------------------
-- 5
--RWP_Object( 5, "blocks_R", "block03.dff", "blocks_R.mts" )
--RWP_Object_Box( 5, 5, 1.73, 7 )
--RWP_Object_Properties( 5, 0.1, BOUNCE, FRICTION, 3.0 , CONCRETE )
--RWP_Object_BaseLabel( 5, BLOCK_R )
--RWP_Object_ShatterVel( 5, 6 )

-- 6
--RWP_Object(6, "blocks_ML", "block01.dff", "blocks_ML.mts" )
--RWP_Object_Box( 6, 5, 1.73, 7 )
--RWP_Object_Properties( 6,0.1, BOUNCE, FRICTION, 3.0 , CONCRETE )
--RWP_Object_BaseLabel( 6, BLOCK_ML )
--RWP_Object_ShatterVel( 6, 6 )

-- 7
--RWP_Object(7, "blocks_MR", "block02.dff", "blocks_MR.mts" )
--RWP_Object_Box( 7, 5, 1.73, 7 )
--RWP_Object_Properties( 7,0, BOUNCE, FRICTION, 3.0 , CONCRETE )
--RWP_Object_BaseLabel( 7, BLOCK_MR )
--RWP_Object_ShatterVel( 7, 6 )

-- 8
--RWP_Object(8, "blocks_L", "block04.dff", "blocks_L.mts" )
--RWP_Object_Box( 8, 5, 1.73, 7 )
--RWP_Object_Properties( 8,0.1, BOUNCE, FRICTION, 3.0 , CONCRETE )
--RWP_Object_BaseLabel( 8, BLOCK_L )
--RWP_Object_ShatterVel( 8, 6 )