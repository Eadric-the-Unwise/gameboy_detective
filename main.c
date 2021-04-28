#include <stdio.h>
#include <gb/gb.h>
#include <gb/font.h>
#include <gb/console.h>
#include <gb/drawing.h>
#include <gb/metasprites.h>
#include "tiles/tile_detectivewalk.h"
#include "character.c"

UBYTE running = 1;
joypads_t joypads;
Character detective;

// Blank tile. (h3's workaround to hide the smoke bc blank_tile wasnt working)
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
#define DETECTIVE_BODY_WALK_FRAME_START 0
#define DETECTIVE_BODY_WALK_FRAME_END 3

void load_detective_data(Character *detective, UINT8 first_tile)
{
    // Body tiles
    detective->body_tile_index = first_tile;
    // Number of tiles = (sizeof(tile_detectivewalk_data) >> 4)
    set_sprite_data(detective->body_tile_index, (sizeof(tile_detectivewalk_data) >> 4), tile_detectivewalk_data);

    // Smoke tiles
    // detective->smoke_tile_index = detective->body_tile_index + (sizeof(tile_detectivewalk_data) >> 4);
    // // Number of tiles = (sizeof(detectivesmoke_data) >> 4)
    // set_sprite_data(detective->smoke_tile_index, (sizeof(detectivesmoke_data) >> 4), detectivesmoke_data);
}

//blocks detective from walking off of the screen
UBYTE can_detective_move(Character *detective, UINT8 x, UINT8 y)
{
    return x >= 16 && x <= 160;
}

void update_detective(Character *detective, UINT8 x, UINT8 y)
{
    UINT8 i;

    if (detective->facing_right == 0)
    {
        // Facing left
        move_metasprite(tile_detectivewalk_metasprites[detective->body_frame_index], detective->body_tile_index, DETECTIVE_BODY_SPRITE_INDEX, x, y);
    }
    else
    {
        // Facing right (Flip the sprites)
        move_metasprite_vflip(tile_detectivewalk_metasprites[detective->body_frame_index], detective->body_tile_index, DETECTIVE_BODY_SPRITE_INDEX, x, y);
    }

    if (detective->body_animate == 1)
    {
        // Body animation is ON
        // Hide the smoke
        for (i = 0; i < DETECTIVE_SMOKE_SPRITE_COUNT; i++)
        {
            set_sprite_tile(DETECTIVE_SMOKE_SPRITE_INDEX + i, 0);
        }
    }
    // else
    // {
    //     // Body animation is OFF
    //     // Show the smoke (the x-4 and y-4 is to offset the smoke sprites so they're positioned correctly)
    //     move_metasprite(detectivesmoke_metasprites[detective->smoke_frame_index], detective->smoke_tile_index, DETECTIVE_SMOKE_SPRITE_INDEX, x - 4, y - 4);
    // }
}

void setup_detective(Character *detective)
{
    // Make sure update_detective() is called.
    detective->updated = 1;

    // Initial position
    detective->x = 100;
    detective->y = 130;

    detective->facing_right = 0; // Set to RIGHT

    // detective body
    detective->body_animate = 0; // Set to OFF
    detective->body_frame_index = DETECTIVE_BODY_STAND_FRAME;
    detective->body_frame_delay = 0;

    // detective smoke
    detective->smoke_frame_index = 0;
    detective->smoke_frame_delay = DETECTIVE_SMOKE_FRAME_DELAY;
}

void main(void)
{
    DISPLAY_ON;
    SHOW_BKG;
    SHOW_SPRITES;

    OBP1_REG = 0xE1;
    BGP_REG = 0x1B;

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

        if (detective.body_animate == 1 && detective.body_frame_delay == 0)
        {
            // Animate the body when detective is moving.
            detective.updated = 1;
            detective.body_frame_delay = FRAME_DELAY;
            detective.body_frame_index++;

            if (detective.body_frame_index > DETECTIVE_BODY_WALK_FRAME_END)
                // Reached the last frame. Reset to FRAME_START.
                detective.body_frame_index = DETECTIVE_BODY_WALK_FRAME_START;

            detective.body_frame_delay = detective.body_frame_index % 2 ? FRAME_DELAY * 2 : FRAME_DELAY;

            // if (detective.body_tile_index ==)
        }
        // if (detective.body_animate == 0 && detective.smoke_frame_delay == 0)
        // {
        //     // Animate the smoke when detective stops moving.
        //     detective.updated = 1;
        //     detective.smoke_frame_delay = DETECTIVE_SMOKE_FRAME_DELAY;
        //     detective.smoke_frame_index++;
        //     // Number of frames = (sizeof(detectivesmoke_metasprites) >> 1)
        //     if (detective.smoke_frame_index >= (sizeof(tile_detectivesmoke_metasprites) >> 1))
        //         // Reached the last frame. Reset to first frame.
        //         detective.smoke_frame_index = 0;
        // }

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