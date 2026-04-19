#ifndef CLI
#define CLI
#include <string>
#include <iostream>
#include "equalizer.hpp"
namespace equalizer
{
  struct cliEqualizer {
  public:
    void load(std::istream& in, std::ostream& out, const std::string& name);
    void save(std::istream& in, std::ostream& out, const std::string& name);

    void rename(std::istream& in, std::ostream& out, const std::string& name);
    void changeVolume(std::istream& in, std::ostream& out);
    void inverse(std::istream& in, std::ostream& out);
    void reverse(std::istream& in, std::ostream& out);
    void fromStereoToMono(std::istream& in, std::ostream& out);
    void mute(std::istream& in, std::ostream& out);
    void unmute(std::istream& in, std::ostream& out);
    void getInfo(std::istream& in, std::ostream& out);
    void cutLeft(std::istream& in, std::ostream& out);
    void cutRight(std::istream& in, std::ostream& out);
    void exit(std::istream& in, std::ostream& out);
  private:
    std::string fileName;
    equalizer::Equalizer equalizer;
    bool isLoaded = false;
    bool isSaved = true;
  };
}

#endif