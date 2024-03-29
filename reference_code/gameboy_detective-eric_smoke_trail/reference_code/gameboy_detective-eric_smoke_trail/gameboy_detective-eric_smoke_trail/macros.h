#pragma once

// Sprite delays (60 = 1 second)
#define FRAME_DELAY 7
#define SMOKE_DELAY 9             //walking smoke animation frame delay
#define SMOKE_IDLE_DELAY 11       //standing smoke animation frame delay
#define SMOKE_IDLE_START_DELAY 90 //pause time between smoke puffs
#define SMOKE_WALK_START_DELAY 20

//SPRITE INDEX IS NOT RELATED TO THE TILE INDEX. These are used to load the Sprite OBJ's into OAM in order
//0 in the OAM

//amount of sprites

#define SMOKE_SMOKE_SPRITE_INDEX 0 //OAM
#define SMOKE_SPRITE_COUNT 3       //assures ShadowOAM only effects the smoke tiles
#define SMOKE_OBJECT_COUNT 4

#define DETECTIVE_CIG_SHINE_SPRITE_INDEX 12 //OAM
#define DETECTIVE_CIG_SHINE_SPRITE_COUNT 2

#define DETECTIVE_BODY_SPRITE_INDEX 14 //OAM
#define DETECTIVE_BODY_SPRITE_COUNT 6

// DETECTIVE body stand
#define DETECTIVE_BODY_STAND_FRAME 0

#define DETECTIVE_BODY_UP_STAND_FRAME 5
#define DETECTIVE_BODY_DOWN_STAND_FRAME 5

// DETECTIVE body walk animations + smoke animations
#define DETECTIVE_BODY_WALK_FRAME_START 0
#define DETECTIVE_BODY_WALK_FRAME_END 3

#define DETECTIVE_BODY_UP_FRAME_START 4
#define DETECTIVE_BODY_UP_FRAME_END 7

#define DETECTIVE_BODY_DOWN_FRAME_START 4
#define DETECTIVE_BODY_DOWN_FRAME_END 7

#define DETECTIVE_SMOKE_STAND_FRAME_START 0
#define DETECTIVE_SMOKE_STAND_FRAME_END 5

#define DETECTIVE_SMOKE_WALK_FRAME_START 6
#define DETECTIVE_SMOKE_WALK_FRAME_END 10

#define TILE_SIZE 8

#define FACE_UP 0
#define FACE_LEFT 1
#define FACE_RIGHT 2
#define FACE_DOWN 3