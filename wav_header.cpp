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
  size_t size = filename.size();
  if (filename.substr(size - 4, 4) != ".wav")
  {
    throw std::invalid_argument("Wrong file format");
  }

  std::ifstream file(filename, std::ios::binary);

  if (!file.is_open())
  {
    throw std::invalid_argument("File opening trouble");
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

void equalizer::WavHeader::saveWav(const std::string& filename, const std::vector<int16_t> audioData)
{
  std::ofstream file(filename, std::ios::binary);
  if (!file.is_open())
  {
    std::cerr << "Problems with creation file" << filename << "\n";
    throw;
  }
  file.write(reinterpret_cast< const char* >(&(*this)), sizeof(WavHeader));
  file.write(reinterpret_cast< const char* >(audioData.data()), audioData.size() * sizeof(int16_t));
}

void equalizer::WavHeader::showInfo(std::ostream& out = std::cout) const noexcept
{
  std::cout << "ChunkID: " << chunkID_ << '\n';
  std::cout << "ChunkSize: " << chunkSize_ << '\n';
  std::cout << "Format: " << format_ << '\n';

  std::cout << "Subchunk1ID: " << subchunk1ID_ << '\n';
  std::cout << "Subchunk1Size: " << subchunkSize_ << '\n';
  std::cout << "AudioFormat: " << audioFormat_ << '\n';
  std::cout << "NumChannels: " << numChannels_ << '\n';
  std::cout << "SampleRate: " << sampleRate_ << '\n';
  std::cout << "ByteRate: " << byteRate_ << '\n';
  std::cout << "BlockAlign: " << blockAlign_ << '\n';
  std::cout << "BitsPerSample: " << bitsPerSample_ << '\n';

  std::cout << "Subchunk2ID: " << subchunk2ID_ << '\n';
  std::cout << "Subchunk2Size: " << subchunk2Size_ << '\n';
}