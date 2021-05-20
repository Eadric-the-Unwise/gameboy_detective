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
#include "maps/dk_bkg1_map.h"
#include "maps/dk_bkg1_tiles.h"

UBYTE running = 1;
joypads_t joypads;
Character detective;
CharacterSmoke smoke;
UINT8 hiwater;
UBYTE updated;

//returns value of hiwater
UINT8 load_detective_data(Character *detective, UINT8 hiwater)
{
    // Body tiles
    detective->body_tile_index = hiwater;
    // Number of tiles = (sizeof(tile_detectivewalk_data) >> 4)
    set_sprite_data(detective->body_tile_index, (sizeof(tile_detectivewalk_data) >> 4), tile_detectivewalk_data);
    hiwater += (sizeof(tile_detectivewalk_data) >> 4);
    //cig_shine index
    detective->cig_shine_tile_index = hiwater;
    set_sprite_data(detective->cig_shine_tile_index, (sizeof(cig_shine_data) >> 4), cig_shine_data);
    hiwater += (sizeof(cig_shine_data) >> 4);

    return hiwater;
}
//returns value of hiwater
UINT8 load_smoke_data(CharacterSmoke *smoke, UINT8 hiwater)
{

    //smoke index loads in the OAM + the tile number of Detective + cig_shine tiles
    smoke->smoke_tile_index = hiwater;
    set_sprite_data(smoke->smoke_tile_index, (sizeof(smoke_data) >> 4), smoke_data);
    hiwater += (sizeof(smoke_data) >> 4);

    return hiwater;
}
//blocks detective from walking off of the screen
UBYTE can_detective_move(UINT8 x, UINT8 y)
{
    //since y can never be negative, we dont need to add && y >= 0
    //unsigned is always a positive integer
    return x >= 16 && x <= 160 && y >= 26 && y <= 150;
}

UINT8 update_detective(Character *detective, UINT8 x, UINT8 y, UINT8 hiwater)
{

    if (detective->direction != FACE_RIGHT)
    {
        //hiwater is replaced by variable on call (in this case, 0)
        // NOT FACING RIGHT
        //move_metasprite returns it's sprite count
        hiwater += move_metasprite(cig_shine_metasprites[detective->body_frame_index], detective->cig_shine_tile_index, hiwater, x, y);
        hiwater += move_metasprite(tile_detectivewalk_metasprites[detective->body_frame_index], detective->body_tile_index, hiwater, x, y);
    }
    else
    {
        // FACE_RIGHT (Flip the sprites)
        //move_metasprite returns it's sprite count
        UINT8 cig_sprite = hiwater + 1;

        hiwater += move_metasprite_vflip(cig_shine_metasprites[detective->body_frame_index], detective->cig_shine_tile_index, hiwater, x, y);
        hiwater += move_metasprite_vflip(tile_detectivewalk_metasprites[detective->body_frame_index], detective->body_tile_index, hiwater, x, y);
        hide_sprite(cig_sprite);
    }
    return hiwater;
}

UINT8 update_smoke(CharacterSmoke *smoke, UINT8 x, UINT8 y, UINT8 hiwater)
{

    if (smoke->direction != FACE_RIGHT)
    {
        hiwater += move_metasprite(smoke_metasprites[smoke->smoke_frame_index], smoke->smoke_tile_index, hiwater, x + TILE_SIZE, y - TILE_SIZE);
    }
    //else if = EITHER THIS OR THIS (no others allowed)
    //else = Anything other than the if is okay
    else
    {
        hiwater += move_metasprite_vflip(smoke_metasprites[smoke->smoke_frame_index], smoke->smoke_tile_index, hiwater, x - TILE_SIZE, y - TILE_SIZE);
    }
    return hiwater;
}

