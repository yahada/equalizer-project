#include "cli.hpp"
#include <limits>
#include <unordered_map>
int main()
{
  using namespace equalizer;
  cliEqualizer cli;
  using cmd_t = void (cliEqualizer::*)(std::istream &, std::ostream &, const std::string &);
  std::unordered_map< std::string, cmd_t > cmds;
  cmds["load"] = &cliEqualizer::load;
  cmds["save"] = &cliEqualizer::save;
  cmds["rename"] = &cliEqualizer::rename;

  std::string cmd;
  while (std::cin >> cmd)
  {
    try
    {
      cmds.at(cmd);
      std::string name;
      std::cin >> name;
      (cli.*cmds.at(cmd))(std::cin, std::cout, name);
    }
    catch (const std::exception& e)
    {
      std::cerr << "<INVALID COMMAND: " << e.what() << " >\n";
      auto toignore = std::numeric_limits<std::streamsize>::max();
      std::cin.ignore(toignore, '\n');
    }
    catch (...)
    {
      std::cerr << "<INVALID COMMAND>\n";
      auto toignore = std::numeric_limits<std::streamsize>::max();
      std::cin.ignore(toignore, '\n');
    }
  }

  if (!std::cin.eof())
  {
    std::cerr << "Bad input\n";
    return 1;
  }
  return 0;
}
