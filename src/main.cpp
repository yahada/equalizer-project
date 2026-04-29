#define GL_SILENCE_DEPRECATION

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "cli.hpp"
#include <sstream>
#include <limits>
#include <unordered_map>
#include "equalizer_ui.hpp"

namespace equalizer
{
void setupGlfwForPlatform(const char*& glslVersion)
{
  glslVersion = "#version 130";
}

void applyAppStyle()
{
  ImGui::StyleColorsDark();

  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowRounding = 0.0f;
  style.ChildRounding = 0.0f;
  style.FrameRounding = 0.0f;
  style.PopupRounding = 0.0f;
  style.ScrollbarRounding = 0.0f;
  style.GrabRounding = 0.0f;
  style.FramePadding = ImVec2(6.0f, 4.0f);
  style.ItemSpacing = ImVec2(8.0f, 6.0f);
  style.WindowPadding = ImVec2(8.0f, 8.0f);
}

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
}



int main()
{
  using namespace equalizer;
  if (!glfwInit())
  {
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
    cmds["reset"] = &cliEqualizer::reset;
    cmds["loadSet"] = &cliEqualizer::getSettings;
    cmds["cut"] = &cliEqualizer::cut;
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

  const char* glslVersion = nullptr;
  setupGlfwForPlatform(glslVersion);

  GLFWwindow* window = glfwCreateWindow(1000, 700, "Audio Equalizer", nullptr, nullptr);
  if (!window)
  {
    glfwTerminate();
    return 1;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;

  applyAppStyle();

  if (!ImGui_ImplGlfw_InitForOpenGL(window, true))
  {
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
  }

  if (!ImGui_ImplOpenGL3_Init(glslVersion))
  {
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
  }

  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    renderEqualizerUI();

    ImGui::Render();

    int displayWidth = 0;
    int displayHeight = 0;
    glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
    glViewport(0, 0, displayWidth, displayHeight);
    glClearColor(0.06f, 0.06f, 0.06f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
  }
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}