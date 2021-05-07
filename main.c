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
#include "character_smoke.h"
#include "macros.h"

UBYTE running = 1;
joypads_t joypads;
Character detective;
CharacterSmoke smoke;

// Blank tile. (h3's workaround to hide the smoke bc blank_tile wasnt working)
const unsigned char blank_data[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void load_detective_data(Character *detective, UINT8 first_tile)
{
    // Body tiles
    detective->body_tile_index = first_tile;
    // Number of tiles = (sizeof(tile_detectivewalk_data) >> 4)
    set_sprite_data(detective->body_tile_index, (sizeof(tile_detectivewalk_data) >> 4), tile_detectivewalk_data);
    //cig_shine index
    detective->cig_shine_tile_index = detective->body_tile_index + (sizeof(tile_detectivewalk_data) >> 4);
    set_sprite_data(detective->cig_shine_tile_index, (sizeof(cig_shine_data) >> 4), cig_shine_data);
}

void load_smoke_data(CharacterSmoke *smoke, UINT8 first_tile)
{
    first_tile += (sizeof(tile_detectivewalk_data) >> 4) + (sizeof(cig_shine_data) >> 4);
    //smoke index loads in the OAM + the tile number of Detective + cig_shine tiles
    smoke->smoke_tile_index = first_tile;
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

    for (UBYTE i = DETECTIVE_CIG_SHINE_SPRITE_INDEX; i < DETECTIVE_CIG_SHINE_TILE_COUNT; i++)
        shadow_OAM[i].y = 0;

    if (detective->direction != FACE_RIGHT)
    {
        // NOT FACING RIGHT
        move_metasprite(tile_detectivewalk_metasprites[detective->body_frame_index], detective->body_tile_index, DETECTIVE_BODY_SPRITE_INDEX, x, y);
        if (detective->direction == FACE_LEFT)
        {
            move_metasprite(cig_shine_metasprites[detective->body_frame_index], detective->cig_shine_tile_index, DETECTIVE_CIG_SHINE_SPRITE_INDEX, x, y);
        }
        // move_metasprite(smoke_metasprites[detective->smoke_frame_index], detective->smoke_tile_index, DETECTIVE_SMOKE_SPRITE_INDEX, x + TILE_SIZE, y - TILE_SIZE);
    }
    else
    {
        // FACE_RIGHT (Flip the sprites)
        move_metasprite_vflip(tile_detectivewalk_metasprites[detective->body_frame_index], detective->body_tile_index, DETECTIVE_BODY_SPRITE_INDEX, x, y);
        move_metasprite_vflip(cig_shine_metasprites[detective->body_frame_index], detective->cig_shine_tile_index, DETECTIVE_CIG_SHINE_SPRITE_INDEX, x, y);
        // move_metasprite_vflip(smoke_metasprites[detective->smoke_frame_index], detective->smoke_tile_index, DETECTIVE_SMOKE_SPRITE_INDEX, x - TILE_SIZE, y - TILE_SIZE);
    }
}

void update_smoke(CharacterSmoke *smoke, UINT8 x, UINT8 y)
{

    for (UBYTE i = SMOKE_SMOKE_SPRITE_INDEX; i < SMOKE_TILE_COUNT; i++)
        shadow_OAM[i].y = 0;

    if (smoke->direction == FACE_LEFT)
    {
        move_metasprite(smoke_metasprites[smoke->smoke_frame_index], smoke->smoke_tile_index, SMOKE_SMOKE_SPRITE_INDEX, x + TILE_SIZE, y - TILE_SIZE);
    }
    else if (smoke->direction == FACE_RIGHT)
    {
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

    detective->direction = FACE_LEFT; // Set to LEFT

    // detective body
    detective->body_animate = 0; // Set to OFF
    detective->body_frame_index = DETECTIVE_BODY_STAND_FRAME;
    detective->body_frame_delay = 0;
}

void setup_smoke(CharacterSmoke *smoke)
{
    // Make sure update_detective() is called.
    smoke->updated = 1;

    // Initial position
    smoke->x = 70;
    smoke->y = 50;

    smoke->facing_right = 0; // Set to LEFT

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
    CharacterSmoke smoke;

    // Load blank sprite data.
    set_sprite_data(0, 1, blank_data);

    // Load detective sprite data.
    load_detective_data(&detective, 1);
    load_smoke_data(&smoke, 1);

    // Set initial detective values.
    setup_detective(&detective);
    setup_smoke(&smoke);

    joypad_init(1, &joypads);

    while (running)
    {

        joypad_ex(&joypads);

        if (detective.body_animate == 1 && detective.body_frame_delay == 0)
        {
            // Animate the body when detective is moving.
            detective.updated = 1;
            detective.body_frame_delay = FRAME_DELAY;
            detective.body_frame_index++;

            if (detective.direction == FACE_LEFT || detective.direction == FACE_RIGHT)
            {
                if (detective.body_frame_index > DETECTIVE_BODY_WALK_FRAME_END)
                {
                    detective.body_frame_index = DETECTIVE_BODY_WALK_FRAME_START;
                }
            }
            else if (detective.direction == FACE_UP)
            {
                if (detective.body_frame_index > DETECTIVE_BODY_UP_FRAME_END)
                {
                    detective.body_frame_index = DETECTIVE_BODY_UP_FRAME_START;
                }
            }
            else if (detective.direction == FACE_DOWN)
            {
                if (detective.body_frame_index > DETECTIVE_BODY_DOWN_FRAME_END)
                {
                    detective.body_frame_index = DETECTIVE_BODY_DOWN_FRAME_START;
                }
            }

            detective.body_frame_delay = detective.body_frame_index % 2 ? FRAME_DELAY * 2 : FRAME_DELAY;
        }

        //SMOKE ANIMATION BELOW
        if (smoke.smoke_start_delay == 0)
        {

            if (smoke.smoke_frame_delay == 0 || smoke.smoke_frame_index == 0)
            {

                // Animate the smoke when detective is moving.
                smoke.updated = 1;

                smoke.smoke_frame_delay = smoke.body_animate ? SMOKE_DELAY : SMOKE_IDLE_DELAY;

                if (smoke.smoke_frame_index == 0)
                {
                    smoke.x = detective.x;
                    smoke.y = detective.y;
                    smoke.facing_right = detective.direction == FACE_RIGHT; //returns a 0 or 1
                    smoke.body_animate = detective.body_animate;
                    smoke.smoke_frame_index = smoke.body_animate ? DETECTIVE_SMOKE_WALK_FRAME_START : DETECTIVE_SMOKE_STAND_FRAME_START;
                }

                // if (smoke.smoke_start_delay == 0)
                // {}
                smoke.smoke_frame_index++;

                //IF smoke.body.animate = 1, then load DETECTIVE_SMOKE_WALK_FRAME_END. Now ====> smoke.smoke_frame_index > DETECTIVE_SMOKE_WALK_FRAME_END
                if (smoke.smoke_frame_index > (smoke.body_animate ? DETECTIVE_SMOKE_WALK_FRAME_END : DETECTIVE_SMOKE_STAND_FRAME_END))
                // Reached the last frame. Reset to FRAME_START.
                {
                    smoke.smoke_frame_index = 0;
                    smoke.direction = detective.direction;
                    smoke.smoke_start_delay = (detective.body_animate ? SMOKE_WALK_START_DELAY : SMOKE_IDLE_START_DELAY);
                }
            }
        }
        if (joypads.joy0 & J_LEFT)
        {

            // Move left
            if (detective.direction != FACE_LEFT)
            { // if previously facing right...
                // ...change to facing left
                detective.updated = 1;
                detective.direction = FACE_LEFT;
                detective.body_frame_index = DETECTIVE_BODY_WALK_FRAME_START;
                detective.body_frame_delay = 0;
            }

            if (can_detective_move(detective.x - 1, detective.y))
            {
                detective.updated = 1;
                detective.x -= 1;
                if (detective.body_animate == 0)
                {
                    // started moving for the first time
                    detective.body_animate = 1;
                }
            }
        }
        else if (joypads.joy0 & J_RIGHT)
        {
            // Move right
            if (detective.direction != FACE_RIGHT)
            {
                detective.updated = 1;
                detective.direction = FACE_RIGHT;
                detective.body_frame_index = DETECTIVE_BODY_WALK_FRAME_START;
                detective.body_frame_delay = 0;
            }

            if (can_detective_move(detective.x + 1, detective.y))
            {
                detective.updated = 1;
                detective.x += 1;
                if (detective.body_animate == 0)
                {
                    // started moving for the first time
                    detective.body_animate = 1;
                }
            }
        }
        if (joypads.joy0 & J_UP)
        {
            // Move up
            if (detective.direction != FACE_UP && !(joypads.joy0 & (J_LEFT | J_RIGHT)))
            {
                detective.updated = 1;
                detective.direction = FACE_UP;
                detective.body_frame_index = DETECTIVE_BODY_UP_FRAME_START;
                detective.body_frame_delay = 0;
            }

            if (can_detective_move(detective.x, detective.y - 1))
            {
                detective.updated = 1;
                detective.y -= 1;

                if (detective.body_animate == 0)
                {
                    // started moving for the first time
                    detective.body_animate = 1;
                }
            }
        }

        else if (joypads.joy0 & J_DOWN)
        {

            // Move down
            if (detective.direction != FACE_DOWN && !(joypads.joy0 & (J_LEFT | J_RIGHT)))
            {
                detective.updated = 1;
                detective.direction = FACE_DOWN;
                detective.body_frame_index = DETECTIVE_BODY_DOWN_FRAME_START;
                detective.body_frame_delay = 0;
            }
            if (can_detective_move(detective.x, detective.y + 1))
            {
                detective.updated = 1;
                detective.y += 1;
                if (detective.body_animate == 0)
                {
                    // started moving for the first time
                    detective.body_animate = 1;
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

                if (detective.direction == FACE_LEFT || detective.direction == FACE_RIGHT)
                {
                    detective.body_frame_index = DETECTIVE_BODY_STAND_FRAME;
                }
                else if (detective.direction == FACE_UP)
                {
                    detective.body_frame_index = DETECTIVE_BODY_UP_STAND_FRAME;
                }
                else if (detective.direction == FACE_DOWN)
                {
                    detective.body_frame_index = DETECTIVE_BODY_DOWN_STAND_FRAME;
                }
            }
            // SMOKE Not moving
            if (smoke.body_animate == 1 || smoke.body_frame_index != DETECTIVE_BODY_STAND_FRAME)
            {
                // If body is animated OR body frame is not STAND_FRAME.

                // Make sure update_detective() is called.
                smoke.updated = 1;

                // Stop body animation

                smoke.body_frame_index = DETECTIVE_BODY_STAND_FRAME;
                // smoke.smoke_frame_index = DETECTIVE_SMOKE_STAND_FRAME_START;
            }
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

        if (smoke.body_frame_delay > 0)
            smoke.body_frame_delay--;

        if (smoke.smoke_frame_delay > 0)
            smoke.smoke_frame_delay--;

        if (smoke.smoke_start_delay > 0)
            smoke.smoke_start_delay--;

        wait_vbl_done();
    }
}
