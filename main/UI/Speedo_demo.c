#include "Speedo_demo.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// Global components
lv_obj_t *scr; // Screen background
lv_obj_t *spd_arc; // Speedometer arc
lv_obj_t *spd_label; // Speedometer label

// Color palette
lv_color_t palette_black = LV_COLOR_MAKE(0, 0, 0);
lv_color_t palette_white = LV_COLOR_MAKE(255, 255, 255);
lv_color_t palette_red = LV_COLOR_MAKE(255, 0, 0);
lv_color_t palette_amber= LV_COLOR_MAKE(250, 140, 0);
lv_color_t palette_yellow = LV_COLOR_MAKE(255, 210, 0);

// Fonts
LV_FONT_DECLARE(futura_light_24);

// Attributes
lv_obj_t *canvas;
static lv_color_t canvas_buf[480 * 480];

const int dimension = 480;
const int spd_max = 300;
const int spd_min = 0;
const int km_total = 0;
const int km_trip = 0;
const int kmh = 0;
const int arc_start = 210;
const int arc_end = -30;
const int spd_line_width = 4;



typedef enum {
    P,
    R,
    N,
    D,
    G1,
    G2,
    G3,
    G4
} Gear;

Gear gear = P;

// Draw the tick marks around the speedometer

void draw_ticks(void) {
    int cx = dimension / 2; 
    int cy = dimension / 2;
    int r_outer = 235; 

    canvas = lv_canvas_create(scr);
    lv_canvas_set_buffer(canvas, canvas_buf, dimension, dimension, LV_IMG_CF_TRUE_COLOR);
    lv_obj_center(canvas);
    lv_canvas_fill_bg(canvas, palette_white, LV_OPA_COVER);

    int start_angle = 225; 
    int end_angle = -45;

    for(int v = 20; v <= 300; v += 10) {
        float angle = (float)start_angle + (float)(v - 20) * (float)(end_angle - start_angle) / (300.0f - 20.0f);
        float rad = angle * (3.14159265f / 180.0f);

        int is_major = (v % 20 == 0);
        int r_inner = is_major ? 200 : 218; 

        int x1 = cx + (int)(r_outer * cosf(rad));
        int y1 = cy - (int)(r_outer * sinf(rad));
        int x2 = cx + (int)(r_inner * cosf(rad));
        int y2 = cy - (int)(r_inner * sinf(rad));

        lv_draw_line_dsc_t dsc;
        lv_draw_line_dsc_init(&dsc);
        dsc.color = palette_black;
        dsc.width = is_major ? 4 : 2;
        dsc.round_start = 1; 
        dsc.round_end = 1;

        lv_point_t pts[2] = {{x1, y1}, {x2, y2}};
        lv_canvas_draw_line(canvas, pts, 2, &dsc);

        // --- DRAW LABELS ---
        if(is_major) {
            char buf[12];
            sprintf(buf, "%d", v);

            int r_text = 168; 
            int tx = cx + (int)(r_text * cosf(rad));
            int ty = cy - (int)(r_text * sinf(rad));

            lv_draw_label_dsc_t label_dsc;
            lv_draw_label_dsc_init(&label_dsc);
            label_dsc.color = palette_black;
            
            // Swapped to your new custom font
            //label_dsc.font = &futura_light_24; 
            label_dsc.align = LV_TEXT_ALIGN_CENTER;

            // Centering the box (30px offset for a 60px wide box)
            lv_canvas_draw_text(canvas, tx - 30, ty - 12, 60, &label_dsc, buf);
        }
    }
}

// Draw yellow limit block from 50 to 60 km/h
void draw_speed_limit_marker(lv_obj_t * canvas, int cx, int cy) {
    // 1. Map 50 and 60 km/h to angles based on your 20-300 scale
    float angle_50 = 225.0f + (50.0f - 20.0f) * (-45.0f - 225.0f) / (300.0f - 20.0f);
    float angle_60 = 225.0f + (60.0f - 20.0f) * (-45.0f - 225.0f) / (300.0f - 20.0f);

    // 2. Setup Draw Descriptor
    lv_draw_arc_dsc_t yellow_dsc;
    lv_draw_arc_dsc_init(&yellow_dsc);
    
    // Use your global palette color
    yellow_dsc.color = palette_yellow; 
    yellow_dsc.width = 16; 

    // 3. Draw the Solid Yellow Block
    // Buffer (1.2f) prevents overlapping the black tick marks for a factory look
    float buffer = 1.2f;
    int start_angle = (int)(360 - angle_50 + buffer);
    int end_angle = (int)(360 - angle_60 - buffer);
    
    // Radius 234 keeps the marker pushed towards the outer edge
    lv_canvas_draw_arc(canvas, cx, cy, 234, start_angle, end_angle, &yellow_dsc);
}

// Build the UI
void Build_UI(void){
    scr = lv_scr_act();    
    lv_obj_set_style_bg_color(scr, palette_white, 0);

    draw_ticks(); // call speedometer tick drawing function
    draw_speed_limit_marker(canvas, dimension / 2, dimension / 2); // call speed limit marker drawing function

}

void lvgl_live_preview_init(void)
{
    Build_UI();
}