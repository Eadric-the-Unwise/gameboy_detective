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
#include "submap.h"
#include "maps/bkg_apartment_map.h"
#include "maps/bkg_apartment_tiles.h"
#include "maps/bkg_apartment_scroll_map.h"
#include "maps/bkg_apartment_scroll_tiles.h"
#include "maps/bkg_submap_map.h"
#include "maps/bkg_submap_tiles.h"
#include "tiles/bkg_apartment_lamp_top.h"
#include "tiles/bkg_apartment_lamp_bot.h"
#include "tiles/apartment_smoke.h"

joypads_t joypads;
Character detective;
CharacterSmoke smoke;
CharacterSmoke smoke_objects[SMOKE_OBJECT_COUNT];
UINT8 hiwater;
UINT8 apartment_smoke_tile_index;
UBYTE apartment_smoke_flip = 0;
UBYTE apartment_smoke_delay = 0;
UBYTE apartment_lamp_delay = 0;

UBYTE updated;
UBYTE indoor;
UBYTE apartment = 0;
UBYTE running = 1;
joypads_t joypads;

UBYTE updated;

//loads the submaps
void init_house(Character *detective)
{
    HIDE_BKG;
    set_bkg_data(0, BKG_APARTMENT_SCROLL_TILE_COUNT, bkg_apartment_scroll_tiles);
    set_bkg_submap(0, 0, BKG_APARTMENT_SCROLL_MAP_WIDTH, BKG_APARTMENT_SCROLL_MAP_HEIGHT, bkg_apartment_scroll_map, BKG_APARTMENT_SCROLL_MAP_WIDTH);
    move_bkg(0, 0);
    SHOW_BKG;
    indoor = 1;
    apartment = 0;
    detective->x = 88;
    detective->y = 80;
    detective->direction = FACE_DOWN;
    detective->body_frame_index = DETECTIVE_BODY_DOWN_FRAME_START;
}

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
UINT8 load_smoke_data(UINT8 hiwater)
{
    set_sprite_data(hiwater, (sizeof(smoke_data) >> 4), smoke_data);
    hiwater += (sizeof(smoke_data) >> 4);

    return hiwater;
}

UINT8 load_apartment_smoke_data(UINT8 hiwater)
{
    set_sprite_data(hiwater, apartment_smokeLen, apartment_smoke);
    hiwater += apartment_smokeLen;

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
        // NOT FACING RIGHT
        hiwater += move_metasprite(cig_shine_metasprites[detective->body_frame_index], detective->cig_shine_tile_index, hiwater, x, y);
        hiwater += move_metasprite(tile_detectivewalk_metasprites[detective->body_frame_index], detective->body_tile_index, hiwater, x, y);
    }
    else
    {
        // FACE_RIGHT (Flip the sprites)
        UINT8 cig_sprite = hiwater + 1;
        hiwater += move_metasprite_vflip(cig_shine_metasprites[detective->body_frame_index], detective->cig_shine_tile_index, hiwater, x, y);
        hiwater += move_metasprite_vflip(tile_detectivewalk_metasprites[detective->body_frame_index], detective->body_tile_index, hiwater, x, y);
        hide_sprite(cig_sprite);
    }
    return hiwater;
}

UINT8 update_smoke(CharacterSmoke *smoke, UINT8 smoke_tile_index, UINT8 x, UINT8 y, UINT8 hiwater)
{
    if (smoke->direction != FACE_RIGHT)
    {
        hiwater += move_metasprite(smoke_metasprites[smoke->smoke_frame_index], smoke_tile_index, hiwater, x + TILE_SIZE, y - TILE_SIZE);
    }
    //else if = EITHER THIS OR THIS (no others allowed)
    //else = Anything other than the if is okay
    else
    {
        hiwater += move_metasprite_vflip(smoke_metasprites[smoke->smoke_frame_index], smoke_tile_index, hiwater, x - TILE_SIZE, y - TILE_SIZE);
    }
    return hiwater;
}

