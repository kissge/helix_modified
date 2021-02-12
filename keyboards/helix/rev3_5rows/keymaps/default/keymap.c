/* Copyright 2020 yushakobo
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


// Defines names for use in layer keycodes and the keymap
enum layer_names {
  _QWERTY = 0,
  _LOWER,
  _RAISE,
  _ADJUST
};

// Defines the keycodes used by our macros in process_record_user
enum custom_keycodes {
  IME = SAFE_RANGE,
  NEXT,
  PREV,
  TCLOSE,
  REOPEN,
  FORWARD,
  BACK,
  WCLOSE,
  NEWTAB,
  CHGMODE,
};

#define LOWER MO(_LOWER)
#define RAISE MO(_RAISE)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_QWERTY] = LAYOUT( \
      KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                      KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, \
      KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                      KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSPC, \
      KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                      KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_ENT, \
      KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_LBRC, KC_RBRC, KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_QUOT, \
      KC_ESC,  IME,     KC_LGUI, KC_LALT, LOWER,   KC_SPC,  KC_SPC,  KC_SPC,  KC_SPC,  RAISE,   KC_EQL,  KC_LBRC, KC_RBRC, KC_BSLS \
    ),

  [_LOWER] = LAYOUT( \
      _______, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______, \
      KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                      KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    _______, \
      _______, NEWTAB,  TCLOSE,  PREV,    NEXT,    WCLOSE,                    KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   \
      _______, REOPEN,  _______, KC_MUTE, KC_VOLD, KC_VOLU, _______, _______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  \
      RESET,   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______  \
      ),

  [_RAISE] = LAYOUT( \
      _______, KC_EXLM, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______, \
      KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                   KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_DEL,  \
      _______, _______, _______, _______, _______, _______,                   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, BACK,    FORWARD, \
      _______, _______, _______, _______, _______, _______, _______, _______, KC_HOME, KC_PGDN, KC_PGUP, KC_END,  _______, KC_PSCR, \
      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______  \
      ),

  [_ADJUST] =  LAYOUT( \
      KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,                     KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12, \
      _______, RESET,   _______, EEP_RST, _______, _______,                   _______, _______, _______, _______, _______, KC_DEL, \
      _______, _______, _______, _______, _______, CHGMODE,                   CHGMODE, _______, _______, _______, _______, _______, \
      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, RGB_TOG, RGB_HUI, RGB_SAI, RGB_VAI, \
      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, RGB_MOD, RGB_HUD, RGB_SAD, RGB_VAD \
      )

};

void encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) { /* Left side encoder */
        if (clockwise) {
            tap_code(KC_PGDN);
        } else {
            tap_code(KC_PGUP);
        }
    } else if (index == 1) { /* Right side encoder */
        if (clockwise) {
            tap_code(KC_DOWN);
        } else {
            tap_code(KC_UP);
        }
    }
}

layer_state_t layer_state_set_user(layer_state_t state) {
  return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case IME:
      if (record->event.pressed) {
        if (keymap_config.kissge_mac_or_win) {
          SEND_STRING(SS_LCTRL(" "));
        } else {
          SEND_STRING(SS_LALT("`"));
        }
      }
      return false;

    case NEXT:
      if (record->event.pressed) {
        if (keymap_config.kissge_mac_or_win) {
          SEND_STRING(SS_LCTRL("\t"));
        } else {
          SEND_STRING(SS_LCTRL(SS_TAP(X_PGDOWN)));
        }
      }
      return false;

    case PREV:
      if (record->event.pressed) {
        if (keymap_config.kissge_mac_or_win) {
          register_code(KC_LCTL);
          register_code(KC_LSFT);
          register_code(KC_TAB);
          unregister_code(KC_TAB);
          unregister_code(KC_LSFT);
          unregister_code(KC_LCTL);
        } else {
          SEND_STRING(SS_LCTRL(SS_TAP(X_PGUP)));
        }
      }
      return false;

    case TCLOSE:
      if (record->event.pressed) {
        if (keymap_config.kissge_mac_or_win) {
          SEND_STRING(SS_LGUI("w"));
        } else {
          SEND_STRING(SS_LCTRL("w"));
        }
      }
      return false;

    case REOPEN:
      if (record->event.pressed) {
        if (keymap_config.kissge_mac_or_win) {
          SEND_STRING(SS_LGUI("T"));
        } else {
          SEND_STRING(SS_LCTRL("T"));
        }
      }
      return false;

    case FORWARD:
      if (record->event.pressed) {
        if (keymap_config.kissge_mac_or_win) {
          SEND_STRING(SS_LGUI(SS_TAP(X_RIGHT)));
        } else {
          SEND_STRING(SS_LALT(SS_TAP(X_RIGHT)));
        }
      }
      return false;

    case BACK:
      if (record->event.pressed) {
        if (keymap_config.kissge_mac_or_win) {
          SEND_STRING(SS_LGUI(SS_TAP(X_LEFT)));
        } else {
          SEND_STRING(SS_LALT(SS_TAP(X_LEFT)));
        }
      }
      return false;

    case WCLOSE:
      if (record->event.pressed) {
        if (keymap_config.kissge_mac_or_win) {
          SEND_STRING(SS_LGUI("W"));
        } else {
          SEND_STRING(SS_LALT(SS_TAP(X_F4)));
        }
      }
      return false;

    case NEWTAB:
      if (record->event.pressed) {
        if (keymap_config.kissge_mac_or_win) {
          SEND_STRING(SS_LGUI("t"));
        } else {
          SEND_STRING(SS_LCTRL("t"));
        }
      }
      return false;

    case CHGMODE:
      if (record->event.pressed) {
        keymap_config.kissge_mac_or_win = !keymap_config.kissge_mac_or_win;
        eeconfig_update_keymap(keymap_config.raw);
      }
      return false;
  }
  return true;
}

/*
void matrix_init_user(void) {

}

void matrix_scan_user(void) {

}

bool led_update_user(led_t led_state) {
    return true;
}
*/
