-- course.lua
--Highway
Course_Id(38)

AI_Bsp_Filename("lap.bsp")

World_Bsp_Filename("graphics.bsp")

Collision_Bsp_Filename("collisions.bsp")

Texture_Dictionary_Filename("textures.txd")

Lights_Filename("Lights.dff")



--Occluder_Filename("Occluder.dff")

--Powerup_Filename("filename.dff")

Sky_Filename(0,"sky.dff")

Setup_Fog(50,300,165,197,209)
Modify_Fog(180,0,300,213,208,179)


--Shine_Scale(x)

--Shadow_Scale(x)

--Vehicle_Body_Dirt_Texture("filename")

--Vehicle_Wheel_Dirt_Texture("filename")



Clump_Filename(0,"WhiteTruckHigh.dff")
Clump_Filename(1,"WhiteTruckMid.dff")
Clump_Filename(2,"WhiteTruckLow.dff")
Clump_Filename(3,"BlueTruckHigh.dff")
Clump_Filename(4,"BlueTruckMid.dff")
Clump_Filename(5,"BlueTruckLow.dff")
Clump_Filename(6,"GrayTruckHigh.dff")
Clump_Filename(7,"GrayTruckMid.dff")
Clump_Filename(8,"GrayTruckLow.dff")
Clump_Filename(9,"GreenTruckHigh.dff")
Clump_Filename(10,"GreenTruckMid.dff")
Clump_Filename(11,"GreenTruckLow.dff")
Clump_Filename(12,"RedTruckHigh.dff")
Clump_Filename(13,"RedTruckMid.dff")
Clump_Filename(14,"RedTruckLow.dff")
Clump_Filename(15,"YellowTruckHigh.dff")
Clump_Filename(16,"YellowTruckMid.dff")
Clump_Filename(17,"YellowTruckLow.dff")

Clump_Filename(18,"BlueLorryHigh.dff")
Clump_Filename(19,"BlueLorryMid.dff")
Clump_Filename(20,"BlueLorryLow.dff")
Clump_Filename(21,"GrayLorryHigh.dff")
Clump_Filename(22,"GrayLorryMid.dff")
Clump_Filename(23,"GrayLorryLow.dff")
Clump_Filename(24,"RedLorryHigh.dff")
Clump_Filename(25,"RedLorryMid.dff")
Clump_Filename(26,"RedLorryLow.dff")
Clump_Filename(27,"YellowLorryHigh.dff")
Clump_Filename(28,"YellowLorryMid.dff")
Clump_Filename(29,"YellowLorryLow.dff")

Clump_Filename(30,"WhiteArcticHigh.dff")
Clump_Filename(31,"WhiteArcticMid.dff")
Clump_Filename(32,"WhiteArcticLow.dff")
Clump_Filename(33,"GreenArcticHigh.dff")
Clump_Filename(34,"GreenArcticMid.dff")
Clump_Filename(35,"GreenArcticLow.dff")
Clump_Filename(36,"YellowArcticHigh.dff")
Clump_Filename(37,"YellowArcticMid.dff")
Clump_Filename(38,"YellowArcticLow.dff")

Clump_Filename(42,"Lampost.dff")
Clump_Filename(43,"Jet.dff")
Clump_Filename(44,"Wheels.dff")
Clump_Filename(45,"Arrows.dff")

Clump_Filename(46,"LorryBrakeLights.dff")
Clump_Filename(47,"TruckBrakeLights.dff")
Clump_Filename(48,"ArcticBrakeLights.dff")

Clump_Filename(50,"LorryBox.dff")
Clump_Filename(51,"CamMan.dff")

Clump_Exclude_From_World(0)
Clump_Exclude_From_World(1)
Clump_Exclude_From_World(2)
Clump_Exclude_From_World(3)
Clump_Exclude_From_World(4)
Clump_Exclude_From_World(5)
Clump_Exclude_From_World(6)
Clump_Exclude_From_World(7)
Clump_Exclude_From_World(8)
Clump_Exclude_From_World(9)
Clump_Exclude_From_World(10)
Clump_Exclude_From_World(11)
Clump_Exclude_From_World(12)
Clump_Exclude_From_World(13)
Clump_Exclude_From_World(14)
Clump_Exclude_From_World(15)
Clump_Exclude_From_World(16)
Clump_Exclude_From_World(17)

Clump_Exclude_From_World(18)
Clump_Exclude_From_World(19)
Clump_Exclude_From_World(20)
Clump_Exclude_From_World(21)
Clump_Exclude_From_World(22)
Clump_Exclude_From_World(23)
Clump_Exclude_From_World(24)
Clump_Exclude_From_World(25)
Clump_Exclude_From_World(26)
Clump_Exclude_From_World(27)
Clump_Exclude_From_World(28)
Clump_Exclude_From_World(29)

Clump_Exclude_From_World(30)
Clump_Exclude_From_World(31)
Clump_Exclude_From_World(32)
Clump_Exclude_From_World(33)
Clump_Exclude_From_World(34)
Clump_Exclude_From_World(35)
Clump_Exclude_From_World(36)
Clump_Exclude_From_World(37)
Clump_Exclude_From_World(38)

--Clump_Exclude_From_World(42)
Clump_Exclude_From_World(43)
Clump_Exclude_From_World(44)

Clump_Exclude_From_World(46)
Clump_Exclude_From_World(47)
Clump_Exclude_From_World(48)

Clump_Exclude_From_World(50)


UVA_Filename(0,"Arrows.uva")


Mts_Filename(0,"Lampost.mts")

--Spline_Filename(index,"filename.spl")
 
--General_Anim_Filename(index,"filename.anm")

--Camera_Anim_Filename("filename.anm")



Clump_Mts_Pair(42,0)

--Physics_Object(clump_index,mts_index,object_type)

--Clump_Exclude_From_World(7)


-------------------------------------------
-- RWP OBJECT VALUES ----------------------
-------------------------------------------
local MASS = 25
local FATNESS = 0
local BOUNCE = 0.0
local FRICTION = 1
local BASE_LABEL = 1000
local METAL = 3

--RWP_Object( 0, "Normal Lorry Box", "LorryBox.dff", "lorry_boxes.mts" )
--RWP_Object( 0, "Normal Lorry Box", "LorryBoxWith10Children.dff", "" )
--RWP_Object_Box( 0, 2.75, 4.65, 13.8 )
--RWP_Object_Properties( 0, FATNESS, BOUNCE, FRICTION, MASS , METAL )
--RWP_Object_BaseLabel( 0, BASE_LABEL )
--RWP_Object_ShatterVel( 0, 100 )

-- Copter Stuff ----------------------------------------------------

General_Anim_Filename( 0 ,"H_Anim_1.anm" )
General_Anim_Filename( 1 ,"H_Anim_2.anm" )


SetCopter("Copter" , 0 )
SetCopter("JetRanger" , 1 )