void setup_detective(Character *detective)
{

    // Initial position, loads from TOP RIGHT of the Metasprite
    detective->x = 95;
    detective->y = 80;

    detective->direction = FACE_LEFT; // Set to LEFT

    // detective body
    detective->body_animate = 0; // Set to OFF
    detective->body_frame_index = DETECTIVE_BODY_WALK_FRAME_START;
    detective->body_frame_delay = 0;
}

void setup_smoke(CharacterSmoke *smoke)
{
    smoke->state = STOPPED;
}

void animate_detective(Character *detective)
{
    if (detective->body_animate == 1 && detective->body_frame_delay == 0)
    {
        // Animate the body when detective is moving.
        updated = 1;
        detective->body_frame_delay = FRAME_DELAY;
        detective->body_frame_index++;

        if (detective->direction == FACE_LEFT || detective->direction == FACE_RIGHT)
        {

            if (detective->body_frame_index > DETECTIVE_BODY_WALK_FRAME_END)
            {
                detective->body_frame_index = DETECTIVE_BODY_WALK_FRAME_START;
            }
        }
        else if (detective->direction == FACE_UP)
        {

            if (detective->body_frame_index > DETECTIVE_BODY_UP_FRAME_END)
            {
                detective->body_frame_index = DETECTIVE_BODY_UP_FRAME_START;
            }
        }
        else if (detective->direction == FACE_DOWN)
        {

            if (detective->body_frame_index > DETECTIVE_BODY_DOWN_FRAME_END)
            {
                detective->body_frame_index = DETECTIVE_BODY_DOWN_FRAME_START;
            }
        }

        detective->body_frame_delay = detective->body_frame_index % 2 ? FRAME_DELAY * 2 : FRAME_DELAY;
    }

    // The amount of delay between frame animation. Decrement animation delays
    if (detective->body_frame_delay > 0)
        detective->body_frame_delay--;
}

void animate_smoke(Character *detective, CharacterSmoke *smoke)
{
    if (smoke->state == RESTART)
    {
        // Start smoke
        smoke->state = PLAYING;
        smoke_walk_FRAME_START = smoke->facing_LR ? DETECTIVE_SMOKE_WALK_LR_FRAME_START : DETECTIVE_SMOKE_WALK_UPDOWN_FRAME_START;
        smoke_walk_FRAME_END = smoke->facing_LR ? DETECTIVE_SMOKE_WALK_LR_FRAME_END : DETECTIVE_SMOKE_WALK_UPDOWN_FRAME_END;

        smoke->x = detective->x;
        smoke->y = detective->y;
        if (indoor == 1)
        {
            smoke->y = detective->y;
        }
        else
        {
            smoke->y = detective->y;
        }

        smoke->body_animate = detective->body_animate;
        smoke->smoke_frame_delay = 0;

        smoke->direction = detective->direction;
        if (smoke->direction == FACE_UP)
        {
            smoke->direction = FACE_RIGHT;
        }
        else if (smoke->direction == FACE_DOWN && !smoke->body_animate)
        {
            smoke->direction = FACE_LEFT;
        }
        if (detective->body_animate)
        {
            // detective->body_animate
            if (detective->direction == FACE_UP || detective->direction == FACE_DOWN)
            {
                // Up / Down
                smoke->smoke_frame_start = DETECTIVE_SMOKE_WALK_UPDOWN_FRAME_START;
                smoke->smoke_frame_end = DETECTIVE_SMOKE_WALK_UPDOWN_FRAME_END;
            }
            else
            {
                // Left / Right
                smoke->smoke_frame_start = DETECTIVE_SMOKE_WALK_LR_FRAME_START;
                smoke->smoke_frame_end = DETECTIVE_SMOKE_WALK_LR_FRAME_END;
            }
        }
        else
        {
            // NOT detective->body_animate
            smoke->smoke_frame_start = DETECTIVE_SMOKE_STAND_FRAME_START;
            smoke->smoke_frame_end = DETECTIVE_SMOKE_STAND_FRAME_END;
        }
        smoke->smoke_frame_index = smoke->smoke_frame_start;
    }

    if (smoke->state == PLAYING && smoke->smoke_frame_delay == 0)
    {
        updated = 1;
        smoke->smoke_frame_delay = smoke->body_animate ? SMOKE_DELAY : SMOKE_IDLE_DELAY;
        smoke->smoke_frame_index++;

        if (smoke->smoke_frame_index > smoke->smoke_frame_end)
        {
            smoke->state = STOPPED;
        }
    }

    if (smoke->smoke_frame_delay > 0)
        smoke->smoke_frame_delay--;
}

