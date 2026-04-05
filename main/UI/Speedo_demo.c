#include "Speedo_demo.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "fonts/futura_medium_24.c"
#include "images/AMGPMLOGOSML.c"

// --- Global UI Component Pointers ---
lv_obj_t *scr; 
lv_obj_t *canvas;
lv_obj_t *needle;
lv_obj_t *hub;

// Pointers for external updates
lv_obj_t *odo_main_label;
lv_obj_t *odo_trip_label;
lv_obj_t *odo_tenth_label;
lv_obj_t *digi_speed_label;
lv_obj_t *gear_val_label;

static lv_color_t canvas_buf[480 * 480];
const int dimension = 480;

// Declarations
LV_IMG_DECLARE(AMGPMLOGOSML);

// Color palette
lv_color_t palette_black = LV_COLOR_MAKE(0, 0, 0);
lv_color_t palette_white = LV_COLOR_MAKE(255, 255, 255);
lv_color_t palette_yellow = LV_COLOR_MAKE(255, 210, 0);
lv_color_t palette_amber = LV_COLOR_MAKE(255, 110, 0);
lv_color_t palette_grey = LV_COLOR_MAKE(60, 60, 60);

// --- EXTERNAL CONTROL FUNCTIONS ---

void update_gear(const char* gear) {
    if(gear_val_label) {
        lv_label_set_text(gear_val_label, gear);
    }
}

void update_speed(int speed) {
    // 1. Update Digital Readout
    char buf[8];
    sprintf(buf, "%d", speed);
    lv_label_set_text(digi_speed_label, buf);

    // 2. Update Needle Position
    float start_angle = 225.0f;
    float end_angle = -45.0f;
    
    int needle_speed = speed;
    if(needle_speed < 20) needle_speed = 20;
    if(needle_speed > 300) needle_speed = 300;

    float angle = start_angle + (float)(needle_speed - 20) * (end_angle - start_angle) / (300.0f - 20.0f);
    float rad = angle * (3.14159265f / 180.0f);

    int r_needle = 230; 
    static lv_point_t needle_pts[2];
    needle_pts[0].x = (dimension / 2) + (int)(5 * cosf(rad));
    needle_pts[0].y = (dimension / 2) - (int)(5 * sinf(rad));
    needle_pts[1].x = (dimension / 2) + (int)(r_needle * cosf(rad));
    needle_pts[1].y = (dimension / 2) - (int)(r_needle * sinf(rad));

    lv_line_set_points(needle, needle_pts, 2);
}

void update_odometer(uint32_t total_km, float trip_km) {
    char buf_main[25];
    uint32_t t = total_km % 1000000;
    sprintf(buf_main, "%u %u %u %u %u %u", 
            (t / 100000) % 10, (t / 10000) % 10, (t / 1000) % 10, 
            (t / 100) % 10, (t / 10) % 10, t % 10);
    lv_label_set_text(odo_main_label, buf_main);

    int trip_whole = (int)trip_km % 1000;
    int trip_tenth = (int)((trip_km - (int)trip_km) * 10) % 10;
    
    char buf_trip[15];
    sprintf(buf_trip, "%u %u %u", (trip_whole / 100) % 10, (trip_whole / 10) % 10, trip_whole % 10);
    lv_label_set_text(odo_trip_label, buf_trip);
    
    char buf_tenth[4];
    sprintf(buf_tenth, "%d", trip_tenth);
    lv_label_set_text(odo_tenth_label, buf_tenth);
}

// --- ANIMATION CALLBACKS ---

static void fade_anim_cb(void * var, int32_t v) {
    lv_obj_set_style_bg_opa((lv_obj_t *)var, v, 0);
}

static void fade_out_cleanup(lv_anim_t * a) {
    lv_obj_del((lv_obj_t *)a->var); 
}

