/* Copyright 2021 Kyle McCreery
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include QMK_KEYBOARD_H
#include <stdio.h>

// OLED setup for bongocat
#define IDLE_FRAMES 3
#define IDLE_SPEED 1000
// #define TAP_FRAMES 7
// #define TAP_SPEED 50
#define ANIM_FRAME_DURATION 700
#define ANIM_SIZE 512

/* Matrix display is 19 x 9 pixels */
#define MATRIX_DISPLAY_X 103
#define MATRIX_DISPLAY_Y 30

//unit
#define GAP 2
#define CUBE_NUMBER 7.5
#define SPACE_UNIT 50
#define TAB_UNIT 1.25
#define CAPS_UNIT 1.5
#define ENTER_UNIT 2
#define RSHIFT_UNIT 1.75

//row
#define R1 1
#define R2 2
#define R3 3
#define R4 4

static long int oled_timeout = 3500;
bool gui_on = true;
bool display_keyboard = false;
uint32_t anim_timer = 0;
uint32_t anim_sleep = 0;
uint8_t current_idle_frame = 0;
uint8_t current_tap_frame = 0;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[0] = LAYOUT_all(
                                                                                                                KC_MUTE,
    KC_ESC,           KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSPC,
    KC_TAB,            KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_QUOT, KC_ENT,
    KC_LSFT, KC_SLSH, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,           KC_RSFT,
    KC_LCTL, KC_LGUI, KC_LALT,          KC_SPC,  KC_SPC,           KC_SPC,           KC_RALT, MO(2),            MO(1) ),

  [1] = LAYOUT_all(
                                                                                                                KC_TRNS,
    KC_ESC,           KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSPC,
    KC_TAB,            KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_QUOT, KC_ENT,
    KC_LSFT, KC_SLSH, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,           KC_RSFT,
    KC_LCTL, KC_LGUI, KC_LALT,          KC_SPC,  KC_SPC,           KC_SPC,           KC_RALT, MO(2),            KC_RCTL ),

  [2] = LAYOUT_all(
                                                                                                                KC_TRNS,
    KC_TRNS,          KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_TRNS,
    KC_CAPS,          KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS,
    KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS, KC_TRNS,          KC_TRNS,          KC_TRNS, KC_TRNS,          KC_TRNS ),

  [3] = LAYOUT_all(
                                                                                                                KC_TRNS,
  	KC_TRNS,          KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
  	KC_TRNS,          KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
  	KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS,
  	KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS, KC_TRNS,          KC_TRNS,          KC_TRNS, KC_TRNS,          KC_TRNS )
};

#ifdef ENCODER_ENABLE       // Encoder Functionality
    uint8_t selected_layer = 0;
    bool encoder_update_user(uint8_t index, bool clockwise) {
        #ifdef OLED_ENABLE
            oled_clear();
            oled_render();
        #endif
        switch (index) {
            case 0:         // This is the only encoder right now, keeping for consistency
                if ( clockwise ) {
                    if ( selected_layer  < 3 && keyboard_report->mods & MOD_BIT(KC_LSFT) ) { // If you are holding L shift, encoder changes layers
                        selected_layer ++;
                        layer_move(selected_layer);
                    } else {
                        tap_code(KC_VOLU);   // Otherwise it just changes volume
                    }
                } else if ( !clockwise ) {
                    if ( selected_layer  > 0 && keyboard_report->mods & MOD_BIT(KC_LSFT) ){
                        selected_layer --;
                        layer_move(selected_layer);
                    } else {
                        tap_code(KC_VOLD);
                    }
                }
        }
    return true;
    }
#endif

