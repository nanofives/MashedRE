-- course.lua
-- Course Name DUMP

Course_Id(37)



--------------------------------------------
-- ID VALUES FOR RW DFF'S ------------------
--------------------------------------------
local DESK = 1000
local HUT = 2000
local HUT_WALL_1 = 3000
local HUT_WALL_2 = 4000
local HUT_WALL_3 = 5000
local HUT_WALL_4 = 6000
local HUT_WALL_5 = 7000
local BARREL_RED = 8000
local TRAFFIC_CONE = 9000
local SIGN02 = 10000
local W_BIN_1 = 11000
local W_FENCE = 12000
local SKIP = 13000
local WINDOW = 14000
local PUMP = 15000
local SHRUBBERY = 16000
local TIPPER = 17000
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
local BOUNCE = 0
local FRICTION = 1
-------------------------------------------
--- WORLD STUFF ---------------------------
-------------------------------------------

Texture_Dictionary_Filename("dump.txd")

World_Bsp_Filename("Graphics.bsp")

AI_Bsp_Filename("Ai1.bsp")

Collision_Bsp_Filename("Collide.bsp")

Occluder_Filename("Occluder.dff")

Lights_Filename("Lights.dff")

Sky_Filename(0,"skydome01.dff")


Setup_Fog(0,180,128,128,128)

--Spline_Filename("spl1.spl")




------------------------------------------
-- Dff's ---------------------------------
------------------------------------------

Clump_Filename(0,"Train.dff")
Clump_Filename(1,"Fence3.dff")
Clump_Filename(2,"Barrierconveyor.dff")
Clump_Filename(3,"IronRailings01.dff")
Clump_Filename(4,"CraneArm.dff")
Clump_Filename(5,"Flyover.dff")
Clump_Filename(6,"RetailPark.dff")
Clump_Filename(7,"GarbageMov.dff")
Clump_Filename(8,"MainIncin.dff")
Clump_Filename(9,"Road.dff")
--Clump_Filename(10,"Seagull.dff")
--Clump_Filename(11,"Trunk.dff")
Clump_Filename(12,"Arrows.dff")
Clump_Filename(13,"CamMan.dff")
Clump_Filename(14,"Fence1.dff")
Clump_Filename(15,"Fence2.dff")
Clump_Filename(16,"Garage.dff")



Clump_Exclude_From_World(1)
Clump_Exclude_From_World(2)
Clump_Exclude_From_World(3)
Clump_Exclude_From_World(4)
Clump_Exclude_From_World(6)
Clump_Exclude_From_World(9)
--Clump_Exclude_From_World(11)
Clump_Exclude_From_World(14)
Clump_Exclude_From_World(15)

------------------------------------------
-- Copter Stuff --------------------------
------------------------------------------
General_Anim_Filename(0,"H_Anim_1.anm")

General_Anim_Filename(1,"H_Anim_2.anm")

General_Anim_Filename(2,"H_Anim_3.anm")

General_Anim_Filename(3,"Train.anm")

--General_Anim_Filename(4,"Tipper.anm")

--General_Anim_Filename(5,"SeaGUllAnim2.anm")

--General_Anim_Filename(6,"SeaGUllAnim3.anm")

--General_Anim_Filename(7,"SeaGullAnim.anm")

SetCopter("CivHuey" , 0 )
SetCopter("Copter" , 1 )
SetCopter("CivHuey" , 2 )



local LINEAR = 1

SetClumpAnim( LINEAR,0,3)
--SetClumpAnim(10,5)
--SetClumpAnim(10,6)
--SetClumpAnim(10,7)
------------------------------------------
-- Mts Files For Graphics ----------------
------------------------------------------

--Mts_Filename(0,"TreeTrunks.mts")
--Mts_Filename(1,"OakLeaf.mts")
Mts_Filename(2,"Magnet.mts")
Mts_Filename(3,"smokePOS.mts")


-- Dff & Mts File Matching command

