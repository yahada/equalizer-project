#include <iostream>
#include <cstdint>
#include <fstream>

namespace equalizer
{
  struct WavHeader
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
  };


  void loadWavHeader(const std::string& filename, WavHeader& header)
  {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
      std::cerr << "open file problem\n";
      throw;
    }
    file.read(reinterpret_cast<char*>(&header), sizeof(WavHeader));

  }

  void showInfo(WavHeader& header)
  {
    std::cout << "ChunkID: " << header.chunkID_ << '\n';
    std::cout << "ChunkSize: " << header.chunkSize_ << '\n';
    std::cout << "Format: " << header.format_ << '\n';

    std::cout << "Subchunk1ID: " << header.subchunk1ID_ << '\n';
    std::cout << "Subchunk1Size: " << header.subchunkSize_ << '\n';
    std::cout << "AudioFormat: " << header.audioFormat_ << '\n';
    std::cout << "NumChannels: " << header.numChannels_ << '\n';
    std::cout << "SampleRate: " << header.sampleRate_ << '\n';
    std::cout << "ByteRate: " << header.byteRate_ << '\n';
    std::cout << "BlockAlign: " << header.blockAlign_ << '\n';
    std::cout << "BitsPerSample: " << header.bitsPerSample_ << '\n';

    std::cout << "Subchunk2ID: " << header.subchunk2ID_ << '\n';
    std::cout << "Subchunk2Size: " << header.subchunk2Size_ << '\n';
  }

}


int main()
{
  equalizer::WavHeader header;
  try
  {
    equalizer::loadWavHeader("file_example_WAV_2MG.wav", header);
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
    return 1;
  }

  equalizer::showInfo(header);
}
