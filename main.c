#include <stdio.h>
#include <gb/gb.h>
#include <gb/font.h>
#include <gb/console.h>
#include <gb/drawing.h>
#include <gb/metasprites.h>
#include "tiles/tile_detectivewalk.h"
#include "tiles/cig_shine.h"
#include "tiles/smoke.h"
#include "character.h"

UBYTE running = 1;
joypads_t joypads;
Character detective;
Character smoke;

// Blank tile. (h3's workaround to hide the smoke bc blank_tile wasnt working)
const unsigned char blank_data[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Sprite delays (60 = 1 second)
#define FRAME_DELAY 7
#define SMOKE_DELAY 9       //walking smoke animation frame delay
#define SMOKE_IDLE_DELAY 11 //standing smoke animation frame delay
#define SMOKE_START_DELAY 8 //pause time between smoke puffs

//SPRITE INDEX IS NOT RELATED TO THE TILE INDEX. These are used to load the Sprite OBJ's into OAM in order
//0 in the OAM
#define DETECTIVE_BODY_SPRITE_INDEX 4
//amount of sprites
// #define DETECTIVE_BODY_SPRITE_COUNT 4

#define DETECTIVE_CIG_SHINE_SPRITE_INDEX 0 //OAM
#define DETECTIVE_SMOKE_SPRITE_INDEX 2     //OAM
#define SMOKE_BODY_SPRITE_INDEX 12
#define SMOKE_CIG_SHINE_SPRITE_INDEX 8
#define SMOKE_SMOKE_SPRITE_INDEX 10

#define DETECTIVE_SMOKE_SPRITE_COUNT 2
// DETECTIVE body stand
#define DETECTIVE_BODY_STAND_FRAME 0
// DETECTIVE body walk
#define DETECTIVE_BODY_WALK_FRAME_START 0
#define DETECTIVE_BODY_WALK_FRAME_END 3
#define DETECTIVE_SMOKE_STAND_FRAME_START 0
#define DETECTIVE_SMOKE_STAND_FRAME_END 5
#define DETECTIVE_SMOKE_WALK_FRAME_START 6
#define DETECTIVE_SMOKE_WALK_FRAME_END 11

#define TILE_SIZE 8

void load_detective_data(Character *detective, UINT8 first_tile)
{
    // Body tiles
    detective->body_tile_index = first_tile;
    // Number of tiles = (sizeof(tile_detectivewalk_data) >> 4)
    set_sprite_data(detective->body_tile_index, (sizeof(tile_detectivewalk_data) >> 4), tile_detectivewalk_data);
    //cig_shine index
    detective->cig_shine_tile_index = detective->body_tile_index + (sizeof(tile_detectivewalk_data) >> 4);
    set_sprite_data(detective->cig_shine_tile_index, (sizeof(cig_shine_data) >> 4), cig_shine_data);
    //smoke index loads in the OAM from where the cig_shine is loading, + the number of tiles of cig_shine
    detective->smoke_tile_index = detective->cig_shine_tile_index + (sizeof(cig_shine_data) >> 4);
    set_sprite_data(detective->smoke_tile_index, (sizeof(smoke_data) >> 4), smoke_data);
}

void load_smoke_data(Character *smoke, UINT8 first_tile)
{
    // Body tiles
    smoke->body_tile_index = first_tile;
    // Number of tiles = (sizeof(tile_detectivewalk_data) >> 4)
    set_sprite_data(smoke->body_tile_index, (sizeof(tile_detectivewalk_data) >> 4), tile_detectivewalk_data);
    //cig_shine index
    smoke->cig_shine_tile_index = smoke->body_tile_index + (sizeof(tile_detectivewalk_data) >> 4);
    set_sprite_data(smoke->cig_shine_tile_index, (sizeof(cig_shine_data) >> 4), cig_shine_data);
    //smoke index loads in the OAM from where the cig_shine is loading, + the number of tiles of cig_shine
    smoke->smoke_tile_index = smoke->cig_shine_tile_index + (sizeof(cig_shine_data) >> 4);
    set_sprite_data(smoke->smoke_tile_index, (sizeof(smoke_data) >> 4), smoke_data);
}
//blocks detective from walking off of the screen
UBYTE can_detective_move(UINT8 x, UINT8 y)
{
    //since y can never be negative, we dont need to add && y >= 0
    //unsigned is always a positive integer
    return x >= 16 && x <= 160 && y >= 26 && y <= 150;
}

void update_detective(Character *detective, UINT8 x, UINT8 y)
{
    for (UBYTE i = DETECTIVE_SMOKE_SPRITE_INDEX; i < 4; i++)
        shadow_OAM[i].y = 0;
    if (detective->facing_right == 0)
    {
        // Facing left
        move_metasprite(tile_detectivewalk_metasprites[detective->body_frame_index], detective->body_tile_index, DETECTIVE_BODY_SPRITE_INDEX, x, y);
        move_metasprite(cig_shine_metasprites[detective->body_frame_index], detective->cig_shine_tile_index, DETECTIVE_CIG_SHINE_SPRITE_INDEX, x, y);
        move_metasprite(smoke_metasprites[detective->smoke_frame_index], detective->smoke_tile_index, DETECTIVE_SMOKE_SPRITE_INDEX, x + TILE_SIZE, y - TILE_SIZE);
    }
    else
    {
        // Facing right (Flip the sprites)
        move_metasprite_vflip(tile_detectivewalk_metasprites[detective->body_frame_index], detective->body_tile_index, DETECTIVE_BODY_SPRITE_INDEX, x, y);
        move_metasprite_vflip(cig_shine_metasprites[detective->body_frame_index], detective->cig_shine_tile_index, DETECTIVE_CIG_SHINE_SPRITE_INDEX, x, y);
        move_metasprite_vflip(smoke_metasprites[detective->smoke_frame_index], detective->smoke_tile_index, DETECTIVE_SMOKE_SPRITE_INDEX, x - TILE_SIZE, y - TILE_SIZE);
    }
}

void update_smoke(Character *smoke, UINT8 x, UINT8 y)
{
    for (UBYTE i = SMOKE_SMOKE_SPRITE_INDEX; i < 12; i++)
        shadow_OAM[i].y = 0;
    if (smoke->facing_right == 0)
    {
        // Facing left
        move_metasprite(tile_detectivewalk_metasprites[smoke->body_frame_index], smoke->body_tile_index, SMOKE_BODY_SPRITE_INDEX, x, y);
        move_metasprite(cig_shine_metasprites[smoke->body_frame_index], smoke->cig_shine_tile_index, SMOKE_CIG_SHINE_SPRITE_INDEX, x, y);
        move_metasprite(smoke_metasprites[smoke->smoke_frame_index], smoke->smoke_tile_index, SMOKE_SMOKE_SPRITE_INDEX, x + TILE_SIZE, y - TILE_SIZE);
    }
    else
    {
        // Facing right (Flip the sprites)
        move_metasprite_vflip(tile_detectivewalk_metasprites[smoke->body_frame_index], smoke->body_tile_index, SMOKE_BODY_SPRITE_INDEX, x, y);
        move_metasprite_vflip(cig_shine_metasprites[smoke->body_frame_index], smoke->cig_shine_tile_index, SMOKE_CIG_SHINE_SPRITE_INDEX, x, y);
        move_metasprite_vflip(smoke_metasprites[smoke->smoke_frame_index], smoke->smoke_tile_index, SMOKE_SMOKE_SPRITE_INDEX, x - TILE_SIZE, y - TILE_SIZE);
    }
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
    detective->smoke_frame_index = DETECTIVE_SMOKE_STAND_FRAME_START;
    detective->smoke_frame_delay = 0;
    detective->smoke_start_delay = 0;
}

void setup_smoke(Character *smoke)
{
    // Make sure update_detective() is called.
    smoke->updated = 1;

    // Initial position
    smoke->x = 70;
    smoke->y = 130;

    smoke->facing_right = 0; // Set to RIGHT

    // detective body
    smoke->body_animate = 0; // Set to OFF
    smoke->body_frame_index = DETECTIVE_BODY_STAND_FRAME;
    smoke->body_frame_delay = 0;
    smoke->smoke_frame_index = DETECTIVE_SMOKE_STAND_FRAME_START;
    smoke->smoke_frame_delay = 0;
    smoke->smoke_start_delay = 0;
}
void main(void)
{
    DISPLAY_ON;
    SHOW_BKG;
    SHOW_SPRITES;

    OBP1_REG = 0xE1;
    BGP_REG = 0x1B;

    Character detective;
    Character smoke;

    // Load blank sprite data.
    set_sprite_data(0, 1, blank_data);

    // Load detective sprite data.
    load_detective_data(&detective, 1);
    load_smoke_data(&smoke, 0x1B);

    // Set initial detective values.
    setup_detective(&detective);
    setup_smoke(&smoke);

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
        }

        if (detective.smoke_frame_delay == 0)
        {
            // Animate the smoke when detective is moving.
            detective.updated = 1;
            detective.smoke_frame_delay = SMOKE_DELAY;
            if (detective.body_animate == 1)
            {
                detective.smoke_frame_delay = SMOKE_DELAY;
            }
            else
                detective.smoke_frame_delay = SMOKE_IDLE_DELAY;

            if (detective.smoke_frame_index == DETECTIVE_SMOKE_WALK_FRAME_START || detective.smoke_frame_index == DETECTIVE_SMOKE_STAND_FRAME_START)
            {
                detective.smoke_start_delay++;

                if (detective.smoke_start_delay > SMOKE_START_DELAY)
                {
                    detective.smoke_start_delay = 0;
                }
            }

            if (detective.smoke_start_delay == 0)
            {
                detective.smoke_frame_index++;

                //IF detective.body.animate = 1, then load DETECTIVE_SMOKE_WALK_FRAME_END. Now ====> detective.smoke_frame_index > DETECTIVE_SMOKE_WALK_FRAME_END
                if (detective.smoke_frame_index > (detective.body_animate ? DETECTIVE_SMOKE_WALK_FRAME_END : DETECTIVE_SMOKE_STAND_FRAME_END))
                    // Reached the last frame. Reset to FRAME_START.

                    detective.smoke_frame_index = detective.body_animate ? DETECTIVE_SMOKE_WALK_FRAME_START : DETECTIVE_SMOKE_STAND_FRAME_START;
            }
        }
        //SMOKE ANIMATION BELOW
        if (smoke.body_animate == 1 && smoke.body_frame_delay == 0)
        {
            // Animate the body when detective is moving.
            smoke.updated = 1;
            smoke.body_frame_delay = FRAME_DELAY;
            smoke.body_frame_index++;

            if (smoke.body_frame_index > DETECTIVE_BODY_WALK_FRAME_END)
                // Reached the last frame. Reset to FRAME_START.
                smoke.body_frame_index = DETECTIVE_BODY_WALK_FRAME_START;

            smoke.body_frame_delay = smoke.body_frame_index % 2 ? FRAME_DELAY * 2 : FRAME_DELAY;
        }

        if (smoke.smoke_frame_delay == 0)
        {
            // Animate the smoke when detective is moving.
            smoke.updated = 1;
            smoke.smoke_frame_delay = SMOKE_DELAY;
            if (smoke.body_animate == 1)
            {
                smoke.smoke_frame_delay = SMOKE_DELAY;
            }
            else
                smoke.smoke_frame_delay = SMOKE_IDLE_DELAY;

            if (smoke.smoke_frame_index == DETECTIVE_SMOKE_WALK_FRAME_START || smoke.smoke_frame_index == DETECTIVE_SMOKE_STAND_FRAME_START)
            {
                smoke.smoke_start_delay++;

                if (smoke.smoke_start_delay > SMOKE_START_DELAY)
                {
                    smoke.smoke_start_delay = 0;
                }
            }

            if (smoke.smoke_start_delay == 0)
            {
                smoke.smoke_frame_index++;

                //IF smoke.body.animate = 1, then load DETECTIVE_SMOKE_WALK_FRAME_END. Now ====> smoke.smoke_frame_index > DETECTIVE_SMOKE_WALK_FRAME_END
                if (smoke.smoke_frame_index > (smoke.body_animate ? DETECTIVE_SMOKE_WALK_FRAME_END : DETECTIVE_SMOKE_STAND_FRAME_END))
                    // Reached the last frame. Reset to FRAME_START.

                    smoke.smoke_frame_index = smoke.body_animate ? DETECTIVE_SMOKE_WALK_FRAME_START : DETECTIVE_SMOKE_STAND_FRAME_START;
            }
        }

        if (joypads.joy0 & J_LEFT)
        {

            // Move left
            if (detective.facing_right == 1)
            { // if previously facing right...
                // ...change to facing left
                detective.updated = 1;
                detective.facing_right = 0;
                detective.smoke_frame_index = DETECTIVE_SMOKE_WALK_FRAME_START;
            }

            if (can_detective_move(detective.x - 1, detective.y))
            {
                detective.updated = 1;
                detective.x -= 1;
                if (detective.body_animate == 0)
                {
                    // started moving for the first time
                    detective.body_animate = 1;
                    detective.smoke_frame_index = DETECTIVE_SMOKE_WALK_FRAME_START;
                }
            }
        }
        else if (joypads.joy0 & J_RIGHT)
        {
            // Move right
            if (detective.facing_right == 0)
            { // if previously facing left...
                // ...change to facing right
                detective.updated = 1;
                detective.facing_right = 1;
                detective.smoke_frame_index = DETECTIVE_SMOKE_WALK_FRAME_START;
            }

            if (can_detective_move(detective.x + 1, detective.y))
            {
                detective.updated = 1;
                detective.x += 1;
                if (detective.body_animate == 0)
                {
                    // started moving for the first time
                    detective.body_animate = 1;
                    detective.smoke_frame_index = DETECTIVE_SMOKE_WALK_FRAME_START;
                }
            }
        }
        if (joypads.joy0 & J_UP)
        {

            if (can_detective_move(detective.x, detective.y - 1))
            {
                detective.updated = 1;
                detective.y -= 1;
                if (detective.body_animate == 0)
                {
                    // started moving for the first time
                    detective.body_animate = 1;
                    detective.smoke_frame_index = DETECTIVE_SMOKE_WALK_FRAME_START;
                }
            }
            if (joypads.joy0 & J_RIGHT)
            {
            }
        }

        else if (joypads.joy0 & J_DOWN)
        {

            if (can_detective_move(detective.x, detective.y + 1))
            {
                detective.updated = 1;
                detective.y += 1;
                if (detective.body_animate == 0)
                {
                    // started moving for the first time
                    detective.body_animate = 1;
                    detective.smoke_frame_index = DETECTIVE_SMOKE_WALK_FRAME_START;
                }
            }
        }
        //~ means "not"
        if (!(joypads.joy0 & (J_LEFT | J_RIGHT | J_UP | J_DOWN)))
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
                // detective.smoke_frame_index = DETECTIVE_SMOKE_STAND_FRAME_START;
            }
        }
        // SMOKE Not moving
        if (smoke.body_animate == 1 || smoke.body_frame_index != DETECTIVE_BODY_STAND_FRAME)
        {
            // If body is animated OR body frame is not STAND_FRAME.

            // Make sure update_detective() is called.
            smoke.updated = 1;

            // Stop body animation
            smoke.body_animate = 0; // Set body animation to OFF
            smoke.body_frame_index = DETECTIVE_BODY_STAND_FRAME;
            // smoke.smoke_frame_index = DETECTIVE_SMOKE_STAND_FRAME_START;
        }

        if (detective.updated == 1)
        {
            // If there's been any changes, update the metasprite.
            detective.updated = 0;
            update_detective(&detective, detective.x, detective.y);
        }

        if (smoke.updated == 1)
        {
            // If there's been any changes, update the metasprite.
            smoke.updated = 0;
            update_smoke(&smoke, smoke.x, smoke.y);
        }
        // The amount of delay between frame animation. Decrement animation delays
        if (detective.body_frame_delay > 0)
            detective.body_frame_delay--;

        if (detective.smoke_frame_delay > 0)
            detective.smoke_frame_delay--;

        if (smoke.body_frame_delay > 0)
            smoke.body_frame_delay--;

        if (smoke.smoke_frame_delay > 0)
            smoke.smoke_frame_delay--;

        wait_vbl_done();
    }
}