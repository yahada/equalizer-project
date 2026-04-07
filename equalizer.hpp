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
    void showInfoAboutFile();

    void cutFromLeft(const float& cutSize);
    void cutFromRight(const float& cutSize);
    void muteAudio();
    void increaseVolume(float);
    void decreaseVolume(float);
    void StereoToMono();
    void inversion();
    void play();
  private:
    WavHeader header_;
    std::vector< int16_t > audioData_;
    float leftCut_ = 0.0f;
    float RightCut_ = 0.0f;

    float gainLow_ = 1.0f;
    float gainMid_ = 1.0f;
    float gainHigh_ = 1.0f;

    //...
  };
}


#endif