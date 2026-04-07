#include "wav_header.hpp"
#include "filter.hpp"

namespace equalizer {
  std::vector< int16_t > fromStereoToMono(std::vector< int16_t > stereoData)
  {
    std::vector< int16_t > monoData(stereoData.size() / 2);
    for (size_t i = 0; i < stereoData.size(); i += 2)
    {
      int16_t left = stereoData[i];
      int16_t right = stereoData[i + 1];
      int32_t sum = static_cast<int32_t>(left) + right;
      monoData[i / 2] =  sum / 2;
    }
    return monoData;
  }
}


int main()
{
  equalizer::WavHeader header;
  std::vector< int16_t > audioData;
  try
  {
    header.readWavFile("file_example_WAV_2MG.wav", audioData);
    std::vector< int16_t > monoData = equalizer::fromStereoToMono(audioData);
    header.numChannels_ = 1;
    uint32_t tmpSize = header.subchunk2Size_ / 2;
    header.chunkSize_ -= tmpSize;
    header.subchunk2Size_ /= 2;
    header.blockAlign_ = 2;
    header.byteRate_ /= 2;
    header.showInfo(std::cout);
  }
  catch(const std::exception& e)
  {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }
}
