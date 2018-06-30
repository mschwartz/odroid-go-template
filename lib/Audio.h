#ifndef AUDIO_H
#define AUDIO_H

/**
 * Audio class.
 *
 * There is one global singleton instance named audio.
 *
 * This class provides support for playing 8-bit audio samples.
 */
class Audio {
public:
  Audio();
  ~Audio();

public:
  /*
   * set volume - valid values are 0-11 (0 is off)
   */
  void setVolume(uint16_t vol);
  /*
   * Play a sound.
   *
   * If priority is higher than currently playing sound, that sound is stopped
   * and this one played. data is sample data, it's length is deterined using
   * sizeof, so it must be an array. rate is the sample rate in Hz if loop is
   * true, the sound will restart when it is done playing.
   */
  void playSound(uint8 priority, uint8_t *data, uint16_t rate, bool loop);
  void stopSound();
};

extern Audio audio;

#endif
