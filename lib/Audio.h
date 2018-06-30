#ifndef AUDIO_H
#define AUDIO_H

class Audio {
public:
  Audio();
  ~Audio();

public:
  void setVolume(uint16_t vol);
  void playSound(uint8_t *data, uint16_t rate, bool loop);
  void stopSound();
};

extern Audio audio;

#endif
