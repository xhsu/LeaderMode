/*
==============================================================================

QC script generated by Half-Life MDL Decompiler 1.2 
2003, Kratisto. Based on code from Valve's HL SDK.

v_xxx.mdl

Original internal name:
"v_xxx.mdl"

==============================================================================
*/

$modelname "v_usp.mdl"
$cd ".\"
$cdtexture ".\"
$scale 1.5
$cliptotextures

$bbox 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
$cbox 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
$eyeposition 0.000000 0.000000 0.000000


$origin -0.5 -1 -0.5


//reference mesh(es)

$body "hands" "righthand"

$bodygroup "lHand"
{
studio "lefthand"
blank
}

$body "hands" "rightsleeve"

$bodygroup "lsleeve"
{
studio "leftsleeve"
blank
}

$body "weapon" "usp"

$bodygroup "slide"
{
studio "slide_forward"
studio "slide_back"
}

$bodygroup "mag"
{
studio "mag_full"
studio "mag_empty"
}

$bodygroup "barrel"
{
studio "barrel_normal"
studio "barrel_empty"
}

$body "hammer" "hammer_ready"

$body "weapon" "newMag"

$bodygroup "laser"
{
blank
studio "miniLaser"
}

$bodygroup "scope"
{
blank
studio "miniScope"
studio "miniScope_back"
}




// x attachment(s)
$attachment 0 "bone_weapon" -6.300000 1.800000 0.000000
$attachment 1 "bone_weapon" -2.500000 2.000000 -2.000000

// x animation sequence(s)
$sequence "idle" "idle" fps 30 
$sequence "fire" "fire" fps 30 
$sequence "fire_last" "fire_last" fps 30
$sequence "select" "select" fps 30 
$sequence "select_first" "select_first" fps 30 
$sequence "holster" "holster" fps 30 
$sequence "reload" "reload" fps 30 
$sequence "reload_empty" "reload_empty" fps 30 
$sequence "checkMag" "checkMag" fps 30 
$sequence "hand_off" "hand_off" fps 30 
$sequence "hand_on" "hand_on" fps 30 
$sequence "block_up" "block_up" fps 30 
$sequence "block_down" "block_down" fps 30 
$sequence "dash_enter" "dash_enter" fps 30 
$sequence "dash_idle" "dash_idle" fps 30 
$sequence "dash_out" "dash_out" fps 30 
// End of QC script.