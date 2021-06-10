
#include <gb/gb.h>

#include "mapscroll.h"
#include "set_bkg_tiles_col.h"

#define PIXELS_PER_TILE       8U
#define SCREEN_WIDTH          20U
#define SCREEN_HEIGHT         18U
#define DRAW_WIDTH          (SCREEN_WIDTH + 3U)
#define DRAW_HEIGHT         (SCREEN_HEIGHT + 3U)
#define SCREEN_WIDTH_PIXELS   (SCREEN_WIDTH * PIXELS_PER_TILE)
#define SCREEN_HEIGHT_PIXELS  (SCREEN_HEIGHT * PIXELS_PER_TILE)

#define SCREEN_MAP_WIDTH      32U
#define SCREEN_MAP_HEIGHT     32U


// ----------------------- Utility function for maps greater than 32x32

UINT8 * p_col_screen;

// Sets one map_width worth of background tiles, will wrap around from bottom to top if needed
// Doesn't work with the Window Map
void set_bkg_tiles_col_C(UINT8 screen_x, UINT8 screen_y, UINT8 row_height, UINT8 map_width, const UINT8 * p_src_map) {

    // Cache returned address of first map tile vram address
    p_col_screen = (UINT8 *)set_bkg_tile_xy(screen_x, screen_y, *p_src_map);

    while (row_height--) {
        // Move to next Screen Map_y and handle screen wraparound 0x9C00 -> 9800
        p_col_screen = (UINT8 *)((UINT16)(p_col_screen + SCREEN_MAP_WIDTH) & 0x9BFFU);
        p_src_map += map_width; // Move to next Map_y
        while (STAT_REG & 0x02); // Check if in LCD modes 0 or 1
        *p_col_screen = *p_src_map;
    }

}

// -----------------------



const UINT8 * p_map_data; // Stores pointer to start of map data
const UINT8 * p_map_temp;


UINT16 camera_x, camera_y;
UINT16 camera_x_max, camera_y_max;

UINT8  map_x, map_y;
UINT8  map_x_last, map_y_last;
UINT8  map_width_tiles, map_height_tiles;


// Pointer to map, width and height in tiles
void map_set(UINT8 * map, UINT8 width_tiles, UINT8 height_tiles) {

    // Save pointer to map data
    p_map_data = map;

    // Save map size
    map_width_tiles = width_tiles;
    map_height_tiles = height_tiles;

    // Pre-calc useful values
    camera_x_max = (map_width_tiles * PIXELS_PER_TILE) - SCREEN_WIDTH_PIXELS;
    camera_y_max = (map_height_tiles * PIXELS_PER_TILE) - SCREEN_HEIGHT_PIXELS;
}


// X,Y are in **Pixel** coordinates
void map_scroll_set(UINT16 x, UINT16 y) {

    // Load new coordinates
    camera_x = x;
    camera_y = y;

    // Bounds check
    if (camera_x > camera_x_max)
        camera_x = camera_x_max;
    if (camera_y > camera_y_max)
        camera_y = camera_y_max;

    // Update hardware map scrolling
    SCX_REG = (UINT8)camera_x;
    SCY_REG = (UINT8)camera_y;

    // Calculate visible map tile Upper-Left
    map_x = (UINT8)(camera_x >> 3);
    map_y = (UINT8)(camera_y >> 3);

    // Add a one-tile border to top and left if possible
    if (map_x) map_x--;
    if (map_y) map_y--;

    // Reset last calc vars
    map_x_last = map_x;
    map_y_last = map_y;

    // Draw the initial map view on the screen at the new area
    map_redraw_visible();
}


// Redraws visible part of map
void map_redraw_visible(void) {

    const UINT8 * p_map;
    UINT8 y;

    p_map = p_map_data + (map_y * map_width_tiles) + map_x;

    // +1 and +1 for top/bottom and left/right 1 tile borders
    // +1  again since SCX/SCY may be in the middle of a tile, 
    // so screen spans height +1 or width + 1
    for (y = 0; y <= DRAW_HEIGHT; y++) {
        set_bkg_tiles(map_x & 31,
                     (map_y + y) & 31,      // x, y
                      DRAW_WIDTH, 1U, p_map);   // w, h, Offset into map
        p_map += map_width_tiles;
    }
}


void map_scroll_xy(INT8 x_move, INT8 y_move) {

    UINT8 offset;

    // Update map position and do bounds check
    if ((x_move < 0) && ((x_move * -1) > camera_x)) // TODO: more efficient underflow checking
        camera_x = 0;
    else {
        camera_x += x_move;
        if (camera_x > camera_x_max)
            camera_x = camera_x_max;
    }

    // Update map position and do bounds check
    if ((y_move < 0) && ((y_move * -1) > camera_y)) // TODO: more efficient underflow checking
        camera_y = 0;
    else {
        camera_y += y_move;

        if (camera_y > camera_y_max)
            camera_y = camera_y_max;
    }

    // Lower byte of camera_x/y is automatically correct offset for SCX/SCY
    // Update hardware map window location
    SCX_REG = (UINT8)camera_x;
    SCY_REG = (UINT8)camera_y;

    // Calculate Left and Top edge of viewport in tile map tile coordinates
    // Add a one-tile border to left and Top if possible
    map_x = (UINT8)(camera_x >> 3);
    map_y = (UINT8)(camera_y >> 3);
    if (map_x) map_x--;
    if (map_y) map_y--;


    // Horizontal Scrolling
    // Only viewport Left/Right edge when moved to a new tile
    if (map_x_last != map_x) {
        map_x_last = map_x;

        // Calculate offset into tile map
        p_map_temp = p_map_data + (map_y * map_width_tiles) + map_x;

        // If scrolling right, use right edge of map instead
        if (x_move > 0) {
            offset = DRAW_WIDTH - 1;
            p_map_temp += DRAW_WIDTH - 1;
        } else offset = 0;

        // C version
        set_bkg_tiles_col_C((map_x + offset) & 31,
        // ASM version        
        // set_bkg_tiles_col((map_x + offset) & 31,             // HW Map Tile start X, auto-clips to 0-31
                          map_y & 31,             // HW Map Tile start Y, auto-clips to 0-31
                          DRAW_HEIGHT,   // Map_xumn height: +2 here is since SCY in the middle of a tile may cause it to span HEIGHT + 1 tiles
                          map_width_tiles,     // Map Map_y-stride in tiles
                          p_map_temp );             // Offset pointer into map array
    }


    // Vertical Scrolling
    // Only viewport Top/Bottom edge when moved to a new tile
    if (map_y_last != map_y) {
        map_y_last = map_y;

        // Calculate offset into tile map
        p_map_temp = p_map_data + (map_y * map_width_tiles) + map_x;

        // Down scrolling, select bottom edge
        if (y_move > 0) {
            offset = DRAW_HEIGHT - 1;
            p_map_temp += (map_width_tiles * (DRAW_HEIGHT - 1));
        } else offset = 0;

        set_bkg_tiles(map_x & 31,  // HW Map Tile start X (auto-wraparound to 0-31)
                      (map_y + offset) & 31,       // HW Map Tile start Y (auto-wraparound to 0-31)
                      DRAW_WIDTH,    // Tile Width is +1 since window can span 21 tiles if mid tile
                      1U,                   // Tile Height
                      p_map_temp);               // Offsetted pointer into map array
    }
}