--Clump_Mts_Pair(11,0)

------------------------------------------
-- Vehicle Commarnds ---------------------
------------------------------------------

Vehicle_Shininess_Range(0.5,1.0)

------------------------------------------
-- UV ANIMATION DICTIONARIES -------------
------------------------------------------

UVA_Filename(0,"GarbageMov.uva")





-------------------------------------------------------------------
-- RENDER WARE PHYSICS COMMANDS -----------------------------------
-------------------------------------------------------------------


RWP_Object( 0 , "Traffic Cone" , "TrafficC.dff" , "TrafficC.mts" )
RWP_Object_Capsule( 0 , 0.57 , 0.77 )
RWP_Object_Properties( 0 , BARRAL_FATNESS , BOUNCE , FRICTION , 0.1 , PLASTIC )
RWP_Object_BaseLabel( 0 , TRAFFIC_CONE )
RWP_Object_ShatterVel( 0 , 100 )

RWP_Object( 1 , "Stop Sign 2 " , "Sign02.dff" , "Sign02.mts" )
RWP_Object_Box( 1 , 4.275 , 1.54 , 0.21 )
RWP_Object_Properties( 1 , GENERAL_FATNESS , BOUNCE , FRICTION , 0.2 , WOOD )
RWP_Object_BaseLabel( 1, SIGN02 )
RWP_Object_ShatterVel( 1 , 6 )

--RWP_Object(2 ,"Wheely Bin Type 3" , "Bin02.dff" , "Bin02.mts" )
--RWP_Object_Box( 2 , 2.408 , 2.045 , 1.704 )
--RWP_Object_Properties( 2 , GENERAL_FATNESS , BOUNCE , FRICTION , 1.0 , PLASTIC )
--RWP_Object_BaseLabel( 2, W_BIN_1 )
--RWP_Object_ShatterVel( 2 , 100 )

RWP_Object( 3 , "Wood Sign Type 3" , "Sign03.dff" , "Sign03.mts" )
RWP_Object_Box( 3 , 2.382 , 1.698 , 0.219 )
RWP_Object_Properties( 3, GENERAL_FATNESS , BOUNCE , FRICTION , 0.05 , WOOD )
RWP_Object_BaseLabel( 3, W_FENCE )
RWP_Object_ShatterVel( 3 , 6 )


--RWP_Object( 4 , "Pump" , "Pump.dff" , "Pump.mts" )
--RWP_Object_Box(4, 0.751, 2.23, 0.751 )
--RWP_Object_Properties( 4 , GENERAL_FATNESS , BOUNCE , FRICTION , 1.0 , METAL )
--RWP_Object_BaseLabel( 4 , PUMP )
--RWP_Object_ShatterVel( 4 , 6 )


--RWP_Object( 5 , "Tipper", "Tipper.dff", "Tipper.mts" )
--RWP_Object_Box( 5 , 4.04, 3.14, 8.57 )
--RWP_Object_Properties( 5 , 0, BOUNCE , FRICTION , 3.0, WOOD )
--RWP_Object_BaseLabel( 5 , TIPPER )
--RWP_Object_ShatterVel( 5 , 100 )


-----------------------------------------------------------------
--Scenario
-----------------------------------------------------------------
-------------------------------
--Triggers
--Need to work out what to do with these defines
--local ON_BLAST = 0
--local ON_SHATTER = 1
--local ON_MOVE = 2

--local IMMEDIATE = 0
--local WAIT_FOR_NEW_POINT = 1

--local TIPPERDRIVE = Physics_Scenario_Create( "Tipperdrive" );
--Physics_Scenario_Add_Animated_Object( TIPPERDRIVE, 5, 4 );
--local TIPPERDRIVE_TRIGGER = Physics_Scenario_Create_Trigger( TIPPERDRIVE, ON_MOVE,IMMEDIATE );

--Physics_Scenario_Set_Sequence( TIPPERDRIVE );