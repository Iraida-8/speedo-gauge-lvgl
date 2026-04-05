#include "Speedo_demo.h"
#include <stdio.h>
#include "fonts/montserrat_bold_28.h"
#include "fonts/montserrat_bold_80.h"
#include "fonts/montserrat_bold_120.h"

// Global components
lv_obj_t *scr; // Screen background

// Color palette
lv_color_t palette_black = LV_COLOR_MAKE(0, 0, 0);

// Attributes
const int dimension = 480;



void Build_UI(void){
    scr = lv_scr_act();    
    lv_obj_set_style_bg_color(scr, palette_black, 0);
}

void lvgl_live_preview_init(void)
{
    Build_UI();
}