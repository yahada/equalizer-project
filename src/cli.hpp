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
    void mute(std::istream& in, std::ostream& out, const std::vector< std::string >& params);
    void unmute(std::istream& in, std::ostream& out, const std::vector< std::string >& params);
    void reverse(std::istream& in, std::ostream& out, const std::vector< std::string >& params);
    void inverse(std::istream& in, std::ostream& out, const std::vector< std::string >& params);
    void fromStereoToMono(std::istream& in, std::ostream& out, const std::vector< std::string >& params);
    void changeVolume(std::istream& in, std::ostream& out, const std::vector< std::string >& params);
    void help(std::istream& in, std::ostream& out, const std::vector< std::string >& params);
    void settings(std::istream& in, std::ostream& out, const std::vector< std::string >& params);
    void loadSettings(std::istream& in, std::ostream& out, const std::vector< std::string >& params);
    // void cutLeft(std::istream& in, std::ostream& out, const std::vector< std::string >& params);
    // void cutRight(std::istream& in, std::ostream& out, const std::vector< std::string >& params);
    void success(std::ostream& out, const std::string& text);
    std::string error(const std::string& text);
    std::string warning(const std::string& text);
    void printBanner(std::ostream& out);
    void exit(std::istream& in, std::ostream& out, const std::vector< std::string >& params);
  private:
    std::vector< std::string > getSplitedParams(std::istream& in);
    std::string fileName;
    equalizer::Equalizer equalizer;
    bool isLoaded = false;
    bool isSaved = true;
  };
}

#endif