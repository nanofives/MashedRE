-- course.lua
-- Training

Course_Id(30)
-------------------------------------------
-- WORLD STUFF ----------------------------
-------------------------------------------
AI_Bsp_Filename( "Lap.bsp" )
World_Bsp_Filename( "graph.bsp" )
Collision_Bsp_Filename( "collisions.bsp" )
Texture_Dictionary_Filename( "textures.txd" )
Sky_Filename( 0 ,"sky.dff" )
Setup_Fog( 20 , 360 , 207 , 224 ,255 )
Lights_Filename( "Lights.dff" )
Occluder_Filename( "Occluder.dff" )

-- Spline_Filename( 0 ,"Filename.spl" )
--------------------------------------------
-- Copter Stuff ----------------------------
--------------------------------------------
General_Anim_Filename( 0 , "H_Anim_1.anm" )
General_Anim_Filename( 1 , "H_Anim_2.anm" )
General_Anim_Filename( 2 , "H_Anim_3.anm" )
General_Anim_Filename( 3 , "H_Anim_4.anm" )

SetCopter( "Copter" , 0 )
SetCopter( "Copter" , 1 )
SetCopter( "CivHuey" , 2 )
SetCopter( "Copter" , 3 )
-------------------------------------------
-- Flyin Camera ---------------------------
-------------------------------------------
--Camera_Anim_Filename( "Intro_Cam.anm" )

-------------------------------------------
-- Dff Files For Graphics -----------------
-------------------------------------------
--Clump_Filename( 0 , "Plant.dff" )
Clump_Filename( 1 , "Camman.dff" )


Clump_Filename( 3 , "Water02.dff" )
Clump_Filename( 4 , "Water03.dff" )
Clump_Filename( 5 , "Tank1.dff" )
Clump_Filename( 6 , "Tank2.dff" )
Clump_Filename( 7 , "Tank3.dff" )
Clump_Filename( 8 , "Tank4.dff" )
Clump_Filename( 9 , "Lake.dff" )

Clump_Filename( 11 , "Road.dff" )


--Clump_Exclude_From_World( 0 )

Clump_Exclude_From_World( 3 )
Clump_Exclude_From_World( 4 )
Clump_Exclude_From_World( 5 )
Clump_Exclude_From_World( 6 ) 
Clump_Exclude_From_World( 7 )
Clump_Exclude_From_World( 8 )
Clump_Exclude_From_World( 9 )
------------------------------------------
-- Mts Files For Graphics ----------------
------------------------------------------
--Mts_Filename( 0 , "Plant.mts" )
Mts_Filename( 2 , "Smoke.mts" )


--Clump_Mts_Pair( 0 , 0 )
------------------------------------------
-- VEHICLE COMMANDS ----------------------
------------------------------------------
--Shadow_Scale(1)

Vehicle_Shininess_Range( 0.7 , 1 )
-------------------------------------------
-- ID VALUES FOR RW DFF'S -----------------
-------------------------------------------
local WHITE_TYRE_LABEL = 1000
local RED_TYRE_LABEL = 2000
local SIGN_2 = 3000
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
-- local FOLIAGE = 7
-------------------------------------------
-- RWP OBJECT VALUES ----------------------
-------------------------------------------
local NO_FATNESS = 0.0
local GENERAL_FATNESS = 0.1
local TYRE_FATNESS = 0.25
local SMALL_FATNESS = 0.05
local BOUNCE = 0
local FRICTION = 1
------------------------------------------------------------------
-- RENDER WARE PHYSICS COMMANDS ----------------------------------
------------------------------------------------------------------
-- 0
RWP_Object( 0 , "White Crash Tyres" , "WhiteTyre.dff" , "Whitetyre.mts" )
RWP_Object_Capsule( 0 , 1.25 , 0.45 )
RWP_Object_Properties( 0 , NO_FATNESS , BOUNCE, FRICTION , 0.5 , RUBBER )
RWP_Object_BaseLabel( 0 , WHITE_TYRE_LABEL )
RWP_Object_ShatterVel( 0 , 100 )

-- RWP_Object_CentreOfMassOffset( index, x, y, z )

-- 1
RWP_Object( 1 , "Red Crash Tyres" , "RedTyre.dff" , "Redtyre.mts" )
RWP_Object_Capsule( 1 , 1.25 , 0.45 )
RWP_Object_Properties( 1 , NO_FATNESS , BOUNCE , FRICTION , 0.5 , RUBBER )
RWP_Object_BaseLabel( 1 , RED_TYRE_LABEL )
RWP_Object_ShatterVel( 1 , 100 )
-- 2
RWP_Object( 2, "Stop Sign 2 ", "Sign02.dff", "Sign02.mts" )
RWP_Object_Box( 2 , 4.275 , 1.54 , 0.21 )
RWP_Object_Properties( 2 , GENERAL_FATNESS , BOUNCE, FRICTION , 0.2 , WOOD )
RWP_Object_BaseLabel( 2 , SIGN_2 )
RWP_Object_ShatterVel( 2 , 6 )

-------------------------------------------------------------------
-- UV ANIMATION DICTIONARIES --------------------------------------
-------------------------------------------------------------------
UVA_Filename(0,"Lake.uva")

