#include "cli.hpp"
#include <limits>
#include <unordered_map>
#include <sstream>

std::vector< std::string > getSplitedParams(std::istream& in)
{
  std::string line;
  std::getline(in, line);

  std::stringstream ss(line);
  std::vector<std::string> result;
  std::string word;

  while (ss >> word)
  {
    result.push_back(word);
  }
  return result;
}

int main()
{
  using namespace equalizer;
  cliEqualizer cli;
  cli.printBanner(std::cout);
  using cmds_t = void (cliEqualizer::*)(std::istream&, std::ostream&, const std::vector< std::string >& params);
  std::unordered_map< std::string, cmds_t > cmds;
  cmds["load"] = &cliEqualizer::load;
  cmds["save"] = &cliEqualizer::save;
  cmds["rename"] = &cliEqualizer::rename;
  cmds["info"] = &cliEqualizer::getInfo;
  cmds["mute"] = &cliEqualizer::mute;
  cmds["unmute"] = &cliEqualizer::unmute;
  cmds["reverse"] = &cliEqualizer::reverse;
  cmds["inverse"] = &cliEqualizer::inverse;
  cmds["volume"] = &cliEqualizer::changeVolume;
  cmds["help"] = &cliEqualizer::help;
  cmds["convert"] = &cliEqualizer::changeVolume;
  cmds["settings"] = &cliEqualizer::settings;
  cmds["exit"] = &cliEqualizer::exit;

  std::string line;
  while (!std::cin.eof())
  {
    std::cout << "> ";
    std::vector< std::string > splitedParams = getSplitedParams(std::cin);
    if (std::cin.eof())
    {
      std::cerr << "Bad input\n";
      return 1;
    }
    if (splitedParams.size() == 0)
    {
      continue;
    }
    std::string cmd = splitedParams[0];
    try
    {
      cmds.at(cmd);
      (cli.*cmds.at(cmd))(std::cin, std::cout, splitedParams);
    }
    catch (const std::out_of_range&)
    {
      std::cerr << cli.error("Unknown command");
    }
    catch (const std::exception& e)
    {
      std::cerr << e.what();
    }
    if (cmd == "exit")
    {
      return 0;
    }
  }
}
