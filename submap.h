#include <stdio.h>
#include <gb/gb.h>
#include "maps/bkg_submap_map.h"
#include "maps/bkg_submap_tiles.h"
#include "maps/bkg_apartment_scroll_map.h"
#include "maps/bkg_apartment_scroll_tiles.h"
#include "character.h"

//SUBMAP CODE//
UINT8 camera_max_x;
UINT8 camera_max_y;
#define MIN(A, B) ((A) < (B) ? (A) : (B))

// current and old positions of the camera in pixels
UWORD camera_x,
    camera_y, old_camera_x, old_camera_y;
// current and old position of the map in tiles
UBYTE map_pos_x, map_pos_y, old_map_pos_x, old_map_pos_y;
// redraw flag, indicates that camera position was changed
UBYTE redraw;

//below PROVIDED BY TOXA IN DISCORD //
UINT8 level_map_width;
UINT8 level_map_height;
UINT8 *level_map_data;
inline void set_level(UINT8 map_width, UINT8 map_height, UINT8 *map_data)
{
    level_map_width = map_width;
    level_map_height = map_height;
    level_map_data = map_data;
    camera_max_x = ((map_width - 20) * 8);
    camera_max_y = ((map_height - 18) * 8);
}
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
            set_bkg_submap(map_pos_x, map_pos_y, MIN(21u, level_map_width - map_pos_x), 1, *level_map_data, level_map_width);
        }
        else
        {
            if ((level_map_height - 18u) > map_pos_y)
                set_bkg_submap(map_pos_x, map_pos_y + 18u, MIN(21u, level_map_width - map_pos_x), 1, *level_map_data, level_map_width);
        }
        old_map_pos_y = map_pos_y;
    }
    // left or right
    map_pos_x = (UBYTE)(camera_x >> 3u);
    if (map_pos_x != old_map_pos_x)
    {
        if (camera_x < old_camera_x)
        {
            set_bkg_submap(map_pos_x, map_pos_y, 1, MIN(19u, level_map_height - map_pos_y), *level_map_data, level_map_width);
        }
        else
        {
            if ((level_map_width - 20u) > map_pos_x)
                set_bkg_submap(map_pos_x + 20u, map_pos_y, 1, MIN(19u, level_map_height - map_pos_y), *level_map_data, level_map_width);
        }
        old_map_pos_x = map_pos_x;
    }
    // set old camera position to current camera position
    old_camera_x = camera_x, old_camera_y = camera_y;
}

//^^^^^^ SUBMAP CODE ABOVE ^^^^^^//
