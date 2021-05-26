// #ifndef CHARACTER_H //if its not defined, then proceed to line 2 and define it.
// #define CHARACTER_H
//pragma once is a stronger method to ifndef <-> endif
#pragma once
#include <gb/gb.h>

typedef struct CharacterSmoke
{
    UINT8 state;

    UINT8 x;
    UINT8 y;

    UBYTE direction;

    // Body Metasprite
    UBYTE body_animate;

    // Smoke Metasprite
    UINT8 smoke_frame_index;
    UINT8 smoke_frame_delay;

} CharacterSmoke;
// #endif
//sort of like an end } of an if statement. If it is defined above, it will not be copied again