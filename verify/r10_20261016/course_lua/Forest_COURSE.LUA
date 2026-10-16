-- course.lua
--Forest

Course_Id( 34 )
------------------------------------------------
-- WORLD STUFF ---------------------------------
------------------------------------------------
AI_Bsp_Filename( "lap.bsp" )
World_Bsp_Filename( "graphics.bsp" )
Collision_Bsp_Filename( "collisions.bsp" )

Object_Bsp_Filename(0, "Trees.bsp" )

Texture_Dictionary_Filename( "textures.txd" )
Lights_Filename( "Lights.dff" )
--Occluder_Filename( "Occluder.dff" )
Sky_Filename( 0 , "skydome.dff" )
Setup_Fog(0,180,136,155,141)

UVA_Filename(0,"Waterfall.uva")

--Spline_Filename( index , "filename.spl" )




------------------------------------------------
-- DFF STUF ------------------------------------
------------------------------------------------
Clump_Filename( 0 , "Water.dff" )

--Clump_Filename( 1 , "Waterfall.dff" )

Clump_Filename( 2 , "FarTree.dff" )

Clump_Filename( 3 , "NearTree.dff" )

Clump_Filename( 4 , "Bushes.dff" )


Clump_Filename( 5 , "MidTree.dff" )


Clump_Filename( 7 , "CamMan.dff" )


Shadow_Scale(0.5)
Vehicle_Shininess_Range(0.0, 0.7)

-------------------------------------------------
---------------MTS-------------------------------

Mts_Filename(0,"FarTree.mts")

Mts_Filename(1,"NearTree.mts")

--Mts_Filename(2,"Oakleaf.mts")


Mts_Filename(3,"MidTree.mts")


Mts_Filename(4,"Bushes.mts")

Mts_Filename(5,"MidTree2.mts")


Mts_Filename(6,"NearTree2.mts")
-------------------------------------------------
-----------MTS PAIR------------------------------


-------------------------------------------------------------------
-------------------TREE TEST STUFF---------------------------------


-------------------------------------------------------------------
-------------------------------------------------------------------



Clump_Exclude_From_World( 0 )
Clump_Exclude_From_World( 2 )
Clump_Exclude_From_World( 3 )
Clump_Exclude_From_World( 4 )
Clump_Exclude_From_World( 5 )

--------------------------------------------
-- Copter Stuff ----------------------------
--------------------------------------------
General_Anim_Filename(0,"H_Anim_1.anm")
General_Anim_Filename(1,"H_Anim_2.anm")

SetCopter("Copter" , 0 )
SetCopter("CivHuey" ,1 )
--------------------------------------------
-- Intro Camera ----------------------------
--------------------------------------------

--Camera_Anim_Filename( "Intro_Cam.anm" )


-------------------------------------------
-- ID VALUES FOR RW DFF'S -----------------
-------------------------------------------
local SIGN_2 = 1000
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
local BOUNCE = 0
local FRICTION = 1
-------------------------------------------------
-- RENDER WARE PHYSICS COMMANDS
-------------------------------------------------
-- 0
--RWP_Object( 0 , "Stop Sign 2 " , "Sign02.dff" , "Sign02.mts" )
--RWP_Object_Box( 0 , 4.275 , 1.54 , 0.21 )
--RWP_Object_Properties( 0 , GENERAL_FATNESS , BOUNCE , FRICTION , --0.2 , WOOD )
--RWP_Object_BaseLabel( 0 , SIGN_2 )
--RWP_Object_ShatterVel( 0 , 6 )