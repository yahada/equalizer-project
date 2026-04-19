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

    void changeVolume(std::istream& in, std::ostream& out, const std::string& name);
    void inverse(std::istream& in, std::ostream& out, const std::string& name);
    void reverse(std::istream& in, std::ostream& out, const std::string& name);
    void fromStereoToMono(std::istream& in, std::ostream& out, const std::string& name);
    void mute(std::istream& in, std::ostream& out, const std::string& name);
    void getInfo(std::istream& in, std::ostream& out, const std::string& name);
  private:
    std::string fileName;
    equalizer::Equalizer equalizer;
    bool isSaved;
  };
}

#endif