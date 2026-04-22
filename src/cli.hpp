#ifndef CLI
#define CLI
#include <string>
#include <vector>
#include <iostream>
#include "equalizer.hpp"
namespace equalizer
{
  class cliEqualizer {
  public:
    void load(std::istream& in, std::ostream& out, const std::vector< std::string >& params);
    void save(std::istream& in, std::ostream& out, const std::vector< std::string >& params);
    void rename(std::istream& in, std::ostream& out, const std::vector< std::string >& params);
    void getInfo(std::istream& in, std::ostream& out, const std::vector< std::string >& params);
    // void changeVolume(std::istream& in, std::ostream& out, const std::vector< std::string >& params);
    // void inverse(std::istream& in, std::ostream& out, const std::vector< std::string >& params);
    // void reverse(std::istream& in, std::ostream& out, const std::vector< std::string >& params);
    // void fromStereoToMono(std::istream& in, std::ostream& out, const std::vector< std::string >& params);
    // void mute(std::istream& in, std::ostream& out, const std::vector< std::string >& params);
    // void unmute(std::istream& in, std::ostream& out, const std::vector< std::string >& params);
    // void cutLeft(std::istream& in, std::ostream& out, const std::vector< std::string >& params);
    // void cutRight(std::istream& in, std::ostream& out, const std::vector< std::string >& params);
    // void exit(std::istream& in, std::ostream& out, const std::vector< std::string >& params);
  private:
    std::vector< std::string > getSplitedParams(std::istream& in);
    std::string fileName;
    equalizer::Equalizer equalizer;
    bool isLoaded = false;
    bool isSaved = true;
  };
}

#endif