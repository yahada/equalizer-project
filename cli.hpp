#ifndef CLI
#define CLI
#include <string>
#include <iostream>
namespace equalizer
{
  struct cliEqualizer {
  public:
    void load(std::istream& in, std::ostream& out, std::string);
    void save(std::istream& in, std::ostream& out, std::string);
    void rename(std::istream& in, std::ostream& out, std::string);

    void changeVolume(std::istream& in, std::ostream& out, std::string);
    void inverse(std::istream& in, std::ostream& out, std::string);
    void reverse(std::istream& in, std::ostream& out, std::string);
    void fromStereoToMono(std::istream& in, std::ostream& out, std::string);
    void mute(std::istream& in, std::ostream& out, std::string);
    void getInfo(std::istream& in, std::ostream& out, std::string);
  private:
    std::string fileName;
    bool isSaved;
  };
}

#endif