#include "main.h"
#include "liblvgl/lvgl.h"
#include "autons.hpp"

// Global variable
int selected_auton = 1; 

// Track if the debug run is active so we don't start it twice
bool is_debug_running = false;

// Styles
static lv_style_t style_btn_default;
static lv_style_t style_btn_checked;
static lv_style_t style_debug_default;
static lv_style_t style_debug_checked;
static lv_obj_t * debug_label_ptr = NULL;

// --- 1. THE DEBUG AUTON TASK ---
// This is the separate thread that runs the actual robot movement
void debug_auton_task_fn(void* param) {
    // Put your test code here!
    printf("Debug run started!\n");
    switch (selected_auton) {
        case 1: left_qual_auton();   break;
        case 2: right_qual_auton();  break;
        case 3: skills_auton_for_qual(); break;
        case 4: right_qual_auton_center(); break;
        case 5: left_qual_auton_ram(); break;
        
        default: 
            // Fallback if something goes wrong
            printf("No auton selected!\n");
            break;
    }

    printf("Debug run finished!\n");
    if (debug_label_ptr != NULL) {
        lv_label_set_text(debug_label_ptr, "debug test run");
    }

    // When finished, we need to reset the text back to "debug test run"
    // (We accept that this part might not update the UI instantly until the next click 
    // without complex mutexes, but it's fine for simple testing)
    is_debug_running = false;
}

static void auton_click_cb(lv_event_t * e) {
    lv_obj_t * clicked_btn = (lv_obj_t *)lv_event_get_target(e);
    lv_obj_t * parent = lv_obj_get_parent(clicked_btn);
    int btn_index = lv_obj_get_index(clicked_btn);

    // FIX: Only update the selected_auton if it's NOT the debug button
    if (btn_index < 7) {
        selected_auton = btn_index + 1;
    }

    uint32_t child_cnt = lv_obj_get_child_count(parent);
    for(uint32_t i = 0; i < child_cnt; i++) {
        lv_obj_t * btn = lv_obj_get_child(parent, i);
        
        // We only want to remove the checked state from the OTHER auton buttons (0-6)
        // We keep the highlight on the auton we chose so we know what the debug button will run
        if (btn_index < 7 && i < 7) {
            lv_obj_remove_state(btn, LV_STATE_CHECKED);
        }
        
        lv_obj_t * label = lv_obj_get_child(btn, 0);

        // Debug Button (Index 7) UI Logic
        if (i == 7) {
            if (btn_index == 7) {
                lv_label_set_text(label, "running...");
                if (!is_debug_running) {
                    is_debug_running = true;
                    pros::Task debug_task(debug_auton_task_fn, NULL, "DebugTask");
                }
            } else {
                lv_label_set_text(label, "debug test run");
            }
        }
    }

    // Only visually "check" the button if it's one of the first 7
    if (btn_index < 7) {
        lv_obj_add_state(clicked_btn, LV_STATE_CHECKED);
    }
}

void create_auton_selector() {
    // --- Styles (Same as before) ---
    lv_style_init(&style_btn_default);
    lv_style_set_bg_color(&style_btn_default, lv_color_hex(0x575E57)); 
    lv_style_set_bg_opa(&style_btn_default, LV_OPA_COVER);
    lv_style_set_border_width(&style_btn_default, 4);
    lv_style_set_border_color(&style_btn_default, lv_color_hex(0x5B875B)); 
    lv_style_set_text_color(&style_btn_default, lv_color_white());

    lv_style_init(&style_btn_checked);
    lv_style_set_bg_color(&style_btn_checked, lv_color_hex(0x5B875B)); 
    lv_style_set_border_color(&style_btn_checked, lv_color_hex(0x575E57)); 
    lv_style_set_text_color(&style_btn_checked, lv_color_black());

    lv_style_init(&style_debug_default);
    lv_style_set_bg_color(&style_debug_default, lv_palette_main(LV_PALETTE_INDIGO));
    lv_style_set_bg_opa(&style_debug_default, LV_OPA_COVER);
    lv_style_set_border_width(&style_debug_default, 4);
    lv_style_set_border_color(&style_debug_default, lv_palette_lighten(LV_PALETTE_INDIGO, 3));
    lv_style_set_text_color(&style_debug_default, lv_color_white());

    lv_style_init(&style_debug_checked);
    lv_style_set_bg_color(&style_debug_checked, lv_palette_lighten(LV_PALETTE_INDIGO, 3));
    lv_style_set_border_color(&style_debug_checked, lv_palette_main(LV_PALETTE_INDIGO));
    lv_style_set_text_color(&style_debug_checked, lv_color_black());

    // --- Container ---
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, 480, 240);
    lv_obj_center(cont);
    lv_obj_remove_flag(cont, LV_OBJ_FLAG_SCROLLABLE); 
    lv_obj_set_style_pad_all(cont, 5, 0);
    lv_obj_set_style_bg_opa(cont, 0, 0); 
    lv_obj_set_style_border_width(cont, 0, 0);

    static int32_t column_dsc[] = {112, 112, 112, 112, LV_GRID_TEMPLATE_LAST};
    static int32_t row_dsc[] = {108, 108, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(cont, column_dsc, row_dsc);
    lv_obj_set_style_pad_column(cont, 8, 0); 
    lv_obj_set_style_pad_row(cont, 8, 0);

    // --- Buttons ---
    for(int i = 0; i < 8; i++) {
        lv_obj_t* btn = lv_button_create(cont);
        lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, i % 4, 1, 
                                  LV_GRID_ALIGN_STRETCH, i / 4, 1);
        
        lv_obj_add_event_cb(btn, auton_click_cb, LV_EVENT_CLICKED, NULL);

        if (i == 7) { 
            lv_obj_add_style(btn, &style_debug_default, 0);
            lv_obj_add_style(btn, &style_debug_checked, LV_STATE_CHECKED);
        } else {
            lv_obj_add_style(btn, &style_btn_default, 0);
            lv_obj_add_style(btn, &style_btn_checked, LV_STATE_CHECKED);
        }

        if(i == 0) lv_obj_add_state(btn, LV_STATE_CHECKED);
        
        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
        lv_obj_set_width(label, 100); 
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_center(label); 

        if (i == 7) {
            debug_label_ptr = label; 
        }

        switch (i) {
            //change to match auton name here
            case 0: lv_label_set_text(label, "Left Qual Auton"); break;
            case 1: lv_label_set_text(label, "Right Qual Auton"); break;
            case 2: lv_label_set_text(label, "Skills Auton Qual"); break;
            case 3: lv_label_set_text(label, "Right Qual Center"); break;
            case 4: lv_label_set_text(label, "Left Qual Ram"); break;
            case 5: lv_label_set_text(label, "Defensive Mode"); break;
            case 6: lv_label_set_text(label, "High Speed Mode"); break;
            case 7: lv_label_set_text(label, "debug test run"); break;
        }
    }
}