// #ifndef CHARACTER_H //if its not defined, then proceed to line 2 and define it.
// #define CHARACTER_H
//pragma once is a stronger method to ifndef <-> endif
#pragma once
#include <gb/gb.h>
#include <stdio.h>
#include <gb/font.h>
#include <gb/console.h>
#include <gb/drawing.h>

typedef struct CharacterSmoke
{
    UINT8 state;

    UINT8 x;
    UINT8 y;

    UBYTE facing_right;
    UINT8 sprite_index;
    UBYTE direction;
    UBYTE facing_LR;

    UINT8 cig_shine_tile_index;

    // Body Metasprite
    UBYTE body_animate;
    UINT8 body_tile_index;
    UINT8 body_frame_index;
    UINT8 body_frame_delay;

    // Smoke Metasprite
    UINT8 smoke_tile_index;
    UINT8 smoke_frame_start;
    UINT8 smoke_frame_end;
    UINT8 smoke_frame_index;
    UINT8 smoke_frame_delay;
    UINT8 smoke_start_delay;

} CharacterSmoke;
// #endif
//sort of like an end } of an if statement. If it is defined above, it will not be copied again