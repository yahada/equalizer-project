#ifndef WAV_HEADER
#define WAV_HEADER
#include <cstdint>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
namespace equalizer
{
  class Filter;
  class Equalizer;
  class WavHeader
  {
  public:
    void readWavFile(const std::string& filename, std::vector< int16_t >& audioData);
    void saveWav(const std::string& filename, const std::vector< int16_t >& audioData);
    void showInfo(std::ostream& out) const noexcept;
  private:

    friend class Filter;
    friend class Equalizer;
    char chunkID_[4];
    uint32_t chunkSize_;
    char format_[4];

    char subchunk1ID_[4];
    uint32_t subchunkSize_;
    uint16_t audioFormat_;
    uint16_t numChannels_;
    uint32_t sampleRate_;
    uint32_t byteRate_;
    uint16_t blockAlign_;
    uint16_t bitsPerSample_;

    char subchunk2ID_[4];
    uint32_t subchunk2Size_;

    bool checkCorrectnessOfHeader(std::string& errorMsg);
    float countAlpha(float cutoff);
  };
}

#endif