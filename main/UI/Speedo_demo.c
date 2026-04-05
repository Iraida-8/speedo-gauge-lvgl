#include "Speedo_demo.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// Global components
lv_obj_t *scr; 
lv_obj_t *spd_arc; 
lv_obj_t *spd_label; 

// Color palette
lv_color_t palette_black = LV_COLOR_MAKE(0, 0, 0);
lv_color_t palette_white = LV_COLOR_MAKE(255, 255, 255);
lv_color_t palette_red = LV_COLOR_MAKE(255, 0, 0);
lv_color_t palette_amber= LV_COLOR_MAKE(250, 140, 0);
lv_color_t palette_yellow = LV_COLOR_MAKE(255, 210, 0);

// Attributes
lv_obj_t *canvas;
static lv_color_t canvas_buf[480 * 480];

const int dimension = 480;
const int spd_max = 300;
const int spd_min = 0;

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

            int r_text = 165; // Slightly tighter radius for the larger font
            int tx = cx + (int)(r_text * cosf(rad));
            int ty = cy - (int)(r_text * sinf(rad));

            lv_draw_label_dsc_t label_dsc;
            lv_draw_label_dsc_init(&label_dsc);
            label_dsc.color = palette_black;
            
            /* Using Built-in Montserrat 24. 
               If your compiler says this is 'undefined', 
               change it to &lv_font_montserrat_20 
            */
            label_dsc.font = &lv_font_montserrat_24; 
            label_dsc.align = LV_TEXT_ALIGN_CENTER;

            // Adjusted height (24) to accommodate the larger font size
            lv_canvas_draw_text(canvas, tx - 30, ty - 12, 60, &label_dsc, buf);
        }
    }
}

void draw_speed_limit_marker(lv_obj_t * canvas, int cx, int cy) {
    float angle_50 = 225.0f + (50.0f - 20.0f) * (-45.0f - 225.0f) / (300.0f - 20.0f);
    float angle_60 = 225.0f + (60.0f - 20.0f) * (-45.0f - 225.0f) / (300.0f - 20.0f);

    lv_draw_arc_dsc_t yellow_dsc;
    lv_draw_arc_dsc_init(&yellow_dsc);
    yellow_dsc.color = palette_yellow; 
    yellow_dsc.width = 16; 

    float buffer = 1.2f;
    int start_angle = (int)(360 - angle_50 + buffer);
    int end_angle = (int)(360 - angle_60 - buffer);
    
    lv_canvas_draw_arc(canvas, cx, cy, 234, start_angle, end_angle, &yellow_dsc);
}

void Build_UI(void){
    scr = lv_scr_act();    
    lv_obj_set_style_bg_color(scr, palette_white, 0);

    draw_ticks(); 
    draw_speed_limit_marker(canvas, dimension / 2, dimension / 2); 
}

void lvgl_live_preview_init(void) {
    Build_UI();
}