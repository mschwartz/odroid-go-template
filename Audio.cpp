#include <odroid_go.h>

static volatile struct Sound {
  uint16_t rate;
  uint8_t *start;
  uint8_t *current;
  uint8_t *end;
  bool loop;
} currentSound;

#include "Audio.h"

static uint16_t volume = 8;
static hw_timer_t *timer = NULL;
static portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

static void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  if (currentSound.current) {
    if (currentSound.current >= currentSound.end) {
      if (currentSound.loop) {
        currentSound.current = currentSound.start;
      }
      else {
        currentSound.current = NULL;
        portEXIT_CRITICAL_ISR(&timerMux);
        return;
      }
    }
    dacWrite(SPEAKER_PIN, *currentSound.current++ / volume);
  }
  portEXIT_CRITICAL_ISR(&timerMux);
}

Audio::Audio() {
  currentSound.current = NULL;
  GO.Speaker.setVolume(8);
  // set up hardware timer for 10KHz sample rate
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000 / 10000, true); // 10KHz
  timerAlarmEnable(timer);
}

Audio::~Audio() {}

void Audio::playSound(uint8_t *data, uint16_t rate, bool loop) {
  portENTER_CRITICAL_ISR(&timerMux);
  currentSound.current = currentSound.start = data;
  currentSound.end = &data[sizeof(data)];
  currentSound.loop = loop;
  currentSound.rate = rate;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void Audio::stopSound() {
  portENTER_CRITICAL_ISR(&timerMux);
  currentSound.current = currentSound.start = NULL;
  portEXIT_CRITICAL_ISR(&timerMux);
}

Audio audio;