static void boot_needle_sweep_cb(void * var, int32_t speed) {
    float start_angle = 225.0f;
    float end_angle = -45.0f;
    
    int needle_speed = speed;
    if(needle_speed < 20) needle_speed = 20;
    if(needle_speed > 300) needle_speed = 300;

    float angle = start_angle + (float)(needle_speed - 20) * (end_angle - start_angle) / (300.0f - 20.0f);
    float rad = angle * (3.14159265f / 180.0f);

    int r_needle = 230; 
    static lv_point_t needle_pts[2];
    needle_pts[0].x = (dimension / 2) + (int)(5 * cosf(rad));
    needle_pts[0].y = (dimension / 2) - (int)(5 * sinf(rad));
    needle_pts[1].x = (dimension / 2) + (int)(r_needle * cosf(rad));
    needle_pts[1].y = (dimension / 2) - (int)(r_needle * sinf(rad));

    lv_line_set_points(needle, needle_pts, 2);
}

// --- DRAWING FUNCTIONS ---

void draw_background(void) {
    int cx = dimension / 2; int cy = dimension / 2; int r_outer = 235; 
    canvas = lv_canvas_create(scr);
    lv_canvas_set_buffer(canvas, canvas_buf, dimension, dimension, LV_IMG_CF_TRUE_COLOR);
    lv_obj_center(canvas);
    lv_canvas_fill_bg(canvas, palette_white, LV_OPA_COVER);

    int start_angle = 225; int end_angle = -45;
    for(int v = 20; v <= 300; v += 10) {
        float angle = (float)start_angle + (float)(v - 20) * (float)(end_angle - start_angle) / (300.0f - 20.0f);
        float rad = angle * (3.14159265f / 180.0f);
        int is_major = (v % 20 == 0);
        int r_inner = is_major ? 200 : 218; 
        int x1 = cx + (int)(r_outer * cosf(rad)); int y1 = cy - (int)(r_outer * sinf(rad));
        int x2 = cx + (int)(r_inner * cosf(rad)); int y2 = cy - (int)(r_inner * sinf(rad));

        lv_draw_line_dsc_t dsc; lv_draw_line_dsc_init(&dsc);
        dsc.color = palette_black; dsc.width = is_major ? 4 : 2;
        lv_point_t pts[2] = {{x1, y1}, {x2, y2}};
        lv_canvas_draw_line(canvas, pts, 2, &dsc);

        if(is_major) {
            char buf[12]; sprintf(buf, "%d", v);
            int r_text = 180; int tx = cx + (int)(r_text * cosf(rad)); int ty = cy - (int)(r_text * sinf(rad));
            lv_draw_label_dsc_t label_dsc; lv_draw_label_dsc_init(&label_dsc);
            label_dsc.color = palette_black; label_dsc.font = &futura_medium_24; 
            label_dsc.align = LV_TEXT_ALIGN_CENTER;
            lv_canvas_draw_text(canvas, tx - 30, ty - 12, 60, &label_dsc, buf);
        }
    }
    // Speed Warning Arc
    float angle_50 = 225.0f + (50.0f - 20.0f) * (-45.0f - 225.0f) / (300.0f - 20.0f);
    float angle_60 = 225.0f + (60.0f - 20.0f) * (-45.0f - 225.0f) / (300.0f - 20.0f);
    lv_draw_arc_dsc_t yellow_dsc; lv_draw_arc_dsc_init(&yellow_dsc);
    yellow_dsc.color = palette_yellow; yellow_dsc.width = 16; 
    lv_canvas_draw_arc(canvas, cx, cy, 234, (int)(360 - angle_50 + 2), (int)(360 - angle_60 - 1), &yellow_dsc);
}

