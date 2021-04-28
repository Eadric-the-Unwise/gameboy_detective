#include <stdio.h>
#include <gb/gb.h>
#include <gb/font.h>
#include <gb/console.h>
#include <gb/drawing.h>
#include <gb/metasprites.h>
#include "tiles/tile_detectivewalk.h"
#include "character.c"

UBYTE running = 1;
UINT8 joystate;

// Blank tile.
const unsigned char blank_data[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Sprite delays (60 = 1 second)
#define FRAME_DELAY 7
#define DETECTIVE_SMOKE_FRAME_DELAY 45

#define DETECTIVE_SMOKE_SPRITE_INDEX 0 // Metasprite size: 1 x 2 = 2 sprites
#define DETECTIVE_SMOKE_SPRITE_COUNT 2

#define DETECTIVE_BODY_SPRITE_INDEX 2 // Metasprite size: 2 x 3 = 6 sprites
#define DETECTIVE_BODY_SPRITE_COUNT 6

// Sprite frames
// DETECTIVE body stand
#define DETECTIVE_BODY_STAND_FRAME 0
// DETECTIVE body walk
#define DETECTIVE_BODY_WALK_FRAME_START 1
#define DETECTIVE_BODY_WALK_FRAME_END 3

void main(void)
{
    DISPLAY_ON;
    SHOW_BKG;
    SHOW_SPRITES;

    OBP1_REG = 0xE1;
    BGP_REG = 0xFF;

    Character detective;

    // Load blank sprite data.
    set_sprite_data(0, 1, blank_data);

    // Load detective sprite data.
    load_detective_data(&detective, 1);

    // Set initial detective values.
    setup_detective(&detective);

    while (running)
    {
        joypad_ex(&joypads);

        if (DETECTIVE.body_animate == 1 && detective.body_frame_delay == 0)
        {
            // Animate the body when detective is moving.
            detective.updated = 1;
            detective.body_frame_delay = FRAME_DELAY;
            detective.body_frame_index++;

            if (detective.body_frame_index > DETECTIVE_BODY_WALK_FRAME_END)
                // Reached the last frame. Reset to FRAME_START.
                detective.body_frame_index = DETECTIVE_BODY_WALK_FRAME_START;
        }
        if (detective.body_animate == 0 && detective.smoke_frame_delay == 0)
        {
            // Animate the smoke when detective stops moving.
            detective.updated = 1;
            detective.smoke_frame_delay = DETECTIVE_SMOKE_FRAME_DELAY;
            detective.smoke_frame_index++;
            // Number of frames = (sizeof(detectivesmoke_metasprites) >> 1)
            if (detective.smoke_frame_index >= (sizeof(detective_metasprites) >> 1))
                // Reached the last frame. Reset to first frame.
                detective.smoke_frame_index = 0;
        }

        if (joypads.joy0 & J_LEFT)
        {
            // Move left
            detective.facing_right = 0;
            if (can_detective_move(&detective, detective.x - 1, detective.y))
            {
                detective.updated = 1;
                detective.x -= 1;
                detective.body_animate = 1; // Set body animation to ON
            }
        }
        else if (joypads.joy0 & J_RIGHT)
        {
            // Move right
            detective.facing_right = 1;
            if (can_detective_move(&detective, detective.x + 1, detective.y))
            {
                detective.updated = 1;
                detective.x += 1;
                detective.body_animate = 1; // Set body animation to ON
            }
        }
        else
        {
            // Not moving
            if (detective.body_animate == 1 || detective.body_frame_index != DETECTIVE_BODY_STAND_FRAME)
            {
                // If body is animated OR body frame is not STAND_FRAME.

                // Make sure update_detective() is called.
                detective.updated = 1;

                // Stop body animation
                detective.body_animate = 0; // Set body animation to OFF
                detective.body_frame_index = DETECTIVE_BODY_STAND_FRAME;

                // Prepare smoke animation
                detective.smoke_frame_index = 0;
                detective.smoke_frame_delay = DETECTIVE_SMOKE_FRAME_DELAY;
            }
        }

        if (detective.updated == 1)
        {
            // If there's been any changes, update the metasprite.
            detective.updated = 0;
            update_detective(&detective, detective.x, detective.y);
        }

        // Decrement animation delays
        if (detective.body_frame_delay > 0)
            detective.body_frame_delay--;
        if (detective.smoke_frame_delay > 0)
            detective.smoke_frame_delay--;

        // Wait for vblank
        wait_vbl_done();
    }
}