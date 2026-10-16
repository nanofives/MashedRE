-- course.lua
--Green

Course_Id(36)

AI_Bsp_Filename("lap.bsp")

World_Bsp_Filename("graphics.bsp")

Collision_Bsp_Filename("collisions.bsp")

Texture_Dictionary_Filename("textures.txd")

Lights_Filename("Lights.dff")



--Occluder_Filename("Occluder.dff")

--Powerup_Filename("filename.dff")

Sky_Filename(0,"sky.dff")

Setup_Fog(20,100,0,0,0)


Clump_Filename(0,"Water.dff")
Clump_Filename(1,"Alpha.dff")
--Clump_Filename(2,"Tree.dff")
Clump_Filename(3,"CamMan.dff")
Clump_Filename(4,"Mist.dff")
--Clump_Filename(5,"Leef.dff")
Clump_Filename(6,"Bush.dff")

UVA_Filename(0,"Water.uva")

--Mts_Filename(0,"Trees.mts")
Mts_Filename(1,"Bush.mts")

Spline_Filename(0,"Wave1.spl")
Spline_Filename(1,"Wave2.spl")
Spline_Filename(2,"Wave3.spl")
Spline_Filename(3,"Wave4.spl")
Spline_Filename(4,"Wave5.spl")
Spline_Filename(5,"Wave6.spl")
Spline_Filename(6,"Wave7.spl")
Spline_Filename(7,"Wave8.spl")
Spline_Filename(8,"Wave9.spl")

 
--General_Anim_Filename(index,"filename.anm")

--Camera_Anim_Filename("filename.anm")



Clump_Mts_Pair(6,1)

--Physics_Object(clump_index,mts_index,object_type)

Clump_Exclude_From_World(0)
Clump_Exclude_From_World(1)
--Clump_Exclude_From_World(2)
Clump_Exclude_From_World(4)
--Clump_Exclude_From_World(5)

RainEnable()