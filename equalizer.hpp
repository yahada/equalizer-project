#ifndef EQUALIZER
#define EQUALIZER
#include "wav_header.hpp"

namespace equalizer
{
  class Equalizer {
  public:
    void openFile(const std::string& filename);
    void saveFile(const std::string& filename);
    void renameFile(const std::string& oldName, const std::string& newName);
    void spliceIntoFrequencies();
    void cutFromLeft(const float& cutSize);
    void cutFromRight(const float& cutSize);
    void muteAudio();
    void increaseVolume(float);
    void decreaseVolume(float);
    void StereoToMono();
    void inversion();
    void inversion(float begin, float end);
    void play();
  private:
    WavHeader header_;
    std::vector< int16_t > audioData_;
    float leftCut_;
    float RightCut_;
    std::vector< int16_t > lowFrequencyData_;
    std::vector< int16_t > midFrequencyData_;
    std::vector< int16_t > highFrequencyData_;
    //...
  };
}


#endif