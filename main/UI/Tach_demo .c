#include "Tach_demo.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "fonts/futura_medium_24.c"

// --- Global UI Component Pointers ---
lv_obj_t *scr; 
lv_obj_t *canvas;
static lv_color_t canvas_buf[400 * 400];
const int dimension = 400; 

static lv_obj_t *tach_needle;
static lv_obj_t *tach_hub;
static lv_obj_t *hour_hand;
static lv_obj_t *minute_hand;
static lv_obj_t *clock_hub;

// Color palette
lv_color_t palette_black = LV_COLOR_MAKE(0, 0, 0);
lv_color_t palette_white = LV_COLOR_MAKE(255, 255, 255);
lv_color_t palette_red   = LV_COLOR_MAKE(220, 10, 10);
lv_color_t palette_amber = LV_COLOR_MAKE(255, 170, 0);

// Helper for center points
const int cx = 200; 
const int cy = 200; 
const int clock_cy = 312; // MOVED UP from 320

void draw_rpm_background(void) {
    canvas = lv_canvas_create(scr);
    lv_canvas_set_buffer(canvas, canvas_buf, dimension, dimension, LV_IMG_CF_TRUE_COLOR);
    lv_obj_center(canvas);
    lv_canvas_fill_bg(canvas, palette_white, LV_OPA_COVER);

    // --- 1. RPM GAUGE SECTION ---
    float start_angle = 210.0f;
    float end_angle = -30.0f;
    int r_outer = 190; 

    lv_draw_arc_dsc_t red_dsc;
    lv_draw_arc_dsc_init(&red_dsc);
    red_dsc.color = palette_red;
    red_dsc.width = 30; 
    lv_canvas_draw_arc(canvas, cx, cy, r_outer - 5, (int)(360 - 20.7f), (int)(360 - (-30.0f)), &red_dsc);

    for(int v = 0; v <= 70; v += 5) {
        float angle = start_angle + (float)v * (end_angle - start_angle) / 70.0f;
        float rad = angle * (M_PI / 180.0f);
        int x1 = cx + (int)(r_outer * cosf(rad));
        int y1 = cy - (int)(r_outer * sinf(rad));
        int x2 = cx + (int)(150 * cosf(rad));
        int y2 = cy - (int)(150 * sinf(rad));

        lv_draw_line_dsc_t dsc;
        lv_draw_line_dsc_init(&dsc);
        dsc.color = (v > 60 && v < 70) ? palette_white : palette_black;
        dsc.width = (v % 10 == 0 || v == 5) ? 8 : 5;
        lv_point_t pts[2] = {{x1, y1}, {x2, y2}};
        lv_canvas_draw_line(canvas, pts, 2, &dsc);

        if(v == 5 || (v % 10 == 0 && v > 0)) {
            char buf[8]; sprintf(buf, "%d", v);
            int tx = cx + (int)(125 * cosf(rad));
            int ty = cy - (int)(125 * sinf(rad));
            lv_draw_label_dsc_t label_dsc;
            lv_draw_label_dsc_init(&label_dsc);
            label_dsc.color = palette_black;
            label_dsc.font = &futura_medium_24; 
            label_dsc.align = LV_TEXT_ALIGN_CENTER;
            lv_canvas_draw_text(canvas, tx - 20, ty - 12, 40, &label_dsc, buf);
        }
    }

    // --- 2. ENGINE LABEL ---
    lv_draw_label_dsc_t engine_dsc;
    lv_draw_label_dsc_init(&engine_dsc);
    engine_dsc.color = palette_black;
    engine_dsc.font = &futura_medium_24; 
    engine_dsc.align = LV_TEXT_ALIGN_CENTER;
    lv_canvas_draw_text(canvas, cx - 100, 145, 200, &engine_dsc, "6.0 DOHC-V8");

    // --- 3. IWC CLOCK SUB-DIAL ---
    int clock_r_outer = 85; 
    int clock_r_base = 79;  
    int hour_inner_r = 58;  
    int hour_outer_r = 72;  

    // BLACK FILL RING
    lv_draw_arc_dsc_t black_fill_dsc;
    lv_draw_arc_dsc_init(&black_fill_dsc);
    black_fill_dsc.color = palette_black;
    black_fill_dsc.width = 15; 
    lv_canvas_draw_arc(canvas, cx, clock_cy, clock_r_base - 7, 0, 360, &black_fill_dsc);

    // RED OUTER RING
    lv_draw_arc_dsc_t base_ring_dsc;
    lv_draw_arc_dsc_init(&base_ring_dsc);
    base_ring_dsc.color = palette_red;
    base_ring_dsc.width = 2; 
    lv_canvas_draw_arc(canvas, cx, clock_cy, clock_r_base, 0, 360, &base_ring_dsc);

    for(int m = 0; m < 60; m++) {
        float angle = 90.0f - (m * 6.0f);
        float rad = angle * (M_PI / 180.0f);
        lv_point_t p[2] = {
            {cx + (int)(clock_r_outer * cosf(rad)), clock_cy - (int)(clock_r_outer * sinf(rad))},
            {cx + (int)(clock_r_base * cosf(rad)), clock_cy - (int)(clock_r_base * sinf(rad))}
        };
        lv_draw_line_dsc_t l_dsc;
        lv_draw_line_dsc_init(&l_dsc);

        // UPDATED: Added 5, 25, 35, 55 (which correspond to 1, 5, 7, 11 o'clock)
        if (m % 10 == 0 || m == 15 || m == 45 || m == 5 || m == 25 || m == 35 || m == 55) {
            l_dsc.color = palette_red;
            l_dsc.width = 4;
        } else {
            l_dsc.color = palette_black;
            l_dsc.width = 1;
        }
        lv_canvas_draw_line(canvas, p, 2, &l_dsc);
    }

    // --- HOUR MARKERS WITH OUTLINES ---
    for(int h = 1; h <= 12; h++) {
        float angle = 90.0f - (h * 30.0f);
        float rad = angle * (M_PI / 180.0f);
        
        if (h % 3 == 0) {
            char h_buf[4]; sprintf(h_buf, "%d", h);
            int tx = cx + (int)(hour_inner_r * cosf(rad));
            int ty = clock_cy - (int)(hour_inner_r * sinf(rad));
            
            lv_draw_label_dsc_t h_lbl;
            lv_draw_label_dsc_init(&h_lbl);
            h_lbl.font = &futura_medium_24;
            h_lbl.align = LV_TEXT_ALIGN_CENTER;

            h_lbl.color = palette_black;
            lv_canvas_draw_text(canvas, tx - 16, ty - 12, 30, &h_lbl, h_buf);
            lv_canvas_draw_text(canvas, tx - 14, ty - 12, 30, &h_lbl, h_buf);
            lv_canvas_draw_text(canvas, tx - 15, ty - 13, 30, &h_lbl, h_buf);
            lv_canvas_draw_text(canvas, tx - 15, ty - 11, 30, &h_lbl, h_buf);

            h_lbl.color = palette_white;
            lv_canvas_draw_text(canvas, tx - 15, ty - 12, 30, &h_lbl, h_buf);
        } else {
            lv_point_t p[2] = {
                {cx + (int)(hour_inner_r * cosf(rad)), clock_cy - (int)(hour_inner_r * sinf(rad))},
                {cx + (int)(hour_outer_r * cosf(rad)), clock_cy - (int)(hour_outer_r * sinf(rad))}
            };
            
            lv_draw_line_dsc_t l_dsc;
            lv_draw_line_dsc_init(&l_dsc);

            l_dsc.color = palette_black;
            l_dsc.width = 5; 
            lv_canvas_draw_line(canvas, p, 2, &l_dsc);

            l_dsc.color = palette_white;
            l_dsc.width = 3; 
            lv_canvas_draw_line(canvas, p, 2, &l_dsc);
        }
    }

    // --- 3.5 DATE WINDOW (COMPLICATION) ---
    int date_x = cx + 20;  // Positioned to the right of center
    int date_y = clock_cy - 10; // Centered vertically with the hands
    int date_w = 26;
    int date_h = 20;

    // Draw the Border/Background of the date box
    lv_draw_rect_dsc_t date_rect_dsc;
    lv_draw_rect_dsc_init(&date_rect_dsc);
    date_rect_dsc.bg_color = palette_white;
    date_rect_dsc.bg_opa = LV_OPA_COVER;
    date_rect_dsc.border_width = 2;
    date_rect_dsc.border_color = palette_black;
    date_rect_dsc.radius = 2; // Slightly rounded corners
    
    // Create the coordinates for the box
    lv_area_t date_area;
    date_area.x1 = date_x;
    date_area.y1 = date_y;
    date_area.x2 = date_x + date_w;
    date_area.y2 = date_y + date_h;
    
    // Draw the box onto the canvas
    lv_canvas_draw_rect(canvas, date_x, date_y, date_w, date_h, &date_rect_dsc);

    // Draw the Day Number (e.g., "24")
    char day_buf[4]; 
    sprintf(day_buf, "05"); // You can replace this with a real day variable later
    
    lv_draw_label_dsc_t date_lbl_dsc;
    lv_draw_label_dsc_init(&date_lbl_dsc);
    date_lbl_dsc.color = palette_black;
    date_lbl_dsc.font = &lv_font_montserrat_14; // Clean, readable size
    date_lbl_dsc.align = LV_TEXT_ALIGN_CENTER;
    
    // Draw the text inside the box (centered)
    lv_canvas_draw_text(canvas, date_x, date_y + 2, date_w, &date_lbl_dsc, day_buf);

    // --- 4. BRANDING ---
    lv_draw_label_dsc_t brand_dsc;
    lv_draw_label_dsc_init(&brand_dsc);
    brand_dsc.color = palette_black;
    brand_dsc.font = &lv_font_montserrat_12; 
    brand_dsc.align = LV_TEXT_ALIGN_CENTER;
    lv_canvas_draw_text(canvas, cx - 50, clock_cy - 40, 100, &brand_dsc, "IWC");

    lv_draw_label_dsc_t tiny_brand_dsc;
    lv_draw_label_dsc_init(&tiny_brand_dsc);
    tiny_brand_dsc.color = palette_black;
    tiny_brand_dsc.font = &lv_font_montserrat_8; 
    tiny_brand_dsc.align = LV_TEXT_ALIGN_CENTER;
    tiny_brand_dsc.letter_space = 2; 
    lv_canvas_draw_text(canvas, cx - 50, clock_cy + 25, 100, &tiny_brand_dsc, "SCHAFFHAUSEN");

    // --- 5. TOP TEXT ---
    lv_draw_label_dsc_t unit_dsc;
    lv_draw_label_dsc_init(&unit_dsc);
    unit_dsc.color = palette_black;
    unit_dsc.font = &lv_font_montserrat_16; 
    unit_dsc.align = LV_TEXT_ALIGN_CENTER;
    lv_canvas_draw_text(canvas, cx - 50, 95, 100, &unit_dsc, "x100\n1/min");
}