void main(void)
{
    SHOW_BKG;
    SHOW_SPRITES;
    SPRITES_8x16;

    OBP1_REG = 0xE1;
    BGP_REG = 0xE4;

    joypad_init(1, &joypads);

    updated = 1;
    indoor = 0;

    /******************************/
    // Declare local variables
    /******************************/

    Character detective;
    CharacterSmoke smoke_objects[SMOKE_OBJECT_COUNT];

    UINT8 tile_hiwater;
    UINT8 sprite_hiwater; //OAM index

    UINT8 smoke_tile_index;
    UINT8 smoke_start_delay = 0;

    /******************************/
    // Load tiles
    /******************************/

    tile_hiwater = 0;

    tile_hiwater = load_detective_data(&detective, tile_hiwater); //copies value 0 then updates with return

    smoke_tile_index = tile_hiwater;
    tile_hiwater = load_smoke_data(tile_hiwater); //copies Detective tile count then updates with return
    apartment_smoke_tile_index = tile_hiwater;
    tile_hiwater = load_apartment_smoke_data(tile_hiwater);
    /******************************/
    // Setup structs
    /******************************/

    setup_detective(&detective);
    for (UINT8 i = 0; i < SMOKE_OBJECT_COUNT; i++)
    {
        setup_smoke(&smoke_objects[i]);
    }

    DISPLAY_ON;
    //set_bkg_data is actually what loads the tile images into VRAM memory
    //set_bkg_tiles is what loads the map in the Map Screen Buffer Screen

    // set_bkg_data(0, BKG_APARTMENT_TILE_COUNT, bkg_apartment_tiles);
    // set_bkg_tiles(0, 0, BKG_APARTMENT_MAP_WIDTH, BKG_APARTMENT_MAP_HEIGHT, bkg_apartment_map);

    /******************************/
    // Setup submap variables and initialize scene & detective XY
    /******************************/
    init_house(&detective);
    indoor = 1;
    map_pos_x = map_pos_y = 0;
    old_map_pos_x = old_map_pos_y = 255;

    camera_x = camera_y = 0;
    old_camera_x = camera_x;
    old_camera_y = camera_y;

    redraw = FALSE;

    SCX_REG = camera_x;
    SCY_REG = camera_y;
    while (running)
    {
        /******************************/
        // Joypad
        /******************************/

        joypad_ex(&joypads);

        if (joypads.joy0 & J_A)
        {
        }
        if (joypads.joy0 & J_B)
        {
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
                if (indoor == 1)
                {
                    updated = 1;
                }
                else
                {
                    updated = 1;
                    detective.x -= 1;
                    if (camera_y)
                    {
                        camera_x--;
                        redraw = TRUE;
                    }
                }
                if (detective.body_animate == 0)
                {
                    // started moving for the first time
                    detective.body_animate = 1;
                    smoke_start_delay = SMOKE_WALK_START_DELAY;
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
                if (indoor == 1)
                {
                    updated = 1;
                }
                else
                {
                    updated = 1;
                    detective.x += 1;

                    if (camera_x < camera_max_x)
                    {
                        camera_x++;
                        redraw = TRUE;
                    }
                }

                if (detective.body_animate == 0)
                {
                    // started moving for the first time
                    detective.body_animate = 1;
                    smoke_start_delay = SMOKE_WALK_START_DELAY;
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
                if (indoor == 1)
                {
                    updated = 1;
                    scroll_bkg(0, -1);
                }

                else
                {
                    updated = 1;
                    detective.y -= 1;
                    if (camera_y)
                    {
                        camera_y--;
                        redraw = TRUE;
                    }
                }
                if (detective.body_animate == 0)
                {
                    // started moving for the first time
                    detective.body_animate = 1;
                    smoke_start_delay = SMOKE_WALK_START_DELAY;
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
                if (indoor == 1)
                {
                    updated = 1;
                    scroll_bkg(0, 1);
                }

                else
                {
                    detective.y += 1;
                    updated = 1;
                    if (camera_y < camera_max_y)
                    {
                        camera_y++;
                        redraw = TRUE;
                    }
                }
                if (detective.body_animate == 0)
                {
                    // started moving for the first time
                    detective.body_animate = 1;
                    smoke_start_delay = SMOKE_WALK_START_DELAY;
                }
            }
        }
        if (joypads.joy0 & J_START)
        {
            init_house(&detective);
        }
        if (joypads.joy0 & J_SELECT)
        {
        }

        //~ means "not"
        if (!(joypads.joy0 & (J_LEFT | J_RIGHT | J_UP | J_DOWN)))
        {
            // Not moving
            if (detective.body_animate == 1)
            {
                // If body is animated

                updated = 1;

                // Stop body animation
                detective.body_animate = 0;

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

                smoke_start_delay = SMOKE_IDLE_START_DELAY;
            }
        }
        if (redraw == 1)
        {
            set_camera();

            redraw = FALSE;
        }

        /******************************/
        // Animations
        /******************************/

        animate_detective(&detective);

        for (UINT8 i = 0; i < SMOKE_OBJECT_COUNT; i++)
        {
            if (smoke_start_delay == 0 && smoke_objects[i].state == STOPPED)
            {
                smoke_objects[i].state = RESTART;
                smoke_start_delay = (detective.body_animate ? SMOKE_WALK_START_DELAY : SMOKE_IDLE_START_DELAY);
            }
            animate_smoke(&detective, &smoke_objects[i]);
        }

        if (smoke_start_delay > 0)
            smoke_start_delay--;

        if (apartment_smoke_delay == 0)
        {
            apartment_smoke_flip = !apartment_smoke_flip;
            apartment_smoke_delay = 30;
            updated = 1;
        }

        if (apartment_smoke_delay > 0)
            apartment_smoke_delay--;

        if (apartment_lamp_delay > 0)
            apartment_lamp_delay--;

        if (apartment == 1)
        {
            if (apartment_lamp_delay == 0)
            {
                set_bkg_data(0, BKG_APARTMENT_TILE_COUNT, bkg_apartment_tiles);
                apartment_lamp_delay = 240;
                updated = 1;
            }
            if (apartment_lamp_delay == 15)
            {
                set_bkg_data(0x12, apartment_lamp_topLen, apartment_lamp_top);
                set_bkg_data(0x1E, apartment_lamp_botLen, apartment_lamp_bot);
            }
        }

        /******************************/
        // Drawing
        /******************************/

        if (updated == 1)
        {
            // If there's been any changes, update the metasprites.

            updated = 0;
            sprite_hiwater = 0; //OAM

            //Apartment Smoke
            set_sprite_tile(sprite_hiwater, apartment_smoke_tile_index);
            move_sprite(sprite_hiwater, 53, 79);
            set_sprite_prop(sprite_hiwater, apartment_smoke_flip ? S_FLIPX : 0);

            sprite_hiwater += 1;

            // Smoke trail
            for (UINT8 i = 0; i < SMOKE_OBJECT_COUNT; i++)
            {
                if (smoke_objects[i].state == PLAYING)
                {
                    sprite_hiwater = update_smoke(&smoke_objects[i], smoke_tile_index, smoke_objects[i].x, smoke_objects[i].y, sprite_hiwater);
                }
            }

            // Detective
            sprite_hiwater = update_detective(&detective, detective.x, detective.y, sprite_hiwater);

            // Hide unused sprites
            for (UINT8 i = sprite_hiwater; i < 40; i++)
                shadow_OAM[i]
                    .y = 0;
        }

        wait_vbl_done();
    }
}
