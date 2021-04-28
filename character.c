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

    UBYTE facing_right;
    UINT8 sprite_index;

    // Body Metasprite
    UBYTE body_animate;
    UINT8 body_tile_index;
    UINT8 body_frame_index;
    UINT8 body_frame_delay;

    // Smoke Metasprite
    UINT8 smoke_tile_index;
    UINT8 smoke_frame_index;
    UINT8 smoke_frame_delay;

} Character;