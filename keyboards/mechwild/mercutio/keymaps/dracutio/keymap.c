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
#include "raw_hid.h"
#include "header.h"
#include <string.h>

char wpm_str[10];

/* Matrix display is 19 x 9 pixels */
#define MATRIX_DISPLAY_X 51.5
#define MATRIX_DISPLAY_Y 15

//unit
#define GAP 2
#define CUBE_NUMBER 3.75
#define SPACE_UNIT 25
#define TAB_UNIT 1.25
#define CAPS_UNIT 1.5
#define ENTER_UNIT 2
#define RSHIFT_UNIT 1.75

//row
#define R1 1
#define R2 2
#define R3 3
#define R4 4

bool display_keyboard = false;
bool first_step = false;
bool second_step = false;
bool third_step = false;

uint8_t selected_layer = 0;

bool is_hid_connected = false;
char hid_info_str[20];
uint8_t screen_max_count = 0; 
uint8_t screen_data_buffer[SERIAL_SCREEN_BUFFER_LENGTH - 1] =  {0}; 
uint8_t volatile serial_slave_screen_buffer[SERIAL_SCREEN_BUFFER_LENGTH] = {0};


void update_screen_index(void) {
  // Send the current info screen index to the connected node script so that it can pass back the new data

  uint8_t screen_index[32] = {0};
  screen_index[0] = selected_layer + 1; // Add one so that we can distinguish it from a null byte
  raw_hid_send(screen_index, sizeof(screen_index));

}

void raw_hid_receive(uint8_t *data, uint8_t length) {
  //Check for first time
  if (length > 1 && data[0] == 1) {
    //PC connected, so set the flag to show a message on the master display
    is_hid_connected = true;
    // Tell the connection which info screen we want to look at initially
    update_screen_index();
    return;
  }

    // Otherwise the data we receive is one line of the screen to show on the display
      
    memcpy((char*)&screen_data_buffer, data, 32);

    // Now get ready to transfer the whole 4 lines to the slave side of the keyboard.
    // First clear the transfer buffer with spaces just in case.
    memset((char*)&serial_slave_screen_buffer[0], ' ', sizeof(serial_slave_screen_buffer));

    // Copy in the 4 lines of screen data, but start at index 1, we use index 0 to indicate a connection in the slave code
    memcpy((char*)&serial_slave_screen_buffer[1], screen_data_buffer, sizeof(screen_data_buffer));

    // Set index 0 to indicate a connection has been established
    serial_slave_screen_buffer[0] = 1;

    // Make sure to zero terminate the buffer
    serial_slave_screen_buffer[sizeof(serial_slave_screen_buffer) - 1] = 0;
}

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
    KC_LCTL, KC_LGUI, KC_LALT,          KC_SPC,  KC_SPC,           KC_SPC,           KC_RALT, MO(2),            MO(1) ),

  [2] = LAYOUT_all(
                                                                                                                KC_TRNS,
    KC_TRNS,          KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_TRNS,
    KC_CAPS,          KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_F11, KC_F12, KC_TRNS,
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS,
    KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS, KC_TRNS,          KC_TRNS,          KC_TRNS, KC_TRNS,          KC_TRNS ),

  [3] = LAYOUT_all(
                                                                                                                KC_TRNS,
    KC_TRNS,          KC_1,   KC_2,   KC_3,   KC_4,   KC_5,   KC_6,   KC_7,   KC_8,   KC_9,   KC_0,  KC_TRNS,
    KC_CAPS,          KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS,
    KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS, KC_TRNS,          KC_TRNS,          KC_TRNS, KC_TRNS,          KC_TRNS )
};