void setup_detective(Character *detective)
{

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
    SPRITES_8x16;

    OBP1_REG = 0xE1;
    BGP_REG = 0xE4;

    Character detective;
    CharacterSmoke smoke;

    UINT8 tile_hiwater = 0;

    // Load detective sprite data.
    tile_hiwater = load_detective_data(&detective, tile_hiwater); //copies value 0 then updates with return
    tile_hiwater = load_smoke_data(&smoke, tile_hiwater);         //copies Detective tile count then updates with return

    // set_bkg_data(256, 53, 848);

    // Set initial detective values.
    setup_detective(&detective);
    setup_smoke(&smoke);

    joypad_init(1, &joypads);

    //set_bkg_data is actually what loads the tile images into VRAM memory
    //set_bkg_tiles is what loads the map in the Map Layer Screen
    set_bkg_data(0, DK_BKG1_TILE_COUNT, dk_bkg1_tiles);
    set_bkg_tiles(0, 0, DK_BKG1_MAP_WIDTH, DK_BKG1_MAP_HEIGHT, dk_bkg1_map);

    while (running)
    {

        joypad_ex(&joypads);

        if (detective.body_animate == 1 && detective.body_frame_delay == 0)
        {
            // Animate the body when detective is moving.
            updated = 1;
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
                updated = 1;

                smoke.smoke_frame_delay = smoke.body_animate ? SMOKE_DELAY : SMOKE_IDLE_DELAY;

                if (smoke.smoke_frame_index == 0)
                {
                    smoke.x = detective.x;
                    smoke.y = detective.y;
                    smoke.facing_right = detective.direction == FACE_RIGHT; //returns a 0 or 1
                    smoke.body_animate = detective.body_animate;
                    smoke.direction = detective.direction;
                    smoke.smoke_frame_index = smoke.body_animate ? DETECTIVE_SMOKE_WALK_FRAME_START : DETECTIVE_SMOKE_STAND_FRAME_START;
                    if (smoke.direction == FACE_UP)
                    {
                        smoke.direction = FACE_RIGHT;
                    }
                    else if (smoke.direction == FACE_DOWN && !smoke.body_animate)
                    {
                        smoke.direction = FACE_LEFT;
                    }
                }

                // if (smoke.smoke_start_delay == 0)
                // {}
                smoke.smoke_frame_index++;

                //IF smoke.body.animate = 1, then load DETECTIVE_SMOKE_WALK_FRAME_END. Now ====> smoke.smoke_frame_index > DETECTIVE_SMOKE_WALK_FRAME_END
                if (smoke.smoke_frame_index > (smoke.body_animate ? DETECTIVE_SMOKE_WALK_FRAME_END : DETECTIVE_SMOKE_STAND_FRAME_END))
                // Reached the last frame. Reset to FRAME_START.
                {
                    smoke.smoke_frame_index = 0;

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
                updated = 1;
                detective.direction = FACE_LEFT;
                detective.body_frame_index = DETECTIVE_BODY_WALK_FRAME_START;
                detective.body_frame_delay = 0;
            }

            if (can_detective_move(detective.x - 1, detective.y))
            {
                updated = 1;
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
                updated = 1;
                detective.direction = FACE_RIGHT;
                detective.body_frame_index = DETECTIVE_BODY_WALK_FRAME_START;
                detective.body_frame_delay = 0;
            }

            if (can_detective_move(detective.x + 1, detective.y))
            {
                updated = 1;
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
                updated = 1;
                detective.direction = FACE_UP;
                detective.body_frame_index = DETECTIVE_BODY_UP_FRAME_START;
                detective.body_frame_delay = 0;
            }

            if (can_detective_move(detective.x, detective.y - 1))
            {
                updated = 1;
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
                updated = 1;
                detective.direction = FACE_DOWN;
                detective.body_frame_index = DETECTIVE_BODY_DOWN_FRAME_START;
                detective.body_frame_delay = 0;
            }
            if (can_detective_move(detective.x, detective.y + 1))
            {
                updated = 1;
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
                updated = 1;

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
                updated = 1;

                // Stop body animation

                smoke.body_frame_index = DETECTIVE_BODY_STAND_FRAME;
                // smoke.smoke_frame_index = DETECTIVE_SMOKE_STAND_FRAME_START;
            }
        }

        if (updated == 1)
        {
            UINT8 sprite_hiwater = 0;
            // If there's been any changes, update the metasprite.
            updated = 0;
            sprite_hiwater = update_smoke(&smoke, smoke.x, smoke.y, sprite_hiwater);
            sprite_hiwater = update_detective(&detective, detective.x, detective.y, sprite_hiwater);

            for (UBYTE i = sprite_hiwater; i < 40; i++)
                shadow_OAM[i].y = 0;
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
