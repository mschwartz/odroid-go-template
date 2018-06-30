#include <odroid_go.h>

/**
 * This is the frequency of the timer interrupt
 */
static const int TIMER_FREQUENCY = 10000; // 10KHz

/**
 * Master volume
 * Each byte of the sample being played is scaled by this.
 */
static uint16_t volume = 8;

/**
 * This struct/varable tracks the currently playing sound.
 * If no sound is playing, then start member is NULL.
 */
static volatile struct Sound {
  uint16_t rate;
  float index, step;
  uint8_t *start;
  uint8_t *current;
  uint8_t *end;
  bool loop;
  uint8_t priority;
} currentSound;

#include "Audio.h"

/**
 * Handle for hardware timer.
 */
static hw_timer_t *timer = NULL;

/**
 * We have critical secions of code when playing sample in the ISR
 * and when we want to start a new sound.
 *
 * So we use this semaphore
 */
static portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

/**
 * The timer interrupt routine.
 */
static void IRAM_ATTR timerISR() {
  portENTER_CRITICAL_ISR(&timerMux);

  // critical section
  if (currentSound.start && volume && volume != 11) {
    // there is a sound playing!
    if (currentSound.current >= currentSound.end) {
      // sound done playing?
      if (currentSound.loop) {
        // start at the beginning
        currentSound.current = currentSound.start;
        currentSound.index = 0;
      }
      else {
        // terminate sound
        currentSound.start = NULL;
        currentSound.pri = 0; // assure next playSound will play a sound.
        portEXIT_CRITICAL_ISR(&timerMux);
        return;
      }
    }
    // write sample scaled by volume to the DAC
    dacWrite(SPEAKER_PIN, *currentSound.current / volume);
    // step (fractionally!) through the sample
    // if sample rate is 20KHz, then step would be 2,
    // causing every other sample to be skipped.
    currentSound.index += currentSound.step;
    curentSound.current = &currentSound.start[currentSound.index];
  }

  // end critical section
  portEXIT_CRITICAL_ISR(&timerMux);
}

/**
 * Audio class constructor
 */
Audio::Audio() {
  currentSound.current = NULL;
  GO.Speaker.setVolume(8);
  // set up hardware timer for 10KHz sample rate
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &timerISR, true);
  timerAlarmWrite(timer, 1000000 / SAMPLE_RATE, true); // 10KHz
  timerAlarmEnable(timer);
}

/**
 * Destructor
 */
Audio::~Audio() {
  // TODO: kill / remove timer
}

/**
 * set audio volume (0-11)
 */
void Audio::setVolume(uint16_t vol) {
  if (vol < 0) {
    vol = 0;
  }
  else if (vol > 11) {
    vol = 11;
  }
  volume = vol;
}

/**
 * Play a sound.
 *
 * data is expected to be an array of bytes.
 * length of data is determined using sizeof operator (for now)
 * rate is the sample rate for the sound, in Hz
 * if loop is true, then the sound will restart when it is done playing.
 */
void Audio::playSound(uint8_t pri, uint8_t data[], uint16_t rate, bool loop) {
  portENTER_CRITICAL_ISR(&timerMux);
  if (pri >= currentSound.priority) {
    currentSound.priority = pri;
    currentSound.current = currentSound.start = data;
    currentSound.end = &data[sizeof(data)];
    currentSound.loop = loop;
    currentSound.rate = rate;
    currentSound.index = 0;
    currentSound.step = rate / SAMPLE_RATE;
  }
  portEXIT_CRITICAL_ISR(&timerMux);
}

/**
 * Stop all sounds
 */
void Audio::stopSound() {
  portENTER_CRITICAL_ISR(&timerMux);

  currentSound.current = currentSound.start = NULL;
  currentSound.priority = 0;

  portEXIT_CRITICAL_ISR(&timerMux);
}

/**
 * one global singletone audio instance
 */
Audio audio;
