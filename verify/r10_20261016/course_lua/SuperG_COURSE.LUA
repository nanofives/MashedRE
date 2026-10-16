-- course.lua
-- SuperG
Course_Id(25)

-- World Stuff -----------------------------

AI_Bsp_Filename("lap.bsp")
World_Bsp_Filename("graphics.bsp")
Collision_Bsp_Filename("collisions.bsp")
Texture_Dictionary_Filename("textures.txd")
Lights_Filename("Lights.dff")
Setup_Fog(0,120,255,255,255)

--Sky_Filename(index,"sky.dff")
--Occluder_Filename("Occluder.dff")
--Spline_Filename(index,"filename.spl")
--General_Anim_Filename(index,"filename.anm")
--Camera_Anim_Filename("filename.anm")


-- Dff's ----------------------------------

Clump_Filename(0,"sea.dff")
Clump_Filename(1,"Reflections.dff")
Clump_Filename(2,"CamMan.dff")

Clump_Exclude_From_World(0)
Clump_Exclude_From_World(1)

-- Vheicle Commands ----------------------

--Shadow_Scale(x)

-- MTS Stuff -----------------------------

--Mts_Filename(index,"filename.mts")

--Clump_Mts_Pair(clump_index,mts_index)


-- Copter Stuff ----------------------------------------------------

General_Anim_Filename( 0 ,"H_Anim_1.anm" )
General_Anim_Filename( 1 ,"H_Anim_2.anm" )
--General_Anim_Filename( 2 ,"H_Anim_3.anm" )
--General_Anim_Filename( 3 ,"H_Anim_4.anm" )
--General_Anim_Filename( 4 ,"H_Anim_5.anm" )

SetCopter("Copter" , 0 )
SetCopter("JetRanger" , 1 )
--SetCopter("Copter" , 2 )
--SetCopter("CivHuey" , 3 )
--SetCopter("CivHuey" , 4 )


