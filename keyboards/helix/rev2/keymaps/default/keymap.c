#include "helix.h"
#include "bootloader.h"
#include "action_layer.h"
#include "eeconfig.h"
#ifdef PROTOCOL_LUFA
#include "lufa.h"
#include "split_util.h"
#endif
#include "LUFA/Drivers/Peripheral/TWI.h"
#ifdef AUDIO_ENABLE
  #include "audio.h"
#endif
#ifdef SSD1306OLED
  #include "ssd1306.h"
#endif

extern keymap_config_t keymap_config;

#ifdef RGBLIGHT_ENABLE
//Following line allows macro to read current RGB settings
extern rgblight_config_t rgblight_config;
#endif

extern uint8_t is_master;

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skip them
// entirely and just use numbers.
#define _QWERTY 0
#define _RAISE 4

enum custom_keycodes {
  RAISE = SAFE_RANGE,
  IME,
  NEXT,
  PREV,
  TCLOSE,
  REOPEN,
  FORWARD,
  BACK,
  WCLOSE,
  NEWTAB,
};

enum macro_keycodes {
  KC_SAMPLEMACRO,
};


// Fillers to make layering more clear
#define _______ KC_TRNS
#define XXXXXXX KC_NO
//Macros
#define M_SAMPLE M(KC_SAMPLEMACRO)

#if HELIX_ROWS == 5
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  [_QWERTY] = KEYMAP( \
      KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                      KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC, \
      KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                      KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P, \
      KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                      KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, \
      KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_PGUP, KC_GRV,  KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, \
      KC_HOME, KC_END,  KC_LGUI, KC_LALT, KC_SPC,  KC_QUOT, KC_PGDN, KC_ENT,  RAISE,   KC_MINS, KC_EQL,  KC_LBRC, KC_RBRC, KC_BSLS \
      ),

  [_RAISE] = KEYMAP( \
      KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,                     KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12, \
      _______, _______, KC_UP,   BACK,    FORWARD, _______,                   _______, PREV,    NEXT,    KC_9,    KC_PSCR, KC_DEL, \
      _______, KC_LEFT, KC_DOWN, KC_RGHT, _______, _______,                   KC_PGUP, WCLOSE,  TCLOSE,  REOPEN,  NEWTAB,  _______, \
      _______, KC_MUTE, KC_VOLD, KC_VOLU, _______, _______, _______, _______, KC_PGDN, KC_HOME, KC_END,  _______, _______, _______, \
      _______, _______, _______, _______, IME,     _______, _______, _______, _______, _______, _______, _______, _______, _______ \
      )
};
#else
#error "undefined keymaps"
#endif


#ifdef AUDIO_ENABLE

float tone_qwerty[][2]     = SONG(QWERTY_SOUND);
float tone_dvorak[][2]     = SONG(DVORAK_SOUND);
float tone_colemak[][2]    = SONG(COLEMAK_SOUND);
float tone_plover[][2]     = SONG(PLOVER_SOUND);
float tone_plover_gb[][2]  = SONG(PLOVER_GOODBYE_SOUND);
float music_scale[][2]     = SONG(MUSIC_SCALE_SOUND);
#endif

// define variables for reactive RGB
bool TOG_STATUS = false;
int RGB_current_mode;

void persistent_default_layer_set(uint16_t default_layer) {
  eeconfig_update_default_layer(default_layer);
  default_layer_set(default_layer);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case RAISE:
      if (record->event.pressed) {
        layer_on(_RAISE);
      } else {
        layer_off(_RAISE);
      }
      return false;
    case IME:
      if (record->event.pressed) {
        SEND_STRING(SS_LALT("`"));
      }
      return false;
    case NEXT:
      if (record->event.pressed) {
        SEND_STRING(SS_LCTRL(SS_TAP(X_PGDOWN)));
      }
      return false;
    case PREV:
      if (record->event.pressed) {
        SEND_STRING(SS_LCTRL(SS_TAP(X_PGUP)));
      }
      return false;
    case TCLOSE:
      if (record->event.pressed) {
        SEND_STRING(SS_LCTRL("w"));
      }
      return false;
    case REOPEN:
      if (record->event.pressed) {
        SEND_STRING(SS_LCTRL("T"));
      }
      return false;
    case FORWARD:
      if (record->event.pressed) {
        SEND_STRING(SS_LALT(SS_TAP(X_RIGHT)));
      }
      return false;
    case BACK:
      if (record->event.pressed) {
        SEND_STRING(SS_LALT(SS_TAP(X_LEFT)));
      }
      return false;
    case WCLOSE:
      if (record->event.pressed) {
        SEND_STRING(SS_LALT(SS_TAP(X_F4)));
      }
      return false;
    case NEWTAB:
      if (record->event.pressed) {
        SEND_STRING(SS_LCTRL("t"));
      }
      return false;
  }
  return true;
}

void matrix_init_user(void) {
    #ifdef AUDIO_ENABLE
        startup_user();
    #endif
    #ifdef RGBLIGHT_ENABLE
      RGB_current_mode = rgblight_config.mode;
    #endif
    //SSD1306 OLED init, make sure to add #define SSD1306OLED in config.h
    #ifdef SSD1306OLED
        TWI_Init(TWI_BIT_PRESCALE_1, TWI_BITLENGTH_FROM_FREQ(1, 800000));
        iota_gfx_init(!has_usb());   // turns on the display
    #endif
}


#ifdef AUDIO_ENABLE

void startup_user()
{
    _delay_ms(20); // gets rid of tick
}

void shutdown_user()
{
    _delay_ms(150);
    stop_all_notes();
}

void music_on_user(void)
{
    music_scale_user();
}

void music_scale_user(void)
{
    PLAY_SONG(music_scale);
}

#endif


//SSD1306 OLED update loop, make sure to add #define SSD1306OLED in config.h
#ifdef SSD1306OLED

void matrix_scan_user(void) {
     iota_gfx_task();  // this is what updates the display continuously
}

void matrix_update(struct CharacterMatrix *dest,
                          const struct CharacterMatrix *source) {
  if (memcmp(dest->display, source->display, sizeof(dest->display))) {
    memcpy(dest->display, source->display, sizeof(dest->display));
    dest->dirty = true;
  }
}

//assign the right code to your layers for OLED display
#define L_BASE 0
#define L_RAISE 16

static void render_logo(struct CharacterMatrix *matrix) {
  static char logo[] = {
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0
  };
  matrix_write(matrix, logo);
}



void render_rocket(struct CharacterMatrix *matrix) {
  static unsigned tick = 0;
  char logo[85] = {0};

  tick += (layer_state == L_BASE) ? 1 : 4;

  for (int i = 0; i < 84; ++i)
  {
    logo[i] = 0x21 + i + (((tick >> 5) % 2) ? 84 : 0);
  }

  matrix_write(matrix, logo);
}


void iota_gfx_task_user(void) {
  struct CharacterMatrix matrix;

#if DEBUG_TO_SCREEN
  if (debug_enable) {
    return;
  }
#endif

  matrix_clear(&matrix);
  if(is_master){
    render_rocket(&matrix);
  }else{
    render_logo(&matrix);
  }
  matrix_update(&display, &matrix);
}

#endif
