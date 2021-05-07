#pragma once

// Sprite delays (60 = 1 second)
#define FRAME_DELAY 7
#define SMOKE_DELAY 9             //walking smoke animation frame delay
#define SMOKE_IDLE_DELAY 11       //standing smoke animation frame delay
#define SMOKE_IDLE_START_DELAY 30 //pause time between smoke puffs
#define SMOKE_WALK_START_DELAY 5

//SPRITE INDEX IS NOT RELATED TO THE TILE INDEX. These are used to load the Sprite OBJ's into OAM in order
//0 in the OAM

//amount of sprites
// #define DETECTIVE_BODY_SPRITE_COUNT 4
#define SMOKE_SMOKE_SPRITE_INDEX 0                      //OAM
#define SMOKE_TILE_COUNT (SMOKE_SMOKE_SPRITE_INDEX + 2) //assures ShadowOAM only effects the smoke tiles
#define DETECTIVE_CIG_SHINE_SPRITE_INDEX 2              //OAM
#define DETECTIVE_BODY_SPRITE_INDEX 4                   //OAM

// DETECTIVE body stand
#define DETECTIVE_BODY_STAND_FRAME 0
// DETECTIVE body walk animations + smoke animations
#define DETECTIVE_BODY_WALK_FRAME_START 0
#define DETECTIVE_BODY_WALK_FRAME_END 3
#define DETECTIVE_SMOKE_STAND_FRAME_START 0
#define DETECTIVE_SMOKE_STAND_FRAME_END 5
#define DETECTIVE_SMOKE_WALK_FRAME_START 6
#define DETECTIVE_SMOKE_WALK_FRAME_END 10

#define TILE_SIZE 8