#ifdef ENCODER_ENABLE       // Encoder Functionality
    bool encoder_update_user(uint8_t index, bool clockwise) {
        #ifdef OLED_ENABLE
            oled_clear();
            oled_render();
        #endif
        switch (index) {
            case 0:         // This is the only encoder right now, keeping for consistency
                if (clockwise) {
                     if ( selected_layer  < 3 && keyboard_report->mods & MOD_BIT(KC_RSFT) ) { // If you are holding L shift, encoder changes layers
                        selected_layer ++;
                        layer_move(selected_layer);
                    } else {
                        tap_code(KC_VOLU);   // Otherwise it just changes volume
                    }
                } else {
                    if ( selected_layer  > 0 && keyboard_report->mods & MOD_BIT(KC_RSFT) ){
                        selected_layer --;
                        layer_move(selected_layer);
                    } else {
                        tap_code(KC_VOLD);
                    }
                }

                if (is_hid_connected) {
                    update_screen_index();
                }
                break;
            default:
                if (clockwise) {
                    tap_code(KC_VOLU);
                } else {
                    tap_code(KC_VOLD);
                }
                break;
        }
    return true;
    }
#endif

#ifdef OLED_ENABLE   // OLED Functionality
    oled_rotation_t oled_init_user(oled_rotation_t rotation) {
        return OLED_ROTATION_180;       // flips the display 180 degrees if offhand
    }


    static void render_stats(void){
        oled_set_cursor(0,0);
        oled_write(serial_slave_screen_buffer[0] > 0 ? (char*)serial_slave_screen_buffer+1 : PSTR(""), false);
    }

    static void render_hid(void){
        oled_set_cursor(16,3);
        oled_write_P(is_hid_connected == true ? PSTR("HID") : PSTR(""), false);
    }

    static void render_keyboard(void){


        oled_set_cursor(2, 3);
        oled_write_P(PSTR("MATRIX\n"), false);

        oled_set_cursor(11, 0);
        sprintf(wpm_str, "wpm: %03d", get_current_wpm());
        oled_write(wpm_str, false);

        oled_set_cursor(0, 0);
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
                case KC_MUTE:
                    
                    if (record->event.pressed) {
                        set_current_wpm(0);
                    }
                    if (selected_layer == 1){
                        return false;
                    } else {
                        return true;
                    }
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
                    for (uint8_t x = CUBE_NUMBER*10+CAPS_UNIT+GAP; x < CUBE_NUMBER*12.5+CAPS_UNIT+ENTER_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER+GAP; y < CUBE_NUMBER*R2; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*10+CAPS_UNIT+GAP; x < CUBE_NUMBER*12.5+CAPS_UNIT+ENTER_UNIT; x++) {
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
                    for (uint8_t x = CUBE_NUMBER*10+ENTER_UNIT+GAP; x < CUBE_NUMBER*12.5+CAPS_UNIT+ENTER_UNIT; x++) {
                        for (uint8_t y = CUBE_NUMBER*2+GAP; y < CUBE_NUMBER*R3; y++) {
                            oled_write_pixel(x,y,true);
                        }
                    }
                } else {
                    for (uint8_t x = CUBE_NUMBER*10+ENTER_UNIT+GAP; x < CUBE_NUMBER*12.5+CAPS_UNIT+ENTER_UNIT; x++) {
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
            switch(selected_layer){
                case 0:
                    if (display_keyboard == true){
                        display_keyboard = false;
                    }
                    oled_set_cursor(0, 0);
                    oled_write_P(PSTR("Dracutio 1.0\n"), false);
                    oled_set_cursor(2, 3);
                    oled_write_P(PSTR("MAIN\n"), false);
                    render_hid();
                    break;
                case 1:
                    render_keyboard();
                    render_hid();
                    display_keyboard = true;
                    break;
                case 2:
                    if (display_keyboard == true){
                        display_keyboard = false;
                    }
                        render_stats();
                        oled_set_cursor(2, 3);
                        oled_write_P(PSTR("FN\n"), false); 
                    break;
                case 3:
                    if (display_keyboard == true){
                        display_keyboard = false;
                    }
                        render_stats();
                        oled_set_cursor(2, 3);
                        oled_write_P(PSTR("NUM\n"), false); 
                    break;
                default:
                    oled_write_P(PSTR(""), false);    // Should never display, here as a catchall
            }

        led_t led_state = host_keyboard_led_state();
        oled_set_cursor(15,2);
        oled_write_P(led_state.caps_lock && !is_hid_connected ? PSTR("CAPS") : PSTR("     "), false);
    }
#endif