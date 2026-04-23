#include "cli.hpp"
#include <algorithm>
#include <sstream>
#include <cctype>
#include <filesystem>

void equalizer::cliEqualizer::load(std::istream& in, std::ostream& out, const std::vector< std::string >& params)
{
  if (params.size() == 1)
  {
    throw std::invalid_argument("no file to load");
  }
  if (params.size() > 2)
  {
    throw std::invalid_argument("load function takes only one parametr");
  }
  std::string name = params[1];
  if (name == fileName)
  {
    return;
  }
  if (!isSaved)
  {
    out << "<YOU HAVE AN UNSAVED TRACK>\n";
    out << "Do you want to save it (y/n): ";
    std::string ans;
    in >> ans;
    std::transform(ans.begin(), ans.end(), ans.begin(), tolower);
    while (ans != "y" && ans != "yes" && ans != "n" && ans != "no")
    {
      out << "<WRONG INPUT>\n";
      out << "Do you want to save it (y/n): ";
      in >> ans;
    }
    if (ans == "y" || ans == "yes")
    {
      std::string newFileName;
      out << "Enter new name of the file: ";
      in >> newFileName;
      std::vector< std::string > tmpParams = {0, newFileName};
      equalizer::cliEqualizer::save(in, out, tmpParams);
      return;
    }
  }
  equalizer.openFile(name);
  fileName = name;
  isSaved = true;
  isLoaded = true;
  out << "<TRACK WITH NAME " << name << " WAS SUCCESSFULLY LOADED>\n";
}

void equalizer::cliEqualizer::save(std::istream&, std::ostream& out, const std::vector< std::string >& params)
{
  if (!isLoaded)
  {
    throw std::invalid_argument("no track to save");
  }
  if (params.size() == 1)
  {
    equalizer.saveFile(fileName);
  } else
  {
    for (size_t i = 1; i < params.size(); ++i)
    {
      equalizer.saveFile(params[i]);
    }
  }
  isSaved = true;
  out << "<THE TRACK WAS SUCCESSFULLY SAVED>\n";
}

void equalizer::cliEqualizer::rename(std::istream&, std::ostream& out, const std::vector< std::string >& params)
{
  if (!isLoaded || params.size() == 1)
  {
    throw std::invalid_argument("no file to rename");
  }
  if (params.size() > 2)
  {
    throw std::invalid_argument("rename function takes only one parametr");
  }
  std::string name = params[1];
  if (fileName == name)
  {
    return;
  }
  equalizer.renameFile(fileName, name);
  fileName = name;
  out << "<THE FILE WAS SUCCESSFULLY RENAMED>\n";
}

void equalizer::cliEqualizer::getInfo(std::istream&, std::ostream& out, const std::vector< std::string >& params)
{
  if (!isLoaded)
  {
    throw std::invalid_argument("no file to get info about");
  }
  if (params.size() == 1)
  {
    out << "<INFO ABOUT FILE>\n";
    out << "name: " << fileName << '\n';
    equalizer.showInfoAboutFile(out);

    return;
  } else
  {
    if (params[1] == "-f")
    {
      std::string tmp = fileName.substr(0, fileName.size() - 4) + "_info.txt";
      std::string res = tmp;
      if (params.size() == 3)
      {
        size_t pos = fileName.find_last_of('/');
        if (pos != std::string::npos)
        {
          res = tmp.substr(pos, tmp.size());
        }
        res = params[2] + res;
      }
      std::ofstream outfile(res);
      if (!outfile)
      {
        throw std::invalid_argument("can't find directory with this path <" + res + ">");
      }
      outfile << "<INFO ABOUT FILE>\n";
      outfile << "name: " << fileName << '\n';
      equalizer.showInfoAboutFile(outfile);
      out << "<SAVED TO: " << res << ">\n";
      outfile.close();
      return;
    }
    else
    {
      throw std::invalid_argument("unknown parametr <" + params[1] + ">");
    }
  }
}

void equalizer::cliEqualizer::mute(std::istream&, std::ostream& out, const std::vector< std::string >&)
{
  if (!isLoaded)
  {
    throw std::invalid_argument("no track to mute");
  }
  if (!equalizer.isMuted_)
  {
  equalizer.changeMuteStatus();
  isSaved = false;
  }
  out << "<THE TRACK WAS SUCCESSFULLY MUTED>\n";
}

