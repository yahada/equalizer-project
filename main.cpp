#include <iostream>
int main()
{
  class WavStruct
  {
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
  }
}
