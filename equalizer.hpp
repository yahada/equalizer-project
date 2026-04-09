#ifndef EQUALIZER
#define EQUALIZER
#include "wav_header.hpp"
#include "filter.hpp"
namespace equalizer
{
  class Equalizer {
  public:
    void openFile(const std::string&);
    void saveFile(const std::string&);
    void renameFile(const std::string&, const std::string&);
    void showInfoAboutFile(std::ostream&) const;

    void cutFromLeft(const float& cutSize);
    void cutFromRight(const float& cutSize);
    void changeMuteStatus();
    void increaseVolume(const float&);
    void decreaseVolume(const float&);
    void StereoToMono();
    void reverse();
    void inversion();
    void play();
  private:
    WavHeader header_;
    std::vector< int16_t > audioData_;
    bool isMuted_ = false;
    float leftCut_ = 0.0f;
    float RightCut_ = 0.0f;

    float gainLow_ = 1.0f;
    float gainMid_ = 1.0f;
    float gainHigh_ = 1.0f;
    std::vector< float > convert();
    float countAlpha(const float& cutoff) const noexcept;

    //...
  };
}


#endif