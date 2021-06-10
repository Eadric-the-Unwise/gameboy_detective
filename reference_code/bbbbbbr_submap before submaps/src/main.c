#include <gb/gb.h>

// GFX Resources
#include "../res/bigmap_map.h"
#include "../res/bigmap_tiles.h"

#include "mapscroll.h"


UINT8 keys = 0;
UINT8 previous_keys = 0;

#define UPDATE_KEYS() previous_keys = keys; keys = joypad()
#define KEY_PRESSED(K) (keys & (K))
#define KEY_TICKED(K)   ((keys & (K)) && !(previous_keys & (K)))
#define KEY_RELEASED(K) ((previous_keys & (K)) && !(keys & (K)))
#define ANY_KEY_PRESSED (keys)

#define PLY_DIR_LEFT   0U
#define PLY_DIR_UP     2U
#define PLY_DIR_RIGHT  4U
#define PLY_DIR_DOWN   6U
#define PLY_DIR_RESET  0xFF


void init_gfx() {
    // Load Background tiles and then map
    // set_bkg_data(0, 79u, dungeon_tiles);
    // map_set(dungeon_mapPLN0, dungeon_mapWidth, dungeon_mapHeight);

    set_bkg_data(0, 241u, bigmap_tiles);
    map_set(bigmap_mapPLN0, bigmap_mapWidth, bigmap_mapHeight);  // Pointer to map, width & height in Tiles

	// Set the background map to visible
    SHOW_BKG;
}


#define SPEED  1
// #define SPEED  2
// #define SPEED  4
// #define SPEED  8

INT8 move_x, move_y;

void main(void)
{
	init_gfx();

    // Set current map camera coordinates (in pixel units)
    map_scroll_set(0,0);

    // Loop forever
    while(1) {

        UPDATE_KEYS(); // Read Joypad

        if (keys & J_LEFT) {
            move_x = -SPEED;
        }
        else if (keys & J_RIGHT) {
            move_x = SPEED;
        } 
        else move_x = 0;

        if (keys & J_UP) {
            move_y = -SPEED;
        }
        else if (keys & J_DOWN) {
            move_y = SPEED;
        }
        else move_y = 0;

        // Does not support scrolling by more than 8 pixels at a time
        if ((move_x != 0) || (move_y != 0))
            map_scroll_xy(move_x, move_y);

		// Done processing, yield CPU and wait for start of next frame
        wait_vbl_done();
    }
} 