void draw_tach_needle(float current_rpm) {
    float start_angle = 210.0f;
    float end_angle = -30.0f;
    float angle = start_angle + (current_rpm * (end_angle - start_angle) / 70.0f);
    float rad = angle * (M_PI / 180.0f);

    int needle_len = 152; 
    int x_end = cx + (int)(needle_len * cosf(rad));
    int y_end = cy - (int)(needle_len * sinf(rad));

    static lv_point_t needle_pts[2];
    needle_pts[0].x = cx; needle_pts[0].y = cy;
    needle_pts[1].x = x_end; needle_pts[1].y = y_end;

    if(!tach_needle) {
        tach_needle = lv_line_create(scr);
        static lv_style_t style_needle;
        lv_style_init(&style_needle);
        lv_style_set_line_width(&style_needle, 7);
        lv_style_set_line_color(&style_needle, palette_amber);
        lv_style_set_line_rounded(&style_needle, true);
        lv_obj_add_style(tach_needle, &style_needle, 0);
    }
    lv_line_set_points(tach_needle, needle_pts, 2);

    if(!tach_hub) {
        tach_hub = lv_obj_create(scr);
        lv_obj_set_size(tach_hub, 44, 44);
        lv_obj_set_style_bg_color(tach_hub, palette_black, 0);
        lv_obj_set_style_bg_opa(tach_hub, LV_OPA_COVER, 0);
        lv_obj_set_style_radius(tach_hub, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_border_width(tach_hub, 0, 0);
        lv_obj_align(tach_hub, LV_ALIGN_TOP_LEFT, cx - 22, cy - 22);
    }
    lv_obj_move_foreground(tach_needle);
    lv_obj_move_foreground(tach_hub);
}

void update_clock_hands(int hours, int minutes) {
    float minute_angle = 90.0f - (minutes * 6.0f); 
    float hour_angle = 90.0f - ((hours % 12) * 30.0f + (minutes * 0.5f));
    int h_len = 45; int m_len = 65; // Adjusted hand lengths for enlarged clock

    static lv_point_t m_pts[2];
    m_pts[0] = (lv_point_t){cx, clock_cy};
    m_pts[1].x = cx + (int)(m_len * cosf(minute_angle * M_PI / 180.0f));
    m_pts[1].y = clock_cy - (int)(m_len * sinf(minute_angle * M_PI / 180.0f));

    if(!minute_hand) {
        minute_hand = lv_line_create(scr);
        lv_obj_set_style_line_width(minute_hand, 5, 0);
        lv_obj_set_style_line_color(minute_hand, palette_amber, 0);
        lv_obj_set_style_line_rounded(minute_hand, true, 0);
    }
    lv_line_set_points(minute_hand, m_pts, 2);

    static lv_point_t h_pts[2];
    h_pts[0] = (lv_point_t){cx, clock_cy};
    h_pts[1].x = cx + (int)(h_len * cosf(hour_angle * M_PI / 180.0f));
    h_pts[1].y = clock_cy - (int)(h_len * sinf(hour_angle * M_PI / 180.0f));

    if(!hour_hand) {
        hour_hand = lv_line_create(scr);
        lv_obj_set_style_line_width(hour_hand, 7, 0);
        lv_obj_set_style_line_color(hour_hand, palette_amber, 0);
        lv_obj_set_style_line_rounded(hour_hand, true, 0);
    }
    lv_line_set_points(hour_hand, h_pts, 2);

    if(!clock_hub) {
        clock_hub = lv_obj_create(scr);
        lv_obj_set_size(clock_hub, 20, 20);
        lv_obj_set_style_bg_color(clock_hub, palette_black, 0);
        lv_obj_set_style_bg_opa(clock_hub, LV_OPA_COVER, 0);
        lv_obj_set_style_radius(clock_hub, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_border_width(clock_hub, 0, 0);
        lv_obj_align(clock_hub, LV_ALIGN_TOP_LEFT, cx - 10, clock_cy - 10);
    }
    lv_obj_move_foreground(minute_hand);
    lv_obj_move_foreground(hour_hand);
    lv_obj_move_foreground(clock_hub);
}

void sync_time_task(lv_timer_t * timer) {
    time_t now; struct tm timeinfo;
    time(&now); localtime_r(&now, &timeinfo);
    update_clock_hands(timeinfo.tm_hour, timeinfo.tm_min);
}

static void boot_needle_sweep_cb(void * var, int32_t rpm) {
    float start_angle = 210.0f;
    float end_angle = -30.0f;
    float angle = start_angle + (float)rpm * (end_angle - start_angle) / 70.0f;
    float rad = angle * (M_PI / 180.0f);

    int needle_len = 152; 
    static lv_point_t needle_pts[2];
    needle_pts[0].x = cx; needle_pts[0].y = cy;
    needle_pts[1].x = cx + (int)(needle_len * cosf(rad));
    needle_pts[1].y = cy - (int)(needle_len * sinf(rad));

    if(tach_needle) {
        lv_line_set_points(tach_needle, needle_pts, 2);
    }
}

// Add this helper right above run_startup_animation
static void fade_anim_cb(void * var, int32_t v) {
    lv_obj_set_style_bg_opa((lv_obj_t *)var, v, 0);
}

void run_startup_animation(void) {
    lv_obj_t * fade_overlay = lv_obj_create(lv_scr_act());
    lv_obj_set_size(fade_overlay, 400, 400);
    lv_obj_set_style_bg_color(fade_overlay, lv_color_hex(0x000000), 0);
    lv_obj_set_style_radius(fade_overlay, 0, 0);
    lv_obj_set_style_border_width(fade_overlay, 0, 0);
    lv_obj_center(fade_overlay);
    
    lv_obj_move_foreground(fade_overlay);

    lv_anim_t a_fade;
    lv_anim_init(&a_fade);
    lv_anim_set_var(&a_fade, fade_overlay);
    lv_anim_set_values(&a_fade, 255, 0); // Start black, fade to clear
    lv_anim_set_time(&a_fade, 1000);
    // FIXED: Use the helper function instead of the raw style setter
    lv_anim_set_exec_cb(&a_fade, (lv_anim_exec_xcb_t)fade_anim_cb);
    lv_anim_set_ready_cb(&a_fade, lv_obj_del_anim_ready_cb);
    lv_anim_start(&a_fade);

    lv_anim_t a_sweep;
    lv_anim_init(&a_sweep);
    lv_anim_set_var(&a_sweep, NULL);
    lv_anim_set_values(&a_sweep, 0, 70);
    lv_anim_set_time(&a_sweep, 1200); 
    lv_anim_set_playback_time(&a_sweep, 1000); 
    lv_anim_set_exec_cb(&a_sweep, (lv_anim_exec_xcb_t)boot_needle_sweep_cb);
    lv_anim_set_path_cb(&a_sweep, lv_anim_path_ease_in_out);
    lv_anim_start(&a_sweep);
}

void Build_UI(void){
    scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, palette_white, 0);
    draw_rpm_background();
    draw_tach_needle(0.0f); 
    time_t now; struct tm timeinfo;
    time(&now); localtime_r(&now, &timeinfo);
    update_clock_hands(timeinfo.tm_hour, timeinfo.tm_min);
    lv_timer_create(sync_time_task, 1000, NULL);
    run_startup_animation();
}

void lvgl_live_preview_init(void) {
    Build_UI();
}