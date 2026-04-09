#include "equalizer.hpp"
#include <cstdio>
#include <math.h>

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
  header_.numChannels_ = 1;
  uint32_t tmpSize = header_.subchunk2Size_ / 2;
  header_.chunkSize_ -= tmpSize;
  header_.subchunk2Size_ /= 2;
  header_.blockAlign_ = header_.numChannels_ * header_.bitsPerSample_ / 8;
  header_.byteRate_ = header_.sampleRate_ * header_.numChannels_ * header_.bitsPerSample_ / 8;
}

void equalizer::Equalizer::openFile(const std::string& filename)
{
  WavHeader header;
  std::vector< int16_t > audioData;
  header.readWavFile(filename, audioData);
  header_ = header;
  audioData_ = audioData;
}

void equalizer::Equalizer::saveFile(const std::string& filename)
{
  header_.saveWav(filename, audioData_);
}

void equalizer::Equalizer::renameFile(const std::string& oldName, const std::string& newName)
{
  if (oldName.substr(oldName.size() - 4, 4) != ".wav")
  {
    throw std::invalid_argument("Wrong file format");
  }

  if (newName.substr(newName.size() - 4, 4) != ".wav")
  {
    throw std::invalid_argument("Can't save to this file because file has wrong format");
  }

  if (std::rename(oldName.c_str(), newName.c_str()) != 0) {
    throw std::logic_error("no file with this name");
  }
}


void equalizer::Equalizer::showInfoAboutFile(std::ostream& out)
{
  header_.showInfo(out);
}

std::vector< float > equalizer::Equalizer::convert()
{
    std::vector< float > samples(audioData_.size());
    for (size_t i = 0; i < audioData_.size(); ++i)
    {
      samples[i] = audioData_[i] / 32768.0f;
    }
    return samples;
}

float equalizer::Equalizer::countAlpha(float cutoff)
{
  float dt = 1.0f / header_.sampleRate_;
  float rc = 1.0f / (2 * M_PI * cutoff);
  return dt / (rc + dt);
}

void equalizer::Equalizer::inversion()
{
  std::vector< int16_t > inversedData(audioData_.size());
  for (size_t i = 0; i < audioData_.size(); ++i)
  if (audioData_[i] == -32768)
  {
    inversedData[i] = 32767;
  }
  else
  {
    inversedData[i] = -audioData_[i];
  }
  audioData_ = inversedData;
}

void equalizer::Equalizer::reverse()
{
  std::vector< int16_t > reversedData(audioData_.size());
  for (size_t i = 0; i < audioData_.size(); ++i)
  {
    reversedData[i] = audioData_[audioData_.size() - i - 1];
  }
  audioData_ = reversedData;
}

void equalizer::Equalizer::cutFromLeft(const float& cutSize)
{
  leftCut_ = cutSize;
}

void equalizer::Equalizer::cutFromRight(const float& cutSize)
{
  RightCut_ = cutSize;
}

void equalizer::Equalizer::changeMuteStatus()
{
  isMuted_ = !isMuted_;
}