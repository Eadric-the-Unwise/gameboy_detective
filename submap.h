#include <stdio.h>
#include <gb/gb.h>
#include "maps/bkg_submap_map.h"
#include "maps/bkg_submap_tiles.h"
#include "maps/bkg_apartment_scroll_map.h"
#include "maps/bkg_apartment_scroll_tiles.h"
#include "character.h"

//SUBMAP CODE//
UINT8 camera_max_x = ((BKG_APARTMENT_SCROLL_MAP_WIDTH - 20) * 8);
UINT8 camera_max_y = ((BKG_APARTMENT_SCROLL_MAP_HEIGHT - 18) * 8);
#define MIN(A, B) ((A) < (B) ? (A) : (B))

// current and old positions of the camera in pixels
UWORD camera_x,
    camera_y, old_camera_x, old_camera_y;
// current and old position of the map in tiles
UBYTE map_pos_x, map_pos_y, old_map_pos_x, old_map_pos_y;
// redraw flag, indicates that camera position was changed
UBYTE redraw;

void set_camera()
{
    // update hardware scroll position
    SCY_REG = camera_y;
    SCX_REG = camera_x;
    // up or down
    map_pos_y = (UBYTE)(camera_y >> 3u);
    if (map_pos_y != old_map_pos_y)
    {
        if (camera_y < old_camera_y)
        {
            set_bkg_submap(map_pos_x, map_pos_y, MIN(21u, BKG_APARTMENT_SCROLL_MAP_WIDTH - map_pos_x), 1, bkg_apartment_scroll_map, BKG_APARTMENT_SCROLL_MAP_WIDTH);
        }
        else
        {
            if ((BKG_APARTMENT_SCROLL_MAP_HEIGHT - 18u) > map_pos_y)
                set_bkg_submap(map_pos_x, map_pos_y + 18u, MIN(21u, BKG_APARTMENT_SCROLL_MAP_WIDTH - map_pos_x), 1, bkg_apartment_scroll_map, BKG_APARTMENT_SCROLL_MAP_WIDTH);
        }
        old_map_pos_y = map_pos_y;
    }
    // left or right
    map_pos_x = (UBYTE)(camera_x >> 3u);
    if (map_pos_x != old_map_pos_x)
    {
        if (camera_x < old_camera_x)
        {
            set_bkg_submap(map_pos_x, map_pos_y, 1, MIN(19u, BKG_APARTMENT_SCROLL_MAP_HEIGHT - map_pos_y), bkg_apartment_scroll_map, BKG_APARTMENT_SCROLL_MAP_WIDTH);
        }
        else
        {
            if ((BKG_APARTMENT_SCROLL_MAP_WIDTH - 20u) > map_pos_x)
                set_bkg_submap(map_pos_x + 20u, map_pos_y, 1, MIN(19u, BKG_APARTMENT_SCROLL_MAP_HEIGHT - map_pos_y), bkg_apartment_scroll_map, BKG_APARTMENT_SCROLL_MAP_WIDTH);
        }
        old_map_pos_x = map_pos_x;
    }
    // set old camera position to current camera position
    old_camera_x = camera_x, old_camera_y = camera_y;
}

//^^^^^^ SUBMAP CODE ABOVE ^^^^^^//