void draw_odometers(void) {
    lv_obj_t * km_top = lv_label_create(scr);
    lv_label_set_text(km_top, "km");
    lv_obj_set_style_text_font(km_top, &futura_medium_24, 0); 
    lv_obj_align(km_top, LV_ALIGN_CENTER, 0, -80);

    odo_main_label = lv_label_create(scr);
    lv_obj_set_style_text_font(odo_main_label, &futura_medium_24, 0);
    lv_obj_set_style_bg_color(odo_main_label, palette_black, 0);
    lv_obj_set_style_text_color(odo_main_label, palette_white, 0);
    lv_obj_set_style_bg_opa(odo_main_label, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_ver(odo_main_label, 1, 0); 
    lv_obj_set_style_pad_hor(odo_main_label, 8, 0); 
    lv_obj_set_style_text_letter_space(odo_main_label, 8, 0);
    lv_obj_set_style_border_width(odo_main_label, 4, 0);
    lv_obj_set_style_border_color(odo_main_label, palette_grey, 0);
    lv_obj_align(odo_main_label, LV_ALIGN_CENTER, 0, -50);

    odo_trip_label = lv_label_create(scr);
    lv_obj_set_style_text_font(odo_trip_label, &futura_medium_24, 0);
    lv_obj_set_style_bg_color(odo_trip_label, palette_black, 0);
    lv_obj_set_style_text_color(odo_trip_label, palette_white, 0);
    lv_obj_set_style_bg_opa(odo_trip_label, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_ver(odo_trip_label, 1, 0);
    lv_obj_set_style_pad_hor(odo_trip_label, 5, 0);        
    lv_obj_set_style_text_letter_space(odo_trip_label, 5, 0);
    lv_obj_set_style_border_width(odo_trip_label, 5, 0);
    lv_obj_set_style_border_color(odo_trip_label, palette_grey, 0);
    lv_obj_align(odo_trip_label, LV_ALIGN_CENTER, -20, 65); 

    odo_tenth_label = lv_label_create(scr);
    lv_obj_set_style_text_font(odo_tenth_label, &futura_medium_24, 0);
    lv_obj_set_style_bg_color(odo_tenth_label, palette_white, 0);
    lv_obj_set_style_text_color(odo_tenth_label, palette_black, 0);
    lv_obj_set_style_bg_opa(odo_tenth_label, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(odo_tenth_label, 2, 0);
    lv_obj_set_style_border_color(odo_tenth_label, palette_black, 0);
    lv_obj_set_style_pad_hor(odo_tenth_label, 8, 0); 
    lv_obj_set_style_pad_ver(odo_tenth_label, 1, 0);
    lv_obj_align_to(odo_tenth_label, odo_trip_label, LV_ALIGN_OUT_RIGHT_MID, 15, 0);
}

void draw_gear_selector(void) {
    lv_obj_t * gear_margin = lv_obj_create(scr);
    lv_obj_set_size(gear_margin, 46, 46);
    lv_obj_set_style_bg_color(gear_margin, palette_black, 0);
    lv_obj_set_style_radius(gear_margin, 4, 0);
    lv_obj_set_style_border_width(gear_margin, 0, 0);
    lv_obj_clear_flag(gear_margin, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(gear_margin, LV_ALIGN_CENTER, 100, 10);

    lv_obj_t * gear_box = lv_obj_create(gear_margin);
    lv_obj_set_size(gear_box, 38, 38);
    lv_obj_set_style_bg_color(gear_box, palette_white, 0);
    lv_obj_set_style_radius(gear_box, 2, 0);
    lv_obj_set_style_border_width(gear_box, 0, 0);
    lv_obj_set_style_pad_all(gear_box, 0, 0); 
    lv_obj_clear_flag(gear_box, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(gear_box);

    gear_val_label = lv_label_create(gear_box);
    lv_obj_set_style_text_color(gear_val_label, palette_black, 0);
    lv_obj_set_style_text_font(gear_val_label, &futura_medium_24, 0);
    lv_obj_set_style_border_width(gear_val_label, 1, 0); 
    lv_obj_set_style_border_color(gear_val_label, palette_black, 0);
    lv_obj_set_style_radius(gear_val_label, 1, 0); 
    lv_obj_set_style_pad_hor(gear_val_label, 5, 0); 
    lv_obj_set_style_pad_ver(gear_val_label, 1, 0);
    lv_obj_center(gear_val_label);
}

void draw_branding(void) {
    lv_obj_t * img_logo = lv_img_create(scr);
    lv_img_set_src(img_logo, &AMGPMLOGOSML);
    lv_obj_align(img_logo, LV_ALIGN_CENTER, 0, 120); 

    lv_obj_t * unit_label = lv_label_create(scr);
    lv_label_set_text(unit_label, "km/h");
    lv_obj_set_style_text_font(unit_label, &futura_medium_24, 0); 
    lv_obj_align_to(unit_label, img_logo, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);

    lv_obj_t * digi_box = lv_obj_create(scr);
    lv_obj_set_size(digi_box, 70, 40);
    lv_obj_set_style_bg_color(digi_box, palette_black, 0);
    lv_obj_set_style_bg_opa(digi_box, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(digi_box, 0, 0);
    lv_obj_align(digi_box, LV_ALIGN_CENTER, 0, -115); 
    lv_obj_clear_flag(digi_box, LV_OBJ_FLAG_SCROLLABLE);

    digi_speed_label = lv_label_create(digi_box);
    lv_obj_set_style_text_color(digi_speed_label, palette_white, 0);
    lv_obj_set_style_text_font(digi_speed_label, &futura_medium_24, 0);
    lv_obj_center(digi_speed_label);
}

void draw_needle(void) {
    needle = lv_line_create(scr);
    static lv_style_t style_needle;
    lv_style_init(&style_needle);
    lv_style_set_line_width(&style_needle, 7);
    lv_style_set_line_color(&style_needle, palette_amber);
    lv_style_set_line_rounded(&style_needle, true);
    lv_obj_add_style(needle, &style_needle, 0);

    hub = lv_obj_create(scr);
    lv_obj_set_size(hub, 44, 44);
    lv_obj_set_style_bg_color(hub, palette_black, 0);
    lv_obj_set_style_bg_opa(hub, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(hub, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(hub, 0, 0);
    lv_obj_center(hub);
}

// --- BOOT ANIMATION TRIGGER ---

void start_boot_animation(void) {
    // 1. Black Fade Overlay
    lv_obj_t * fade_overlay = lv_obj_create(lv_scr_act());
    lv_obj_set_size(fade_overlay, dimension, dimension);
    lv_obj_set_style_bg_color(fade_overlay, palette_black, 0);
    lv_obj_set_style_radius(fade_overlay, 0, 0);
    lv_obj_set_style_border_width(fade_overlay, 0, 0);
    lv_obj_center(fade_overlay);

    lv_anim_t a_fade;
    lv_anim_init(&a_fade);
    lv_anim_set_var(&a_fade, fade_overlay);
    lv_anim_set_values(&a_fade, 255, 0);
    lv_anim_set_time(&a_fade, 1000);
    lv_anim_set_exec_cb(&a_fade, (lv_anim_exec_xcb_t)fade_anim_cb);
    lv_anim_set_ready_cb(&a_fade, fade_out_cleanup);
    lv_anim_start(&a_fade);

    // 2. Needle Sweep (Digital readout stays at 0)
    lv_anim_t a_sweep;
    lv_anim_init(&a_sweep);
    lv_anim_set_var(&a_sweep, NULL);
    lv_anim_set_values(&a_sweep, 0, 300);
    lv_anim_set_time(&a_sweep, 1200); 
    lv_anim_set_playback_time(&a_sweep, 1000); 
    lv_anim_set_exec_cb(&a_sweep, (lv_anim_exec_xcb_t)boot_needle_sweep_cb);
    lv_anim_set_path_cb(&a_sweep, lv_anim_path_ease_in_out);
    lv_anim_start(&a_sweep);
}

// --- BUILD UI ---

void Build_UI(void){
    scr = lv_scr_act();    
    lv_obj_set_style_bg_color(scr, palette_white, 0);

    draw_background(); 
    draw_odometers();
    draw_branding();
    draw_gear_selector(); 
    draw_needle(); 
    
    // Initial State
    update_odometer(125400, 42.5f);
    update_gear("P"); 
    lv_label_set_text(digi_speed_label, "0");
    boot_needle_sweep_cb(NULL, 0); 

    start_boot_animation();
}

void lvgl_live_preview_init(void) {
    Build_UI();
}