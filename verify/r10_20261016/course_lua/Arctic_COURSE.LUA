-- course.lua
-- Arctic

Course_Id(0)
-------------------------------------------------------------------
--- WORLD STUFF ---------------------------------------------------
-------------------------------------------------------------------
AI_Bsp_Filename("ai.bsp")
World_Bsp_Filename("graph.bsp")
Collision_Bsp_Filename("collisions.bsp")
Texture_Dictionary_Filename("textures.txd")
--Occluder_Filename("Occluder.dff")
Sky_Filename(0,"sky.dff")
Setup_Fog(0.1,70,40,44,48)
Lights_Filename("Lights.dff")

Spline_Filename(1,"Wave.spl")
Spline_Filename(2,"Wave1.spl")
Spline_Filename(3,"Wave2.spl")
Spline_Filename(4,"Wave3.spl")
Spline_Filename(5,"Wave4.spl")
Spline_Filename(6,"Wave5.spl")
Spline_Filename(7,"Wave6.spl")
Spline_Filename(8,"Wave7.spl")
Spline_Filename(9,"Wave8.spl")
Spline_Filename(10,"Wave9.spl")
--------------------------------------------------------------------
-- Copter Stuff ----------------------------------------------------
--------------------------------------------------------------------
General_Anim_Filename( 0 ,"H_Anim_1.anm" )
General_Anim_Filename( 1 ,"H_Anim_2.anm" )
General_Anim_Filename( 2 ,"H_Anim_3.anm" )
General_Anim_Filename( 3 ,"H_Anim_4.anm" )
General_Anim_Filename( 4 ,"H_Anim_5.anm" )
General_Anim_Filename( 5 ,"KTC1.anm" )

SetCopter("Copter" , 0 )
SetCopter("JetRanger" , 1 )
SetCopter("Copter" , 2 )
SetCopter("JetRanger" , 3 )
SetCopter("JetRanger" , 4 )
--------------------------------------------------------------------
-- Anim's ----------------------------------------------------------
--------------------------------------------------------------------

-- Camera_Anim_Filename("filename.anm")

--------------------------------------------------------------------
-- Dff's -----------------------------------------------------------
--------------------------------------------------------------------
-- Clump_Filename(1,"crate.dff")
Clump_Filename(2,"sea.dff")
Clump_Filename(3,"Freighter.dff")
Clump_Filename(6,"FishingBoat.dff")
Clump_Filename(7,"CamMan.dff")
Clump_Filename(8,"windows.dff")
Clump_Filename(9,"Mist.dff")

Clump_Exclude_From_World(8)
Clump_Exclude_From_World(9)
---------------------------------------------------------------------
-- Mts's ------------------------------------------------------------
---------------------------------------------------------------------
-- Mts_Filename(0,"Crate.mts")
Mts_Filename(1,"OrangeLights.mts")
Mts_Filename(2,"RedLights.mts")
Mts_Filename(3,"YellowLights.mts")

-- Clump_Mts_Pair(clump_index,mts_index)
-------------------------------------------------------------------
-- Vehicle Commarnds ----------------------------------------------
-------------------------------------------------------------------
Shadow_Scale(.3)

Vehicle_Shininess_Range(0.6,1.0)
-------------------------------------------------------------------
-- UV ANIMATION DICTIONARIES --------------------------------------
-------------------------------------------------------------------
UVA_Filename( 0 , "Sea.uva" )
-------------------------------------------------------------------
-- ID VALUES FOR RW DFF'S -----------------------------------------
-------------------------------------------------------------------
local WHITE_TYRE = 1000
local RED_TYRE = 2000
local CRATE = 3000
--local DOZER = 4000
-------------------------------------------------------------------
-- RWP OBJECT VALUES ----------------------------------------------
-------------------------------------------------------------------
local GENERAL_FATNESS = 0.1
local TYRE_FATNESs = 0.25
local BOUNCE = 0
local FRICTION = 1
-------------------------------------------------------------------
-- RWP MATERIAL ID ------------------------------------------------
-------------------------------------------------------------------
local SURFACE = 0
local WOOD = 1
local WOODSNAP = 2
local METAL = 3
local RUBBER = 4
local CONCRETE = 5
local PLASTIC = 6
local FOLIAGE = 7
---------------------------------------------------------------------
-- RENDER WARE PHYSICS COMMANDS -------------------------------------
---------------------------------------------------------------------
-- Tempary Render Ware Box To Replace the RW Cylinder
RWP_Object( 0, "White Crash Tyres", "WhiteTyre.dff", "WhiteTyre.mts" )
RWP_Object_Box( 0, 0.6, 0.77, 0.6 )
RWP_Object_Properties( 0, TYRE_FATNESs, BOUNCE, FRICTION, 0.5 , RUBBER )
RWP_Object_BaseLabel( 0, WHITE_TYRE )

-- RWP_Object_CentreOfMassOffset(index,x,y,z)

-- Tempary Render Ware Box To Replace the RW Cylinder
RWP_Object( 1, "Red Crash Tyres", "RedTyre.dff", "RedTyre.mts" )
RWP_Object_Box( 1, 0.6 ,0.77, 0.6 )
RWP_Object_Properties( 1, TYRE_FATNESs, BOUNCE, FRICTION, 0.5 , RUBBER )
RWP_Object_BaseLabel( 1, RED_TYRE )

RWP_Object( 2,"Crate Type 2", "Crate02.dff", "Crate02.mts" )
RWP_Object_Box( 2, 3.025, 3.025, 5.0 )
RWP_Object_Properties( 2, GENERAL_FATNESS, BOUNCE, FRICTION, 4.0 , METAL )
RWP_Object_BaseLabel( 2, CRATE )

--RWP_Object( 3, "BullDozer", "Dozer.dff", "Dozer.mts" )
--RWP_Object_Box( 3, 4.9, 4.6, 7.69 )
--RWP_Object_Properties( 3, GENERAL_FATNESS, BOUNCE, FRICTION, 5.0 , METAL ) 
--RWP_Object_BaseLabel( 3, DOZER )

--=============================
-- Line Rain commands
--=============================
-- Explanation
---------------
-- RainEnable(); 
-- turns rain on, takes no parameters. If this is used without the following other 
-- functions, the default values (from the arctic) will be used.

-- RainSetHeadColour(red,green,blue,alpha); 
-- sets the colour of the rain 'head'

-- RainSetTailColour(red,green,blue,alpha); 
-- sets the colour of the rain 'tail'

-- RainSetPosition(xmin,xmax,ymin,ymax,zmin,zmax); 
-- sets the area around the camera where the rain will be generated

-- RainSetScale(scale, length); 
-- sets the scale for the rain

-- RainSetCameraScale(yScale,atScale);
-- sets the distance above and in front of the camera where the rain will be generated

-- RainSetDirection(xmin, xmax, ymin, ymax, zmin, zmax); 
-- sets the direction the rain will travel between the min and max in each direction

-------------------------------
-- Example usage
-------------------------------
RainEnable()
--RainSetHeadColour(120,128,136,64)
--RainSetTailColour(120,128,136,0)
--RainSetPosition(-6.0,6.0, 0.0,0.0, -6.0,6.0)
--RainSetScale(0.25,0.25)
--RainSetCameraScale(3.0,6.0)
--RainSetDirection(0.0,0.1, 0.3,0.6, 0.0,0.1)
