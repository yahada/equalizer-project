#include "cli.hpp"
#include <algorithm>
#include <sstream>
#include <cctype>

void equalizer::cliEqualizer::load(std::istream&, std::ostream& out, const std::vector< std::string >& params)
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
  // if (!isSaved)
  // {
  //   out << "<YOU HAVE AN UNSAVED TRACK>\n";
  //   out << "Do you want to save it (y/n): ";
  //   std::string ans;
  //   in >> ans;
  //   std::transform(ans.begin(), ans.end(), ans.begin(), tolower);
  //   while (ans != "y" && ans != "yes" && ans != "n" && ans != "no")
  //   {
  //     out << "<WRONG INPUT>\n";
  //     out << "Do you want to save it (y/n): ";
  //     in >> ans;
  //   }
  //   if (ans == "y" || ans == "yes")
  //   {
  //     std::string newFileName;
  //     out << "Enter new name of the file: ";
  //     in >> newFileName;
  //     equalizer::cliEqualizer::save(in, out, newFileName);
  //     return;
  //   }
  // }
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

// void equalizer::cliEqualizer::getInfo(std::istream&, std::ostream& out)
// {
//   if (!isLoaded)
//   {
//     out << "<THERE'S NO TRACK TO SHOW INFO ABOUT. PLEASE LOAD IT FIRST>\n";
//     return;
//   }
//   out << "<INFO ABOUT FILE>\n";
//   equalizer.showInfoAboutFile(out);
// }

// void equalizer::cliEqualizer::mute(std::istream&, std::ostream& out)
// {
//   if (!isLoaded)
//   {
//     out << "<THERE'S NO TRACK TO MUTE. PLEASE LOAD IT FIRST>\n";
//     return;
//   }
//   if (!equalizer.isMuted_)
//   {
//   equalizer.changeMuteStatus();
//   isSaved = false;

//   }
//   out << "<THE TRACK WAS SUCCESSFULLY MUTED>\n";

// }

// void equalizer::cliEqualizer::unmute(std::istream&, std::ostream& out)
// {
//   if (!isLoaded)
//   {
//     out << "<THERE'S NO TRACK TO MUTE. PLEASE LOAD IT FIRST>\n";
//     return;
//   }

//   if (equalizer.isMuted_)
//   {
//     equalizer.changeMuteStatus();
//     isSaved = false;
//   }
//   out << "<THE TRACK WAS SUCCESSFULLY UNMUTED>\n";
// }

// void equalizer::cliEqualizer::reverse(std::istream&, std::ostream& out)
// {
//   if (!isLoaded)
//   {
//     out << "<THERE'S NO TRACK TO REVERSE. PLEASE LOAD IT FIRST>\n";
//     return;
//   }

//   equalizer.reverse();
//   isSaved = false;
//   out << "<THE TRACK WAS SUCCESSFULLY REVERSED>\n";
// }

// void equalizer::cliEqualizer::inverse(std::istream&, std::ostream& out)
// {
//   if (!isLoaded)
//   {
//     out << "<THERE'S NO TRACK TO INVERSED. PLEASE LOAD IT FIRST>\n";
//     return;
//   }
//   equalizer.inversion();
//   isSaved = false;
//   out << "<THE TRACK WAS SUCCESSFULLY INVERSED>\n";
// }

// void equalizer::cliEqualizer::fromStereoToMono(std::istream&, std::ostream& out)
// {
//   if (!isLoaded)
//   {
//     out << "<THERE'S NO TRACK TO CONVERT. PLEASE LOAD IT FIRST>\n";
//     return;
//   }

//   if (equalizer.header_.numChannels_ == 1)
//   {
//     return;
//   }

//   equalizer.StereoToMono();
//   isSaved = false;
//   out << "<THE TRACK WAS SUCCESSFULLY CONVERTED FROM STEREO TO MONO>\n";
// }

