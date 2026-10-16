-- course.lua
-- Course_Name("City")

Course_Id(26)

------------------------------------------
-- World Stuff ---------------------------
------------------------------------------
Texture_Dictionary_Filename("City.txd")
World_Bsp_Filename("Graphics.bsp")
AI_Bsp_Filename("Ai1.bsp")
Collision_Bsp_Filename("Collide.bsp")
Lights_Filename("Lights.dff")

--Spline_Filename("spl1.spl")
--Occluder_Filename("Occluder.dff")
Setup_Fog(0,250,0,0,0)

UVA_Filename(0,"Animations.uva")


-------------------------------------------
-- Copter Stuff ---------------------------
-------------------------------------------
General_Anim_Filename(1,"Huey_G3.anm")
General_Anim_Filename(2,"Huey_G2.anm")
General_Anim_Filename(3,"Huey_G.anm")

SetCopter("CivHuey" , 2 )
SetCopter("Copter" , 3 )
SetCopter("CivHuey" , 1 )


General_Anim_Filename( 0 ,"KTC1.anm" )
-----------------------------------------


-- Dff Files For Graphics
Sky_Filename(0,"skydome.dff")

Clump_Filename(0,"win01.dff")

--Clump_Filename(1,"win02.dff")

Clump_Filename(2,"win03.dff")
Clump_Filename(3,"Animations.dff")

Clump_Filename(4,"objects01.dff")
Clump_Filename(5,"objects02.dff")
Clump_Filename(6,"Build01.dff")
Clump_Filename(7,"Build02.dff")
Clump_Filename(8,"Build03.dff")
Clump_Filename(9,"Build04.dff")
Clump_Filename(10,"Standard.dff")
Clump_Filename(11,"road.dff")
Clump_Filename(12,"Trunk.dff")
Clump_Filename(13,"water.dff")

Clump_Exclude_From_World(6)
Clump_Exclude_From_World(7)
Clump_Exclude_From_World(8)
Clump_Exclude_From_World(9)
Clump_Exclude_From_World(10)
Clump_Exclude_From_World(11)
Clump_Exclude_From_World(12)
Clump_Exclude_From_World(13)


-- Mts Files For Graphics

Mts_Filename(0,"TreeTrunks.mts")
Mts_Filename(1,"OakLeaf.mts")
Mts_Filename(2,"Staticposts.mts")
Mts_Filename(3,"WhiteStaticposts.mts")
Mts_Filename(4,"Standard.mts")
Mts_Filename(5,"Trafficlight.mts")
Mts_Filename(6,"Twolight.mts")

-- Dff & Mts File Matching command

Clump_Mts_Pair(12,0)
Clump_Mts_Pair(10,4)



-- Vehicle Commarnds

Vehicle_Shininess_Range(0.5,1.0)

--------------------------------------------
-- ID VALUES FOR RW DFF'S ------------------
--------------------------------------------

local LAMPOSTS = 1000
local PARKPOST = 2000
local GATES = 3000
local SWING = 4000
local TRAFFICLIGHT = 5000
local TWOLIGHT = 6000


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
local GLASS_SHATTER = 8
-------------------------------------------
-- RWP OBJECT VALUES ----------------------
-------------------------------------------
local GENERAL_FATNESS = 0.1
local SMALL_FATNESS = 0.05
local BARRAL_FATNESS = 0.2
local NO_FATNESS = 0
local BOUNCE = 0
local FRICTION = 1
-------------------------------------------


------------------------------------------------------------------
-- RENDER WARE PHYSICS COMMANDS -----------------------------------
-------------------------------------------------------------------


RWP_Object( 0 , "LAMPOSTS" , "Lamposts.dff" , "Lamposts.mts" )
RWP_Object_Capsule( 0 , 0.56 , 10.24 )
RWP_Object_Properties( 0 , NO_FATNESS , BOUNCE , FRICTION , 0.3 , METAL )
RWP_Object_BaseLabel( 0 , LAMPOSTS )
RWP_Object_ShatterVel( 0 , 100 )

RWP_Object( 1 , "PARKPOST" , "Parkpost.dff" , "Parkpost.mts" )
RWP_Object_Capsule( 1 , 0.20 , 8.22 )
RWP_Object_Properties( 1 , NO_FATNESS , BOUNCE , FRICTION , 0.3 , METAL )
RWP_Object_BaseLabel( 1 , PARKPOST )
RWP_Object_ShatterVel( 1 , 100 )

RWP_Object( 2 , "GATES" , "gates.dff" , "gates.mts" )
RWP_Object_Box( 2 , 12.4 , 7.5, 1)
RWP_Object_Properties( 2 , NO_FATNESS , BOUNCE , FRICTION , 0.5 , METAL )
RWP_Object_BaseLabel( 2 , GATES )
RWP_Object_ShatterVel( 2 , 100 )

RWP_Object( 3 , "TRAFFICLIGHT" , "Trafficlight.dff" , "Trafficlight.mts" )
RWP_Object_Capsule( 3 , 0.56 , 10.24 )
RWP_Object_Properties( 3 , NO_FATNESS , BOUNCE , FRICTION , 0.3 , METAL )
RWP_Object_BaseLabel( 3 , TRAFFICLIGHT )
RWP_Object_ShatterVel( 3 , 100 )

RWP_Object( 4 , "TWOLIGHT" , "Twolight.dff" , "Twolight.mts" )
RWP_Object_Capsule( 4 , 0.56 , 10.24 )
RWP_Object_Properties( 4 , NO_FATNESS , BOUNCE , FRICTION , 0.3 , METAL )
RWP_Object_BaseLabel( 4 , TWOLIGHT )
RWP_Object_ShatterVel( 4 , 100 )