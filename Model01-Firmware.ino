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
#define MACRO_VERSION_INFO 1
#define Macro_VersionInfo M(MACRO_VERSION_INFO)
#define MACRO_ANY 2
#define Macro_Any M(MACRO_ANY)
#define MACRO_LEFTPAREN 3
#define MACRO_RIGHTPAREN 4
#define MACRO_ALLMODS 5
#define Macro_RightParen M(MACRO_RIGHTPAREN)
#define Macro_LeftParen M(MACRO_LEFTPAREN)
#define Macro_AllMods M(MACRO_ALLMODS)
#define NUMPAD_KEYMAP 2

#define GENERIC_FN2  KEYMAP_STACKED ( \
    ___,                  Key_F1,                  Key_F2,                   Key_F3,                   Key_F4,                     Key_F5,                 Macro_AllMods, \
    Key_Tab,              ___,                     Key_mouseScrollUp,        ___,                      Key_mouseBtnR,              XXX,                    XXX, \
    Key_Home,             XXX,                     Key_mouseScrollDn,        XXX,                      Key_mouseBtnL,              XXX, \
    Key_LeftShift,        Key_Home,                Key_End,                  XXX,                      XXX,                        XXX,                    Key_LeftAlt, \
    Key_LeftShift,        Key_Backspace,           Key_LeftGui,              Key_LeftControl, \
    ___,                                        \
                                                \
    Macro_AllMods,        Key_F6,                  Key_F7,                   Key_F8,                   Key_F9,                     Key_F10,                Key_Pipe, \
    Key_LeftCurlyBracket, Key_RightCurlyBracket,   Macro_LeftParen,          Macro_RightParen,         Key_LeftBracket,            Key_RightBracket,       Key_F12, \
    Key_LeftArrow,        Key_DownArrow,           Key_UpArrow,              Key_RightArrow,           Consumer_ScanPreviousTrack, Consumer_ScanNextTrack, \
    Key_RightAlt,         Consumer_PlaySlashPause, Consumer_VolumeDecrement, Consumer_VolumeIncrement, ___,                        Key_Backslash,          Key_RightShift, \
    Key_RightControl,     Key_RightGui,            Key_Spacebar,             Key_RightShift, \
    ___                                         \
)



#define NUMPAD KEYMAP_STACKED  (\
    ___,                   ___,         ___,         ___,           ___,                ___,                ___, \
    ___,                   ___,         ___,         ___,           ___,                ___,                ___, \
    ___,                   ___,         ___,         ___,           ___,                ___, \
    ___,                   ___,         ___,         ___,           ___,                ___,                ___, \
    ___,                   ___,         ___,         ___, \
    Key_Keymap1_Momentary,                      \
                                                \
                                                \
    Key_ToggleNumlock,     ___,         Key_Keypad7, Key_Keypad8,   Key_Keypad9,        Key_KeypadSubtract, ___, \
    ___,                   ___,         Key_Keypad4, Key_Keypad5,   Key_Keypad6,        Key_KeypadAdd,      ___, \
    ___,                   Key_Keypad1, Key_Keypad2, Key_Keypad3,   Key_Equals,         Key_ToggleNumlock, \
    ___,                   ___,         Key_Keypad0, Key_KeypadDot, Key_KeypadMultiply, Key_KeypadDivide,   Key_Enter, \
    ___,                   ___,         ___,         ___, \
    Key_Keymap1_Momentary                       \
)

#define QWERTY KEYMAP_STACKED (                 \
     ___,                      Key_1,         Key_2,        Key_3,           Key_4,         Key_5,     Macro_AllMods, \
     Key_Backtick,             Key_Q,         Key_W,        Key_E,           Key_R,         Key_T,     Key_Tab, \
     Key_Escape,               Key_A,         Key_S,        Key_D,           Key_F,         Key_G, \
     Key_LeftShift,            Key_Z,         Key_X,        Key_C,           Key_V,         Key_B,     Key_LeftAlt, \
     Key_LeftShift,            Key_Backspace, Key_LeftGui,  Key_LeftControl, \
     Key_KeymapNext_Momentary,                  \
                                                \
     Macro_AllMods,            Key_6,         Key_7,        Key_8,           Key_9,         Key_0,     Key_Minus, \
     Key_Enter,                Key_Y,         Key_U,        Key_I,           Key_O,         Key_P,     Key_Equals, \
     Key_H,                    Key_J,         Key_K,        Key_L,           Key_Semicolon, Key_Quote, \
     Key_RightAlt,             Key_N,         Key_M,        Key_Comma,       Key_Period,    Key_Slash, Key_RightShift, \
     Key_RightControl,         Key_RightGui,  Key_Spacebar, Key_RightShift, \
     Key_KeymapNext_Momentary                   \
      )

const Key keymaps[][ROWS][COLS] PROGMEM = {
  QWERTY,
  GENERIC_FN2,
  NUMPAD
};

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

static void OneShotHyper(uint8_t keyState) {
  handleKeyswitchEvent(OSM(LeftShift), UNKNOWN_KEYSWITCH_LOCATION, keyState);
  handleKeyswitchEvent(OSM(LeftControl), UNKNOWN_KEYSWITCH_LOCATION, keyState);
  handleKeyswitchEvent(OSM(LeftAlt), UNKNOWN_KEYSWITCH_LOCATION, keyState);
  handleKeyswitchEvent(OSM(LeftGui), UNKNOWN_KEYSWITCH_LOCATION, keyState);
}

const macro_t *macroAction(uint8_t macroIndex, uint8_t keyState) {
  if (macroIndex == TOGGLENUMLOCK && keyToggledOn(keyState)) {
    return NumLock.toggle();
  } else if (macroIndex == MACRO_LEFTPAREN) {
    return MACRODOWN(I(10), D(LeftShift), T(9), U(LeftShift));
  } else if (macroIndex == MACRO_RIGHTPAREN) {
    return MACRODOWN(I(10), D(LeftShift), T(0), U(LeftShift));
  } else if (macroIndex == MACRO_ALLMODS) {
    OneShotHyper(keyState);
  }
  return MACRO_NONE;
}

void setup() {
  Kaleidoscope.setup(KEYMAP_SIZE);
  BootKeyboard.begin();
  Kaleidoscope.use(&LEDControl,
                   &LEDOff,
                   &NumLock,
                   &OneShot,

                   &Macros,
                   &MouseKeys,
                   &HostPowerManagement,
                   NULL);

  NumLock.numPadLayer = NUMPAD_KEYMAP;
  AlphaSquare.color = { 255, 0, 0 };
  HostPowerManagement.enableWakeup();
  LEDOff.activate();
}


void loop() {
  Kaleidoscope.loop();
}
