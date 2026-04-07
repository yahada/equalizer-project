#include "equalizer.hpp"

void equalizer::Equalizer::StereoToMono()
{
  std::vector< int16_t > monoData(audioData_.size() / 2);
  for (size_t i = 0; i < audioData_.size(); i += 2)
  {
    int16_t left = audioData_[i];
    int16_t right = audioData_[i + 1];
    int32_t sum = static_cast<int32_t>(left) + right;
    monoData[i / 2] =  sum / 2;
  }
  audioData_ = monoData;
}

void equalizer::Equalizer::openFile(const std::string& filename)
{
  WavHeader header;
  std::vector< int16_t > audioData;
  header.readWavFile(filename, audioData);
  header_ = header;
  audioData_ = audioData;
}
