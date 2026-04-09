#include "Info_demo.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "fonts/futura_medium_24.c"
#include "images/Minipump.c"

// --- Global UI Component Pointers ---
lv_obj_t *scr; 
lv_obj_t *canvas;
static lv_color_t canvas_buf[400 * 400];
const int dimension = 400; 

// Color palette
lv_color_t palette_black = LV_COLOR_MAKE(0, 0, 0);
lv_color_t palette_white = LV_COLOR_MAKE(255, 255, 255);
lv_color_t palette_red   = LV_COLOR_MAKE(220, 10, 10);

const int cx = 200; 
const int cy = 200;

// Declare the image from Minipump.c
extern const lv_img_dsc_t Minipump;

void draw_info_elements(void) {
    canvas = lv_canvas_create(scr);
    lv_canvas_set_buffer(canvas, canvas_buf, dimension, dimension, LV_IMG_CF_TRUE_COLOR);
    lv_obj_center(canvas);
    lv_canvas_fill_bg(canvas, palette_white, LV_OPA_COVER);

    lv_draw_label_dsc_t lbl_dsc;
    lv_draw_label_dsc_init(&lbl_dsc);
    lbl_dsc.font = &futura_medium_24;
    lbl_dsc.align = LV_TEXT_ALIGN_CENTER;

    int r_out = 198; 
    int r_in  = 175;

    // --- 1. FUEL GAUGE (Top - Centered at 90°) ---
    float f_center = 90.0f;
    float f_span = 25.0f; 
    for(int m = 0; m <= 4; m++) {
        float angle = (f_center + f_span) - (m * (f_span * 2.0f) / 4.0f);
        float rad = angle * (M_PI / 180.0f);
        lv_point_t p[2] = {
            {cx + (int)(r_out * cosf(rad)), cy - (int)(r_out * sinf(rad))},
            {cx + (int)(r_in  * cosf(rad)), cy - (int)(r_in  * sinf(rad))}
        };
        lv_draw_line_dsc_t l_dsc; lv_draw_line_dsc_init(&l_dsc);
        l_dsc.color = (m == 0) ? palette_red : palette_black; 
        l_dsc.width = (m % 2 == 0) ? 7 : 4;
        lv_canvas_draw_line(canvas, p, 2, &l_dsc);
        
        if(m % 2 == 0) {
            char *txt = (m == 0) ? "R" : (m == 2) ? "1/2" : "1/1";
            lbl_dsc.color = (m == 0) ? palette_red : palette_black;
            lv_canvas_draw_text(canvas, p[1].x - 25, p[1].y + 5, 50, &lbl_dsc, txt);
        }
    }

    // --- IMPORTED GAS PUMP ICON (SMALLER) ---
    lv_draw_img_dsc_t img_dsc;
    lv_draw_img_dsc_init(&img_dsc);
    img_dsc.zoom = 110; 
    lv_canvas_draw_img(canvas, cx - 10, cy - 140, &Minipump, &img_dsc);

    // --- FUEL DOOR TRIANGLE (Pointing Right) ---
    lv_draw_rect_dsc_t tri_dsc;
    lv_draw_rect_dsc_init(&tri_dsc);
    tri_dsc.bg_color = palette_black;
    tri_dsc.bg_opa = LV_OPA_COVER; // Ensure it's fully opaque
    tri_dsc.border_width = 0;

    // Shifted further right (cx + 22) to avoid overlapping the pump icon's boundary
    // and lowered (cy - 130) to align with the middle of the pump
    lv_point_t tri_points[] = {
        {cx + 18, cy - 134}, // Top left
        {cx + 18, cy - 124}, // Bottom left
        {cx + 25, cy - 129}  // Tip pointing right
    };
    lv_canvas_draw_polygon(canvas, tri_points, 3, &tri_dsc);

    // --- 2. TEMPERATURE GAUGE (Left - Centered at 180°) ---
    float t_angles[] = {200.0f, 192.0f, 180.0f, 172.0f, 165.0f};
    char *t_labels[] = {"", "40", "80", "", "120"};

    for(int i = 0; i < 5; i++) {
        float rad = t_angles[i] * (M_PI / 180.0f);
        lv_point_t p[2] = {
            {cx + (int)(r_out * cosf(rad)), cy - (int)(r_out * sinf(rad))},
            {cx + (int)(r_in  * cosf(rad)), cy - (int)(r_in  * sinf(rad))}
        };

        lv_draw_line_dsc_t l_dsc; lv_draw_line_dsc_init(&l_dsc);
        l_dsc.color = palette_black;
        l_dsc.width = (i == 0 || i == 2 || i == 4) ? 7 : 4;
        lv_canvas_draw_line(canvas, p, 2, &l_dsc);

        // 120 Mark: Flush Red line
        if(i == 4) {
            float thickness_offset = 2.2f; 
            float red_rad = (t_angles[i] - thickness_offset) * (M_PI / 180.0f); 
            lv_point_t p_red[2] = {
                {cx + (int)(r_out * cosf(red_rad)), cy - (int)(r_out * sinf(red_rad))},
                {cx + (int)(r_in  * cosf(red_rad)), cy - (int)(r_in  * sinf(red_rad))}
            };
            l_dsc.color = palette_red;
            l_dsc.width = 7; 
            lv_canvas_draw_line(canvas, p_red, 2, &l_dsc);
        }

        if(strlen(t_labels[i]) > 0) {
            lbl_dsc.color = palette_black;
            int tx = p[1].x + 6; 
            int ty = p[1].y - 14; 
            lv_canvas_draw_text(canvas, tx, ty, 50, &lbl_dsc, t_labels[i]);
        }
    }

    // --- 3. OIL PRESSURE (Right - Centered at 0°) ---
    float o_start = 15.0f; float o_end = -15.0f;
    for(int i = 0; i <= 3; i++) {
        float angle = o_start + (float)i * (o_end - o_start) / 3.0f;
        float rad = angle * (M_PI / 180.0f);
        lv_point_t p[2] = {
            {cx + (int)(r_out * cosf(rad)), cy - (int)(r_out * sinf(rad))},
            {cx + (int)(r_in  * cosf(rad)), cy - (int)(r_in  * sinf(rad))}
        };
        lv_draw_line_dsc_t l_dsc; lv_draw_line_dsc_init(&l_dsc);
        l_dsc.color = palette_black; 
        l_dsc.width = 7;
        lv_canvas_draw_line(canvas, p, 2, &l_dsc);

        char buf[4]; sprintf(buf, "%d", 3 - i); 
        lv_canvas_draw_text(canvas, p[1].x - 35, p[1].y - 14, 40, &lbl_dsc, buf);
    }
}

void Build_UI(void){
    scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, palette_white, 0);
    draw_info_elements();
}

void lvgl_live_preview_init(void) {
    Build_UI();
}