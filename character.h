// #ifndef CHARACTER_H //if its not defined, then proceed to line 2 and define it.
// #define CHARACTER_H
//pragma once is a stronger method to ifndef <-> endif
//it makes sure that the header file is only included once
#pragma once
#include <stdio.h>
#include <gb/font.h>
#include <gb/console.h>
#include <gb/drawing.h>

typedef struct Character
{
    // When this is set to 1, the update_toad() function will be called.
    UBYTE updated;

    UINT8 x;
    UINT8 y;

    UBYTE direction;

    UINT8 sprite_index;

    UINT8 cig_shine_tile_index;

    // Body Metasprite
    UBYTE body_animate;
    UINT8 body_tile_index;
    UINT8 body_frame_index;
    UINT8 body_frame_delay;

} Character;
// #endif
//sort of like an end } of an if statement. If it is defined above, it will not be copied again