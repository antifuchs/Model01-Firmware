// -*- mode: c++ -*-
// Copyright 2016 Keyboardio, inc. <jesse@keyboard.io>
// See "LICENSE" for license details

#define DEBUG_SERIAL false

#ifndef BUILD_INFORMATION
#define BUILD_INFORMATION "locally built"
#endif


#include "Kaleidoscope-MouseKeys.h"
#include "Kaleidoscope-Macros.h"
#include "Kaleidoscope-LEDControl.h"
#include "Kaleidoscope-Numlock.h"
#include "Kaleidoscope-OneShot.h"
#include "Kaleidoscope.h"

#include "LED-Off.h"
#include "Kaleidoscope-LEDEffect-BootGreeting.h"
#include "Kaleidoscope-LEDEffect-SolidColor.h"
#include "Kaleidoscope-LED-AlphaSquare.h"

// Support for host power management (suspend & wakeup)
#include "Kaleidoscope-HostPowerManagement.h"

// Keymaps:
enum {QWERTY, FUNCTION};

// Macros:
enum { MACRO_ALLMODS, MACRO_SEXP};

#define LHYPER(key) (LALT(LGUI(LCTRL((key)))))

// *INDENT-OFF*
// To align these blocks, mark each one (one side at a time) and use C-u M-x align RET.
KEYMAPS(
  [QWERTY] = KEYMAP_STACKED
    (M(MACRO_ALLMODS), Key_1,         Key_2,       Key_3, Key_4, Key_5, M(MACRO_SEXP),
     Key_Backtick,     Key_Q,         Key_W,       Key_E, Key_R, Key_T, Key_Tab,
     Key_Escape,       Key_A,         Key_S,       Key_D, Key_F, Key_G,
     Key_LeftShift,    Key_Z,         Key_X,       Key_C, Key_V, Key_B, Key_LeftAlt,
     Key_LeftShift,    Key_Backspace, Key_LeftGui, Key_LeftControl,
     ShiftToLayer(FUNCTION),

     M(MACRO_ALLMODS), Key_6,        Key_7,        Key_8,     Key_9,      Key_0,         Key_Minus,
     Key_Enter,        Key_Y,        Key_U,        Key_I,     Key_O,      Key_P,         Key_Equals
     /* long key */,   Key_H,        Key_J,        Key_K,     Key_L,      Key_Semicolon, Key_Quote,
     Key_RightAlt,     Key_N,        Key_M,        Key_Comma, Key_Period, Key_Slash,     Key_RightShift,
     Key_RightControl, Key_RightGui, Key_Spacebar, Key_RightShift,
     ShiftToLayer(FUNCTION)),

  [FUNCTION] = KEYMAP_STACKED
    (___,           Key_F1,        Key_F2,            Key_F3,        Key_F4,        Key_F5,        ___,
     Key_Tab,       XXX,           Key_mouseScrollUp, LHYPER(Key_E), LHYPER(Key_R), LHYPER(Key_T), XXX,
     Key_mouseBtnR, Key_mouseBtnL, Key_mouseScrollDn, LHYPER(Key_D), LHYPER(Key_F), LHYPER(Key_G),
     Key_LeftShift, Key_Home,      Key_End,           LHYPER(Key_C), LHYPER(Key_V), LHYPER(Key_B), Key_LeftAlt,
     Key_LeftShift, Key_Backspace, Key_LeftGui,       Key_LeftControl,
     ___,

     ___,              Key_F6,                  Key_F7,                   Key_F8,                   Key_F9,          Key_F10,                    Key_F11,
     XXX,              XXX,                     Key_LeftCurlyBracket,     Key_RightCurlyBracket,    Key_LeftBracket, Key_RightBracket,           Key_Pipe
     /* long key */,   Key_LeftArrow,           Key_DownArrow,            Key_UpArrow,              Key_RightArrow,  Consumer_ScanPreviousTrack, Consumer_ScanNextTrack,
     Key_RightAlt,     Consumer_PlaySlashPause, Consumer_VolumeDecrement, Consumer_VolumeIncrement, XXX,             Key_Backslash,              Key_RightShift,
     Key_RightControl, Key_RightGui,            Key_Spacebar,             Key_RightShift,
     ___
       )
      )
// *INDENT-ON*

static kaleidoscope::LEDSolidColor solidRed(160, 0, 0);
static kaleidoscope::LEDSolidColor solidOrange(140, 70, 0);
static kaleidoscope::LEDSolidColor solidYellow(130, 100, 0);
static kaleidoscope::LEDSolidColor solidGreen(0, 160, 0);
static kaleidoscope::LEDSolidColor solidBlue(0, 70, 130);
static kaleidoscope::LEDSolidColor solidIndigo(0, 0, 170);
static kaleidoscope::LEDSolidColor solidViolet(130, 0, 120);

/** toggleLedsOnSuspendResume toggles the LEDs off when the host goes to sleep,
 * and turns them back on when it wakes up.
 */
void toggleLedsOnSuspendResume(kaleidoscope::HostPowerManagement::Event event) {
  switch (event) {
  case kaleidoscope::HostPowerManagement::Suspend:
    LEDControl.paused = true;
    LEDControl.set_all_leds_to({0, 0, 0});
    LEDControl.syncLeds();
    break;
  case kaleidoscope::HostPowerManagement::Resume:
    LEDControl.paused = false;
    LEDControl.refreshAll();
    break;
  case kaleidoscope::HostPowerManagement::Sleep:
    break;
  }
}

void hostPowerManagementEventHandler(kaleidoscope::HostPowerManagement::Event event) {
  toggleLedsOnSuspendResume(event);
}

static void OneShotSexp(uint8_t keyState) {
  handleKeyswitchEvent(OSM(LeftControl), UNKNOWN_KEYSWITCH_LOCATION, keyState);
  handleKeyswitchEvent(OSM(LeftAlt), UNKNOWN_KEYSWITCH_LOCATION, keyState);
}

static void OneShotHyper(uint8_t keyState) {
  handleKeyswitchEvent(OSM(LeftControl), UNKNOWN_KEYSWITCH_LOCATION, keyState);
  handleKeyswitchEvent(OSM(LeftAlt), UNKNOWN_KEYSWITCH_LOCATION, keyState);
  handleKeyswitchEvent(OSM(LeftGui), UNKNOWN_KEYSWITCH_LOCATION, keyState);
}

const macro_t *macroAction(uint8_t macroIndex, uint8_t keyState) {
  switch(macroIndex) {
  case MACRO_ALLMODS:
    OneShotHyper(keyState);
    return MACRO_NONE;
  case MACRO_SEXP:
    OneShotSexp(keyState);
    return MACRO_NONE;
  }
  return MACRO_NONE;
}

void setup() {
  Kaleidoscope.setup();
  Kaleidoscope.use(&LEDControl,
                   &LEDOff,
                   &OneShot,

                   &Macros,
                   &MouseKeys,
                   &HostPowerManagement,
                   NULL);

  AlphaSquare.color = { 255, 0, 0 };
  HostPowerManagement.enableWakeup();
  LEDOff.activate();
}


void loop() {
  Kaleidoscope.loop();
}
