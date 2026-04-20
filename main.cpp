#include "cli.hpp"
#include <limits>
#include <unordered_map>
int main()
{
  using namespace equalizer;
  cliEqualizer cli;
  using cmdNoName_t = void (cliEqualizer::*)(std::istream &, std::ostream &);
  std::unordered_map< std::string, cmdNoName_t > cmdsNoName;
  cmdsNoName["info"] = &cliEqualizer::getInfo;
  cmdsNoName["mute"] = &cliEqualizer::mute;
  cmdsNoName["unmute"] = &cliEqualizer::unmute;
  cmdsNoName["reverse"] = &cliEqualizer::reverse;
  cmdsNoName["inverse"] = &cliEqualizer::inverse;
  cmdsNoName["volume"] = &cliEqualizer::changeVolume;
  cmdsNoName["exit"] = &cliEqualizer::exit;


  using cmdWithName_t = void (cliEqualizer::*)(std::istream &, std::ostream &, const std::string&);
  std::unordered_map< std::string, cmdWithName_t > cmdWithName;
  cmdWithName["load"] = &cliEqualizer::load;
  cmdWithName["save"] = &cliEqualizer::save;
  cmdWithName["rename"] = &cliEqualizer::rename;

  std::string cmd;
  while (!std::cin.eof())
  {
    std::cout << "> ";
    std::cin >> cmd;

    if (std::cin.eof())
    {
      break;
    }

    try
    {
      cmdsNoName.at(cmd);
      (cli.*cmdsNoName.at(cmd))(std::cin, std::cout);
    }
    catch(...)
    {
      try
      {
        cmdWithName.at(cmd);
        std::string name;
        std::cin >> name;
        (cli.*cmdWithName.at(cmd))(std::cin, std::cout, name);
      }
      catch (const std::out_of_range&)
      {
        std::cerr << "<INVALID COMMAND>\n";

        auto toignore = std::numeric_limits<std::streamsize>::max();
        std::cin.ignore(toignore, '\n');
      }
      catch (const std::exception& e)
      {
        std::cerr << "<INVALID COMMAND: " << e.what() << " >\n";
        auto toignore = std::numeric_limits<std::streamsize>::max();
        std::cin.ignore(toignore, '\n');
      }
    }
    if (cmd == "exit")
    {
      return 0;
    }
  }

  if (std::cin.eof())
  {
    std::cerr << "Bad input\n";
    return 1;
  }
}