#ifdef OLED_ENABLE   // OLED Functionality
    oled_rotation_t oled_init_user(oled_rotation_t rotation) {
        return OLED_ROTATION_180;       // flips the display 180 degrees if offhand
    }

    bool clear_screen = false;          // used to manage singular screen clears to prevent display glitch

    static void render_anim(void) {

        // start screen animation

    static const char PROGMEM rest[IDLE_FRAMES][ANIM_SIZE] = {
        
    {
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 26, 30,  6,  0, 24,200,120, 24,  0,  0,  0,  0,128,192, 64, 64,192,192,192,192,192,192,192,192,192,192,192,192,224,192,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,224,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,129,248, 60, 30, 27, 17, 48, 48, 60, 63, 63, 63, 63, 63, 63, 19, 24, 24,124,254,131,193,195, 31,124,224,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 16,255,127,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,255,225, 15,140,192,113, 63, 12,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
        2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 14, 15, 12, 12, 44, 60, 28, 28, 12, 12, 12, 12, 12, 44, 60, 28, 12, 12, 12, 12, 12, 13, 15, 14,  7,  3,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
    },
    {
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 24,200,120, 24,  0,  0,  0,  0,128,192, 64, 64,192,192,192,192,192,192,192,192,192,192,192,192,224,192,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,224,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,129,248, 60, 30, 27, 17, 48, 48, 60, 63, 63, 63, 63, 63, 63, 19, 24, 24,124,254,131,193,195, 31,124,224,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 16,255,127,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,255,225, 15,140,192,113, 63, 12,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
        2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 14, 15, 12, 12, 44, 60, 28, 28, 12, 12, 12, 12, 12, 44, 60, 28, 12, 12, 12, 12, 12, 13, 15, 14,  7,  3,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
    },
    {
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,192, 64, 64,192,192,192,192,192,192,192,192,192,192,192,192,224,192,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,224,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,248, 60, 30, 27, 17, 48, 48, 60, 63, 63, 63, 63, 63, 63, 19, 24, 24,124,254,131,193,195, 31,124,224,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 16,255,127,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,255,225, 15,140,192,113, 63, 12,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
        2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 14, 15, 12, 12, 44, 60, 28, 28, 12, 12, 12, 12, 12, 44, 60, 28, 12, 12, 12, 12, 12, 13, 15, 14,  7,  3,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
    }


    };

    void animation_phase(void) {
            current_idle_frame = (current_idle_frame + 1) % IDLE_FRAMES;
            oled_write_raw_P(rest[abs((IDLE_FRAMES-1)-current_idle_frame)], ANIM_SIZE);
    }

    if (get_current_wpm() != 000) {
        oled_on();

        if (timer_elapsed32(anim_timer) > ANIM_FRAME_DURATION) {
            anim_timer = timer_read32();
            animation_phase();
        }

        anim_sleep = timer_read32();
    } else {
        if (timer_elapsed32(anim_sleep) > oled_timeout) {
            oled_off();
        } else {
            if (timer_elapsed32(anim_timer) > ANIM_FRAME_DURATION) {
                anim_timer = timer_read32();
                animation_phase();
            }
        }
    }
}
    static void render_keyboard(void){

        for (uint8_t x = 0; x < MATRIX_DISPLAY_X; x++) {
            oled_write_pixel(x,0,true);
        }
        for (uint8_t y = 0; y < MATRIX_DISPLAY_Y; y++) {
            oled_write_pixel(0,y,true);
        }
        for (uint8_t x = 0; x < MATRIX_DISPLAY_X; x++) {
            oled_write_pixel(x,MATRIX_DISPLAY_Y,true);
        }
        for (uint8_t y = 0; y < MATRIX_DISPLAY_Y; y++) {
            oled_write_pixel(MATRIX_DISPLAY_X,y,true);
        }

    }

    bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (display_keyboard == true){
        display_keyboard = false;
        switch (keycode) {
                case KC_ESC:
                    if (record->event.pressed) {
                        // x = position, y = row
                        for (uint8_t x = 0; x < CUBE_NUMBER*TAB_UNIT; x++) {
                            for (uint8_t y = R1; y < CUBE_NUMBER; y++) {
                                oled_write_pixel(x,y,true);
                            }
                        }
                    } else {
                        for (uint8_t x = 0; x < CUBE_NUMBER*TAB_UNIT; x++) {
                            for (uint8_t y = R1; y < CUBE_NUMBER; y++) {
                                oled_write_pixel(x,y,false);
                            }
                        }
                    }
                    return true;
                case KC_Q:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER+TAB_UNIT+GAP; x < CUBE_NUMBER*2+TAB_UNIT; x++) {
                        for (uint8_t y = R1; y < CUBE_NUMBER; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER+TAB_UNIT+GAP; x < CUBE_NUMBER*2+TAB_UNIT; x++) {
                        for (uint8_t y = R1; y < CUBE_NUMBER; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_W:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*2+TAB_UNIT+GAP; x < CUBE_NUMBER*3+TAB_UNIT; x++) {
                        for (uint8_t y = R1; y < CUBE_NUMBER; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*2+TAB_UNIT+GAP; x < CUBE_NUMBER*3+TAB_UNIT; x++) {
                        for (uint8_t y = R1; y < CUBE_NUMBER; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_E:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*3+TAB_UNIT+GAP; x < CUBE_NUMBER*4+TAB_UNIT; x++) {
                        for (uint8_t y = R1; y < CUBE_NUMBER; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*3+TAB_UNIT+GAP; x < CUBE_NUMBER*4+TAB_UNIT; x++) {
                        for (uint8_t y = R1; y < CUBE_NUMBER; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_R:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*4+TAB_UNIT+GAP; x < CUBE_NUMBER*5+TAB_UNIT; x++) {
                        for (uint8_t y = R1; y < CUBE_NUMBER; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*4+TAB_UNIT+GAP; x < CUBE_NUMBER*5+TAB_UNIT; x++) {
                        for (uint8_t y = R1; y < CUBE_NUMBER; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_T:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*5+TAB_UNIT+GAP; x < CUBE_NUMBER*6+TAB_UNIT; x++) {
                        for (uint8_t y = R1; y < CUBE_NUMBER; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*5+TAB_UNIT+GAP; x < CUBE_NUMBER*6+TAB_UNIT; x++) {
                        for (uint8_t y = R1; y < CUBE_NUMBER; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_Y:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*6+TAB_UNIT+GAP; x < CUBE_NUMBER*7+TAB_UNIT; x++) {
                        for (uint8_t y = R1; y < CUBE_NUMBER; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*6+TAB_UNIT+GAP; x < CUBE_NUMBER*7+TAB_UNIT; x++) {
                        for (uint8_t y = R1; y < CUBE_NUMBER; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_U:
                    if (record->event.pressed) {
                        // x = position, y = row
                        for (uint8_t x = CUBE_NUMBER*7+TAB_UNIT+GAP; x < CUBE_NUMBER*8+TAB_UNIT; x++) {
                            for (uint8_t y = R1; y < CUBE_NUMBER; y++) {
                                oled_write_pixel(x,y,true);
                            }
                        }
                    } else {
                        for (uint8_t x = CUBE_NUMBER*7+TAB_UNIT+GAP; x < CUBE_NUMBER*8+TAB_UNIT; x++) {
                            for (uint8_t y = R1; y < CUBE_NUMBER; y++) {
                                oled_write_pixel(x,y,false);
                            }
                        }
                    }
                    return true;
                case KC_I:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*8+TAB_UNIT+GAP; x < CUBE_NUMBER*9+TAB_UNIT; x++) {
                        for (uint8_t y = R1; y < CUBE_NUMBER; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*8+TAB_UNIT+GAP; x < CUBE_NUMBER*9+TAB_UNIT; x++) {
                        for (uint8_t y = R1; y < CUBE_NUMBER; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_O:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*9+TAB_UNIT+GAP; x < CUBE_NUMBER*10+TAB_UNIT; x++) {
                        for (uint8_t y = R1; y < CUBE_NUMBER; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*9+TAB_UNIT+GAP; x < CUBE_NUMBER*10+TAB_UNIT; x++) {
                        for (uint8_t y = R1; y < CUBE_NUMBER; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_P:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*10+TAB_UNIT+GAP; x < CUBE_NUMBER*11+TAB_UNIT; x++) {
                        for (uint8_t y = R1; y < CUBE_NUMBER; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*10+TAB_UNIT+GAP; x < CUBE_NUMBER*11+TAB_UNIT; x++) {
                        for (uint8_t y = R1; y < CUBE_NUMBER; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_BSPC:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*11+TAB_UNIT+GAP; x < CUBE_NUMBER*13+TAB_UNIT+ENTER_UNIT; x++) {
                        for (uint8_t y = R1; y < CUBE_NUMBER; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*11+TAB_UNIT+GAP; x < CUBE_NUMBER*13+TAB_UNIT+ENTER_UNIT; x++) {
                        for (uint8_t y = R1; y < CUBE_NUMBER; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_TAB:
                if (record->event.pressed) {
                    for (uint8_t x = 0; x < CUBE_NUMBER*CAPS_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER+GAP; y < CUBE_NUMBER*R2; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = 0; x < CUBE_NUMBER*CAPS_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER+GAP; y < CUBE_NUMBER*R2; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_A:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER+CAPS_UNIT+GAP; x < CUBE_NUMBER*2+CAPS_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER+GAP; y < CUBE_NUMBER*R2; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER+CAPS_UNIT+GAP; x < CUBE_NUMBER*2+CAPS_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER+GAP; y < CUBE_NUMBER*R2; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_S:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*2+CAPS_UNIT+GAP; x < CUBE_NUMBER*3+CAPS_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER+GAP; y < CUBE_NUMBER*R2; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*2+CAPS_UNIT+GAP; x < CUBE_NUMBER*3+CAPS_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER+GAP; y < CUBE_NUMBER*R2; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_D:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*3+CAPS_UNIT+GAP; x < CUBE_NUMBER*4+CAPS_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER+GAP; y < CUBE_NUMBER*R2; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*3+CAPS_UNIT+GAP; x < CUBE_NUMBER*4+CAPS_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER+GAP; y < CUBE_NUMBER*R2; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_F:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*4+CAPS_UNIT+GAP; x < CUBE_NUMBER*5+CAPS_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER+GAP; y < CUBE_NUMBER*R2; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*4+CAPS_UNIT+GAP; x < CUBE_NUMBER*5+CAPS_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER+GAP; y < CUBE_NUMBER*R2; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_G:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*5+CAPS_UNIT+GAP; x < CUBE_NUMBER*6+CAPS_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER+GAP; y < CUBE_NUMBER*R2; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*5+CAPS_UNIT+GAP; x < CUBE_NUMBER*6+CAPS_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER+GAP; y < CUBE_NUMBER*R2; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_H:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*6+CAPS_UNIT+GAP; x < CUBE_NUMBER*7+CAPS_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER+GAP; y < CUBE_NUMBER*R2; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*6+CAPS_UNIT+GAP; x < CUBE_NUMBER*7+CAPS_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER+GAP; y < CUBE_NUMBER*R2; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_J:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*7+CAPS_UNIT+GAP; x < CUBE_NUMBER*8+CAPS_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER+GAP; y < CUBE_NUMBER*R2; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*7+CAPS_UNIT+GAP; x < CUBE_NUMBER*8+CAPS_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER+GAP; y < CUBE_NUMBER*R2; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_K:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*8+CAPS_UNIT+GAP; x < CUBE_NUMBER*9+CAPS_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER+GAP; y < CUBE_NUMBER*R2; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*8+CAPS_UNIT+GAP; x < CUBE_NUMBER*9+CAPS_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER+GAP; y < CUBE_NUMBER*R2; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_L:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*9+CAPS_UNIT+GAP; x < CUBE_NUMBER*10+CAPS_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER+GAP; y < CUBE_NUMBER*R2; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*9+CAPS_UNIT+GAP; x < CUBE_NUMBER*10+CAPS_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER+GAP; y < CUBE_NUMBER*R2; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_ENT:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*10+CAPS_UNIT+GAP; x < CUBE_NUMBER*13+CAPS_UNIT+ENTER_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER+GAP; y < CUBE_NUMBER*R2; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*10+CAPS_UNIT+GAP; x < CUBE_NUMBER*13+CAPS_UNIT+ENTER_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER+GAP; y < CUBE_NUMBER*R2; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_LSFT:
                if (record->event.pressed) {
                    for (uint8_t x = 0; x < CUBE_NUMBER+ENTER_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*2+GAP; y < CUBE_NUMBER*R3; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = 0; x < CUBE_NUMBER+ENTER_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*2+GAP; y < CUBE_NUMBER*R3; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_Z:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER+ENTER_UNIT+GAP; x < CUBE_NUMBER*2+ENTER_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*2+GAP; y < CUBE_NUMBER*R3; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER+ENTER_UNIT+GAP; x < CUBE_NUMBER*2+ENTER_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*2+GAP; y < CUBE_NUMBER*R3; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_X:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*2+ENTER_UNIT+GAP; x < CUBE_NUMBER*3+ENTER_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*2+GAP; y < CUBE_NUMBER*R3; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*2+ENTER_UNIT+GAP; x < CUBE_NUMBER*3+ENTER_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*2+GAP; y < CUBE_NUMBER*R3; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_C:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*3+ENTER_UNIT+GAP; x < CUBE_NUMBER*4+ENTER_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*2+GAP; y < CUBE_NUMBER*R3; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*3+ENTER_UNIT+GAP; x < CUBE_NUMBER*4+ENTER_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*2+GAP; y < CUBE_NUMBER*R3; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_V:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*4+ENTER_UNIT+GAP; x < CUBE_NUMBER*5+ENTER_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*2+GAP; y < CUBE_NUMBER*R3; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*4+ENTER_UNIT+GAP; x < CUBE_NUMBER*5+ENTER_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*2+GAP; y < CUBE_NUMBER*R3; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_B:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*5+ENTER_UNIT+GAP; x < CUBE_NUMBER*6+ENTER_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*2+GAP; y < CUBE_NUMBER*R3; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*5+ENTER_UNIT+GAP; x < CUBE_NUMBER*6+ENTER_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*2+GAP; y < CUBE_NUMBER*R3; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_N:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*6+ENTER_UNIT+GAP; x < CUBE_NUMBER*7+ENTER_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*2+GAP; y < CUBE_NUMBER*R3; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*6+ENTER_UNIT+GAP; x < CUBE_NUMBER*7+ENTER_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*2+GAP; y < CUBE_NUMBER*R3; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_M:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*7+ENTER_UNIT+GAP; x < CUBE_NUMBER*8+ENTER_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*2+GAP; y < CUBE_NUMBER*R3; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*7+ENTER_UNIT+GAP; x < CUBE_NUMBER*8+ENTER_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*2+GAP; y < CUBE_NUMBER*R3; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_COMM:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*8+ENTER_UNIT+GAP; x < CUBE_NUMBER*9+ENTER_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*2+GAP; y < CUBE_NUMBER*R3; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*8+ENTER_UNIT+GAP; x < CUBE_NUMBER*9+ENTER_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*2+GAP; y < CUBE_NUMBER*R3; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_DOT:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*9+ENTER_UNIT+GAP; x < CUBE_NUMBER*10+ENTER_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*2+GAP; y < CUBE_NUMBER*R3; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*9+ENTER_UNIT+GAP; x < CUBE_NUMBER*10+ENTER_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*2+GAP; y < CUBE_NUMBER*R3; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_RSFT:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*10+ENTER_UNIT+GAP; x < CUBE_NUMBER*13+CAPS_UNIT+ENTER_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*2+GAP; y < CUBE_NUMBER*R3; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*10+ENTER_UNIT+GAP; x < CUBE_NUMBER*13+CAPS_UNIT+ENTER_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*2+GAP; y < CUBE_NUMBER*R3; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_LCTL:
                if (record->event.pressed) {
                    for (uint8_t x = 0; x < CUBE_NUMBER+RSHIFT_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*3+GAP; y < CUBE_NUMBER*R4; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = 0; x < CUBE_NUMBER+RSHIFT_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*3+GAP; y < CUBE_NUMBER*R4; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_LGUI:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER+RSHIFT_UNIT+GAP; x < CUBE_NUMBER*2+RSHIFT_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*3+GAP; y < CUBE_NUMBER*R4; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER+RSHIFT_UNIT+GAP; x < CUBE_NUMBER*2+RSHIFT_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*3+GAP; y < CUBE_NUMBER*R4; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_LALT:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*2+RSHIFT_UNIT+GAP; x < CUBE_NUMBER*3+RSHIFT_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*3+GAP; y < CUBE_NUMBER*R4; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*2+RSHIFT_UNIT+GAP; x < CUBE_NUMBER*3+RSHIFT_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*3+GAP; y < CUBE_NUMBER*R4; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_SPC:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*3+RSHIFT_UNIT+GAP; x < CUBE_NUMBER*4+RSHIFT_UNIT+SPACE_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*3+GAP; y < CUBE_NUMBER*R4; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*3+RSHIFT_UNIT+GAP; x < CUBE_NUMBER*4+RSHIFT_UNIT+SPACE_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*3+GAP; y < CUBE_NUMBER*R4; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                case KC_RALT:
                if (record->event.pressed) {
                    for (uint8_t x = CUBE_NUMBER*4+RSHIFT_UNIT+GAP+SPACE_UNIT; x < CUBE_NUMBER*5.25+RSHIFT_UNIT+SPACE_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*3+GAP; y < CUBE_NUMBER*R4; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*4+RSHIFT_UNIT+GAP+SPACE_UNIT; x < CUBE_NUMBER*5.25+RSHIFT_UNIT+SPACE_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*3+GAP; y < CUBE_NUMBER*R4; y++) {
                            oled_write_pixel(x,y,false);
                        }
                    }
                }
                    return true;
                default:
                    return true;
            }
        }
   
    return true;
}

    void oled_task_user(void) {

        if ( IS_HOST_LED_OFF(USB_LED_NUM_LOCK) && IS_HOST_LED_OFF(USB_LED_CAPS_LOCK) && selected_layer == 0 && get_highest_layer(layer_state) == 0 ) {
            render_anim();
            clear_screen = true;
        } else {
            if (clear_screen == true) {
                oled_clear();
                oled_render();
                clear_screen = false;
            }
            switch(selected_layer){
                case 0:
                    if (display_keyboard == true){
                        display_keyboard = false;
                    }
                    break;
                case 1:
                    display_keyboard = true;
                    render_keyboard();
                    break;
                case 2:
                    if (display_keyboard == true){
                        display_keyboard = false;
                    }
                    oled_write_P(PSTR("WIP layer 2"), false); 
                    break;
                case 3:
                    if (display_keyboard == true){
                        display_keyboard = false;
                    }
                    oled_write_P(PSTR("WIP layer 3"), false); 
                    break;
                default:
                    oled_write_P(PSTR("WIP"), false);    // Should never display, here as a catchall
            }
        }
    }
#endif
