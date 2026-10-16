-- course.lua
-- Course Name Neustein

Course_Id( 39 )
--------------------------------------------------------
-- World Sruff -----------------------------------------
--------------------------------------------------------
Texture_Dictionary_Filename( "Neustein.txd" )
World_Bsp_Filename( "Graphics.bsp" )
AI_Bsp_Filename( "Ai1.bsp" )
Collision_Bsp_Filename( "Collide.bsp" )
Lights_Filename( "Lights.dff" )
Setup_Fog( 0 , 250 , 255 , 255 , 255 )
Sky_Filename( 0 , "SkyDome_02.dff" )
Sky_Filename( 1 , "SkyDome_01.dff" )

--Occluder_Filename( "Occluder.dff" )
-------------------------------------------------------
-- Intro Camera ---------------------------------------
-------------------------------------------------------
--Camera_Anim_Filename( "Intro_Cam.anm" )

-------------------------------------------------------
--Dff Files For Graphics ------------------------------
-------------------------------------------------------
--Clump_Filename( 0 , "Trolly_1.dff" )
Clump_Filename( 1 , "Glass.dff" )
Clump_Filename( 3 , "Trunk.dff" )
Clump_Filename( 5 , "Rock.dff" )
Clump_Filename( 6 , "Edge.dff" )
Clump_Filename( 7 , "CamMan.dff" )

Clump_Exclude_From_World( 3 )
Clump_Exclude_From_World( 5 )
Clump_Exclude_From_World( 6 )
-------------------------------------------------------
-- Mts Files For Graphics -----------------------------
-------------------------------------------------------
Mts_Filename( 0 , "Trunks_1.mts" )
Mts_Filename( 1 , "Leaf.mts" )
Mts_Filename( 2 , "Trunks_2.mts" )
Mts_Filename( 3 , "Cable_1.mts" )
Mts_Filename( 4 , "Cable_2.mts" )
Mts_Filename( 5 , "Trunks_3.mts" )
-------------------------------------------------------
-- Dff & Mts File Matching command --------------------
-------------------------------------------------------
--Clump_Mts_Pair( 3 , 0 )
--Clump_Mts_Pair( 4 , 2 ) 
--Clump_Mts_Pair( 8 , 5 )  
-------------------------------------------------------
-- Anim's ---------------------------------------------
-------------------------------------------------------
General_Anim_Filename( 0 , "Trolly_1.anm" )
General_Anim_Filename( 1 , "Trolly_2.anm" )
General_Anim_Filename( 2 , "H_Anim_1.anm" )
General_Anim_Filename( 3 , "H_Anim_2.anm" )

--------------------------------------------
-- Copter Stuff ----------------------------
--------------------------------------------
local LINEAR = 1
--SetClumpAnim( LINEAR , 0 , 0 )
--SetClumpAnim( LINEAR , 0 , 1 )

SetCopter("Copter" , 2 )
SetCopter("JetRanger" , 3 )
-------------------------------------------------------
-- Vehicle Commands -----------------------------------
-------------------------------------------------------
--Vehicle_Shininess_Range( 0.5, 0.8 )
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
local GENERAL_FATNESS = 0.5
local SMALL_FATNESS = 0.05
local BOUNCE = 0
local FRICTION = 1
--------------------------------------------------------
-- ID VALUES FOR RW DFF'S ------------------------------
--------------------------------------------------------
--local DESK = 1000
--local HUT = 2000
local POST = 3000
local TROLLY = 4000
local CABLE_CAR = 5000
--------------------------------------------------------
-- RENDER WARE PHYSICS COMMANDS ------------------------
--------------------------------------------------------
-- 1
RWP_Object( 0 , "Post" , "Post_1.dff" , "Post_1.mts" )
RWP_Object_Capsule( 0 , 0.189 , 2.811 )
--RWP_Object_Cylinder( 0 , 0,189, 3 )
RWP_Object_Properties( 0 , NO_FATNESS , BOUNCE ,  FRICTION , 0.2 , WOOD )
RWP_Object_BaseLabel( 0 , POST )
RWP_Object_ShatterVel( 0 , 100 )

-- RWP_Object_CentreOfMassOffset( index , x , y , z )

-- 2
RWP_Object( 1 , "Trolly " , "Trolly_1.dff" , "Trolly.mts" )
RWP_Object_Box( 1 , 0.6 , 0.6 , 0.6 )
RWP_Object_Properties( 1 , GENERAL_FATNESS , BOUNCE ,  FRICTION , 2 , METAL )
RWP_Object_BaseLabel( 1 , TROLLY )
RWP_Object_ShatterVel( 1 , 100 )

-- 3
RWP_Object( 2 , "Cable Car " , "CableCar.dff" , "CableCar.mts" )
RWP_Object_Box( 2 , 5.0, 3.5, 8.0 )
RWP_Object_Properties( 2 , 1.0 , BOUNCE ,  FRICTION , 10 , METAL )
RWP_Object_BaseLabel( 2 , CABLE_CAR )
RWP_Object_ShatterVel( 2 , 100 )