// void equalizer::cliEqualizer::changeVolume(std::istream& in, std::ostream& out)
// {
//   if (!isLoaded)
//   {
//     out << "<THERE'S NO TRACK TO CHANGE VOLUME. PLEASE LOAD IT FIRST>\n";
//     return;
//   }

//   std::string params;
//   std::getline(in, params);
//   std::string item;
//   std::stringstream ss(params);
//   std::vector< std::string > splited_params;
//   while (std::getline(ss, item, ' '))
//   {
//     splited_params.push_back(item);
//   }
  
//   if (splited_params.empty())
//   {
//     out << "<NO PARAMETERS FOUND. USE --help TO GET DESCRIPTION OF ALL POSSIBLE PARAMETERS YOU CAN ENTER>\n";
//     return;
//   }
//   if (splited_params[0] == "--help")
//   {
//     out << "ALL POSIBLE MODIFICATORS TO THIS COMMAND:\n";
//   }
//   float gain = 0.0;
//   try
//   {
//     gain = std::stof(splited_params[0]);
//   }
//   catch(...)
//   {
//     throw std::invalid_argument("First parametr must be integer");
//   }
//   if (gain < -200.0 || gain > 200.0)
//   {
//     throw std::invalid_argument("Volume gain must be in range [-200, 200]");
//   }
//   bool lowsFlag = false;
//   bool midFlag = false;
//   bool highFlag = false;
//   for (size_t i = 1; i < splited_params.size(); ++i)
//   {
//     if (splited_params[i][0] != '-')
//     {
//       std::string errorMsg = "Unknown parametr: '" + splited_params[i] + "'";
//       throw std::invalid_argument(errorMsg);
//     } else
//     {
//       for (size_t j = 1; j < splited_params[i].size(); ++j)
//       {
//         if (splited_params[i][j] != 'h' || splited_params[i][j] != 'm' || splited_params[i][j] != 'l')
//         {
//           std::string errorMsg = "Unknown modificator: '" + std::to_string(splited_params[i][j]) + "'";
//       throw std::invalid_argument(errorMsg);
//         } else
//         {
//           switch(splited_params[i][j])
//           {
//             case 'h':
//               highFlag = true;
//             case 'l':
//               lowsFlag = true;
//             case 'm':
//               midFlag = true;
//           }
//         }
//       }
//     }
//   }
//   if (highFlag || midFlag || lowsFlag == 0)
//   {
//     equalizer.changeVolume(gain / 100, gain / 100, gain / 100);
//     out << "<VOLUME WAS CHANGED SUCCESSFULLY>\n";
//     return;
//   }

//   if (lowsFlag)
//   {
//     equalizer.changeVolume(gain / 100, 0.0, 0.0);
//   }
//   if (midFlag)
//   {
//     equalizer.changeVolume(0.0, gain / 100, 0.0);
//   }
//   if (highFlag)
//   {
//     equalizer.changeVolume(0.0, 0.0, gain / 100);
//   }
//   out << "<VOLUME WAS CHANGED SUCCESSFULLY>\n";

// }

// void equalizer::cliEqualizer::exit(std::istream& in, std::ostream& out)
// {
//   if (!isSaved)
//   {
//     out << "<YOU HAVE AN UNSAVED TRACK>\n";
//     out << "Do you want to save it (y/n): ";
//     std::string ans;
//     in >> ans;
//     std::transform(ans.begin(), ans.end(), ans.begin(), tolower);
//     while (ans != "y" && ans != "yes" && ans != "n" && ans != "no")
//     {
//       out << "<WRONG INPUT>\n";
//       out << "Do you want to save it (y/n): ";
//       in >> ans;
//     }
//     if (ans == "y" || ans == "yes")
//     {
//       std::string newFileName;
//       out << "Enter new name of the file: ";
//       in >> newFileName;
//       equalizer::cliEqualizer::save(in, out, newFileName);
//       return;
//     }
//   } else
//   {
//     return;
//   }
// }