void equalizer::cliEqualizer::unmute(std::istream&, std::ostream& out, const std::vector< std::string >&)
{
  if (!isLoaded)
  {
    throw std::invalid_argument("no track to unmute");
  }

  if (equalizer.isMuted_)
  {
    equalizer.changeMuteStatus();
    isSaved = false;
  }
  out << "<THE TRACK WAS SUCCESSFULLY UNMUTED>\n";
}

void equalizer::cliEqualizer::reverse(std::istream&, std::ostream& out, const std::vector< std::string >&)
{
  if (!isLoaded)
  {
    throw std::invalid_argument("no track to reverse");
  }

  equalizer.reverse();
  isSaved = false;
  out << "<THE TRACK WAS SUCCESSFULLY REVERSED>\n";
}

void equalizer::cliEqualizer::inverse(std::istream&, std::ostream& out, const std::vector< std::string >&)
{
  if (!isLoaded)
  {
    throw std::invalid_argument("no track to inverse");
  }
  equalizer.inversion();
  isSaved = false;
  out << "<THE TRACK WAS SUCCESSFULLY INVERSED>\n";
}

void equalizer::cliEqualizer::fromStereoToMono(std::istream&, std::ostream& out, const std::vector< std::string >&)
{
  if (!isLoaded)
  {
    throw std::invalid_argument("no track to convert from stereo to mono");
  }

  if (equalizer.header_.numChannels_ == 1)
  {
    throw std::invalid_argument("track already in mono format");
  }
  equalizer.StereoToMono();
  isSaved = false;

  out << "<THE TRACK WAS SUCCESSFULLY CONVERTED FROM STEREO TO MONO>\n";
}

void equalizer::cliEqualizer::changeVolume(std::istream&, std::ostream& out, const std::vector< std::string >& params)
{
  if (!isLoaded)
  {
    throw std::invalid_argument("no track to change volume");
  }

  if (params.size() == 1)
  {
    throw std::invalid_argument("no parametrs found. Use --help for more information");
  }

  if (params[1] == "--help")
  {
    out << "COMMANDS\n";
    return;
  }

  float gain = 0.0;
  try
  {
    gain = std::stof(params[1]);
  }
  catch(...)
  {
    throw std::invalid_argument("first parametr must be integer");
  }

  if (gain < -200.0 || gain > 200.0)
  {
    throw std::invalid_argument("volume gain must be in range [-200, 200]");
  }

  bool lowsFlag = false;
  bool midFlag = false;
  bool highFlag = false;
  for (size_t i = 2; i < params.size(); ++i)
  {
    if (params[i] == "--help")
    {
      out << "COMMANDS\n";
      return;
    }

    if (params[i] != "-h" && params[i] != "-m" && params[i] != "-l")
    {
      throw std::invalid_argument("unknown parametr <" + params[i] + ">");
    }
    switch(params[i][1])
    {
      case 'h': highFlag = true;
      case 'm': midFlag = true;
      case 'l': lowsFlag = true;
    }
  }
  if (highFlag || midFlag || lowsFlag == 0)
  {
    equalizer.changeVolume(gain / 100, gain / 100, gain / 100);
    out << "<VOLUME WAS CHANGED SUCCESSFULLY>\n";
    return;
  }

  if (lowsFlag)
  {
    equalizer.changeVolume(gain / 100, 0.0, 0.0);
  }
  if (midFlag)
  {
    equalizer.changeVolume(0.0, gain / 100, 0.0);
  }
  if (highFlag)
  {
    equalizer.changeVolume(0.0, 0.0, gain / 100);
  }
  out << "<VOLUME WAS CHANGED SUCCESSFULLY>\n";
}

void equalizer::cliEqualizer::exit(std::istream& in, std::ostream& out, const std::vector< std::string >& params)
{
  if (!isSaved)
  {
    out << "<YOU HAVE AN UNSAVED TRACK>\n";
    out << "Do you want to save it (y/n): ";
    std::string ans;
    in >> ans;
    std::transform(ans.begin(), ans.end(), ans.begin(), tolower);
    while (ans != "y" && ans != "yes" && ans != "n" && ans != "no")
    {
      out << "<WRONG INPUT>\n";
      out << "Do you want to save it (y/n): ";
      in >> ans;
    }
    if (ans == "y" || ans == "yes")
    {
      std::string newFileName;
      out << "Enter new name of the file: ";
      in >> newFileName;
      std::vector< std::string > tmpParams = {0, newFileName};
      equalizer::cliEqualizer::save(in, out, tmpParams);
      return;
    }
  }
}









