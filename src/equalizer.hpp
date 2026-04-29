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
    void resetChanges();
    void cutSegment(float positionRatio, float durationSeconds);
    void trimToRange(float startRatio, float endRatio);
    const std::vector< int16_t >& processedAudioData() const noexcept;
    uint32_t sampleRate() const noexcept;
    uint16_t numChannels() const noexcept;
    float durationSeconds() const noexcept;
    bool getUiStatus() const noexcept;
    void getSettings(std::ostream&) const;
    void loadSettings(bool, float, float, float, float, float);
    void changeDuration(float left, float right);
    void changeMuteStatus() noexcept;
    void changeVolume(const float& lowFreqGain, const float& midFreqGain, const float& highFreqGain);
    void StereoToMono();
    void reverse();
    void inversion();
    void play();
  private:
    friend class cliEqualizer;
    WavHeader header_;
    std::vector< int16_t > audioData_;
    std::vector< int16_t > changedAudioData_;
    bool isMuted_ = false;
    float leftCut_ = 0.0f;
    float rightCut_ = 0.0f;

    float gainLow_ = 1.0f;
    float gainMid_ = 1.0f;
    float gainHigh_ = 1.0f;
    void refreshHeaderSizes() noexcept;
    std::vector< float > convert();
    bool uiStatus = false;
  };
}


#endif
