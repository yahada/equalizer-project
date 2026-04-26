#include "wav_header.hpp"

bool equalizer::WavHeader::checkCorrectnessOfHeader(std::string& errorMsg)
{

  if (std::string(chunkID_, 4) != "RIFF")
  {
    errorMsg = "Not a valid RIFF file";
    return false;
  }

  if (std::string(format_, 4) != "WAVE")
  {
    errorMsg = "Not a valid WAVE file";
    return false;
  }

  if (sampleRate_ != 44100 && sampleRate_ != 48000)
  {
    errorMsg = "Equalizer supports files with sample rate 44100 or 48000 Hz";
    return false;
  }

  if (numChannels_ != 1 && numChannels_ != 2)
  {
    errorMsg = "Equalizer support work only with files with 1 or 2 channels";
    return false;
  }

  if (bitsPerSample_ != 16)
  {
    errorMsg = "Equalizer support work only with 16 bit files";
    return false;
  }

  if (std::string(subchunk1ID_, 3) != "fmt")
  {
    errorMsg = "Missing <<fmt>> sub chunck in input file";
    return false;
  }

  if (std::string(subchunk2ID_, 4) != "data")
  {
    errorMsg = "Missing <<data>> sub chunck in input file";
    return false;
  }
  return true;
}

void equalizer::WavHeader::readWavFile(const std::string& filename, std::vector< int16_t >& audioData)
{
  if (filename.size() < 4)
  {
    throw std::invalid_argument("Wrong file format: " + filename);
  }

  if (filename.substr(filename.size() - 4, 4) != ".wav")
  {
    throw std::invalid_argument("Wrong file format: " + filename);
  }

  std::ifstream file(filename, std::ios::binary);

  if (!file.is_open())
  {
    throw std::invalid_argument("Cannot open file: " + filename);
  }

  file.read(reinterpret_cast< char* >(&(*this)), sizeof(WavHeader));

  std::string errMsg;
  if (!checkCorrectnessOfHeader(errMsg))
  {
    throw std::invalid_argument(errMsg);
  }

  audioData.resize(subchunk2Size_ / sizeof(int16_t));

  file.read(reinterpret_cast< char* >(audioData.data()), subchunk2Size_);
}

void equalizer::WavHeader::saveWav(const std::string& filename, const std::vector<int16_t>& audioData)
{
  std::ofstream file(filename, std::ios::binary);
  if (!file.is_open())
  {
    throw std::runtime_error("Problems with creating file: " + filename);
  }
  file.write(reinterpret_cast< const char* >(&(*this)), sizeof(WavHeader));
  file.write(reinterpret_cast< const char* >(audioData.data()), audioData.size() * sizeof(int16_t));
}

void equalizer::WavHeader::showInfo(std::ostream& out) const noexcept
{
  out << "ChunkID: " << chunkID_ << '\n';
  out << "ChunkSize: " << chunkSize_ << '\n';
  out << "Format: " << format_ << '\n';

  out << "Subchunk1ID: " << subchunk1ID_ << '\n';
  out << "Subchunk1Size: " << subchunkSize_ << '\n';
  out << "AudioFormat: " << audioFormat_ << '\n';
  out << "NumChannels: " << numChannels_ << '\n';
  out << "SampleRate: " << sampleRate_ << '\n';
  out << "ByteRate: " << byteRate_ << '\n';
  out << "BlockAlign: " << blockAlign_ << '\n';
  out << "BitsPerSample: " << bitsPerSample_ << '\n';

  out << "Subchunk2ID: " << subchunk2ID_ << '\n';
  out << "Subchunk2Size: " << subchunk2Size_ << '\n';
}
