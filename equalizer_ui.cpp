#include "equalizer_ui.hpp"
#include "equalizer.hpp"

#include <imgui.h>

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
struct UIContext
{
  equalizer::Equalizer equalizer;
  EqualizerUIState state{};
  bool fileLoaded = false;
  bool requestOpenPopup = false;
  bool requestSavePopup = false;
  char openPath[512] = "samples/file_example_WAV_1MG.wav";
  char savePath[512] = "processed.wav";
  std::string fileName = "No file";
  std::string status = "Open wav file first.";
  std::string headerInfo;
};

UIContext gUI{};

void setStatus(const std::string& status)
{
  gUI.status = status;
}

float totalDurationSeconds()
{
  return gUI.fileLoaded ? gUI.equalizer.durationSeconds() : 0.0f;
}

float currentTimeSeconds()
{
  return totalDurationSeconds() * gUI.state.playbackPosition;
}

void resetTimelineState()
{
  gUI.state.playbackPosition = 0.0f;
  gUI.state.selectionStart = 0.10f;
  gUI.state.selectionEnd = 0.90f;
}

void resetEqSliders()
{
  gUI.state.lowDb = 0.0f;
  gUI.state.midDb = 0.0f;
  gUI.state.highDb = 0.0f;
  gUI.state.presetIndex = 0;
}

void syncHeaderInfo()
{
  if (!gUI.fileLoaded)
  {
    gUI.headerInfo.clear();
    return;
  }

  std::ostringstream out;
  gUI.equalizer.showInfoAboutFile(out);
  gUI.headerInfo = out.str();
}

void suggestSavePath()
{
  const std::filesystem::path inputPath(gUI.openPath);
  const std::filesystem::path parent = inputPath.parent_path();
  const std::filesystem::path outputPath = parent / (inputPath.stem().string() + "_processed.wav");
  std::snprintf(gUI.savePath, sizeof(gUI.savePath), "%s", outputPath.string().c_str());
}

void openAudioFile()
{
  gUI.equalizer.openFile(gUI.openPath);
  gUI.fileLoaded = true;
  gUI.fileName = std::filesystem::path(gUI.openPath).filename().string();
  resetTimelineState();
  syncHeaderInfo();
  suggestSavePath();
  setStatus("File opened.");
}

void saveAudioFile()
{
  if (!gUI.fileLoaded)
  {
    throw std::logic_error("Open file before save.");
  }
  gUI.equalizer.saveFile(gUI.savePath);
  setStatus("File saved.");
}

void applyPresetToSliders()
{
  switch (gUI.state.presetIndex)
  {
    case 0:
      gUI.state.lowDb = 0.0f;
      gUI.state.midDb = 0.0f;
      gUI.state.highDb = 0.0f;
      break;
    case 1:
      gUI.state.lowDb = 6.0f;
      gUI.state.midDb = 1.5f;
      gUI.state.highDb = -1.0f;
      break;
    case 2:
      gUI.state.lowDb = -1.0f;
      gUI.state.midDb = 4.5f;
      gUI.state.highDb = 2.5f;
      break;
    case 3:
      gUI.state.lowDb = -1.5f;
      gUI.state.midDb = 0.5f;
      gUI.state.highDb = 6.0f;
      break;
    default:
      break;
  }
}

void applyEqFromSliders()
{
  if (!gUI.fileLoaded)
  {
    throw std::logic_error("Open file before EQ.");
  }

  gUI.equalizer.resetChanges();
  if (gUI.state.enableFilter)
  {
    gUI.equalizer.changeVolume(
      gUI.state.lowDb / 12.0f,
      gUI.state.midDb / 12.0f,
      gUI.state.highDb / 12.0f
    );
  }
  syncHeaderInfo();
  setStatus(gUI.state.enableFilter ? "EQ applied." : "EQ off.");
}

void applyEqFromUiChange()
{
  if (!gUI.fileLoaded)
  {
    setStatus("Open file before EQ.");
    return;
  }

  try
  {
    applyEqFromSliders();
  }
  catch (const std::exception& error)
  {
    setStatus(error.what());
  }
}

void finishAudioProcessing(const std::string& status)
{
  resetTimelineState();
  resetEqSliders();
  syncHeaderInfo();
  setStatus(status);
}

std::vector< float > buildWaveformPreview()
{
  if (!gUI.fileLoaded)
  {
    return {};
  }

  const std::vector< int16_t >& samples = gUI.equalizer.processedAudioData();
  const std::size_t channels = gUI.equalizer.numChannels();
  if (samples.empty() || channels == 0)
  {
    return {};
  }

  const int pointCount = 1024;
  std::vector< float > waveform(pointCount, 0.0f);
  const std::size_t totalFrames = samples.size() / channels;

  for (int pointIndex = 0; pointIndex < pointCount; ++pointIndex)
  {
    const std::size_t frameStart = static_cast< std::size_t >(
      static_cast< double >(pointIndex) * totalFrames / pointCount
    );
    const std::size_t frameEnd = std::max<std::size_t>(
      frameStart + 1,
      static_cast< std::size_t >(static_cast< double >(pointIndex + 1) * totalFrames / pointCount)
    );

    float peak = 0.0f;
    for (std::size_t frame = frameStart; frame < std::min(frameEnd, totalFrames); ++frame)
    {
      const std::size_t sampleIndex = frame * channels;
      float amplitude = 0.0f;
      for (std::size_t channel = 0; channel < channels; ++channel)
      {
        const float normalized = static_cast< float >(samples[sampleIndex + channel]) / 32768.0f;
        amplitude = std::max(amplitude, std::abs(normalized));
      }
      peak = std::max(peak, amplitude);
    }
    waveform[pointIndex] = peak;
  }

  return waveform;
}

void drawSectionTitle(const char* title)
{
  ImGui::TextUnformatted(title);
  ImGui::Separator();
}

void drawTimeLabel(const char* label, float seconds)
{
  const int totalSeconds = static_cast< int >(seconds);
  const int minutes = totalSeconds / 60;
  const int secs = totalSeconds % 60;
  ImGui::Text("%s %02d:%02d", label, minutes, secs);
}

void drawTimeRangeLabel(const char* label, float startSeconds, float endSeconds)
{
  const int startTotalSeconds = static_cast< int >(startSeconds);
  const int endTotalSeconds = static_cast< int >(endSeconds);
  ImGui::Text(
    "%s %02d:%02d - %02d:%02d",
    label,
    startTotalSeconds / 60,
    startTotalSeconds % 60,
    endTotalSeconds / 60,
    endTotalSeconds % 60
  );
}

void drawWaveformGrid(ImDrawList* drawList, const ImVec2& topLeft, const ImVec2& bottomRight)
{
  const ImU32 gridColor = IM_COL32(72, 77, 88, 70);
  const float width = bottomRight.x - topLeft.x;
  const float height = bottomRight.y - topLeft.y;

  for (int i = 1; i < 8; ++i)
  {
    const float x = topLeft.x + width * static_cast< float >(i) / 8.0f;
    drawList->AddLine(ImVec2(x, topLeft.y), ImVec2(x, bottomRight.y), gridColor, 1.0f);
  }

  for (int i = 1; i < 4; ++i)
  {
    const float y = topLeft.y + height * static_cast< float >(i) / 4.0f;
    drawList->AddLine(ImVec2(topLeft.x, y), ImVec2(bottomRight.x, y), gridColor, 1.0f);
  }

  const float centerY = (topLeft.y + bottomRight.y) * 0.5f;
  drawList->AddLine(ImVec2(topLeft.x, centerY), ImVec2(bottomRight.x, centerY), IM_COL32(120, 127, 142, 120), 1.5f);
}

void drawWaveformSignal(ImDrawList* drawList, const ImVec2& topLeft, const ImVec2& bottomRight)
{
  const std::vector< float > waveform = buildWaveformPreview();
  if (waveform.empty())
  {
    return;
  }

  const float width = bottomRight.x - topLeft.x;
  const float halfHeight = (bottomRight.y - topLeft.y) * 0.5f;
  const float centerY = topLeft.y + halfHeight;

  for (std::size_t index = 1; index < waveform.size(); ++index)
  {
    const float prevX = topLeft.x + width * static_cast< float >(index - 1) / static_cast< float >(waveform.size() - 1);
    const float currX = topLeft.x + width * static_cast< float >(index) / static_cast< float >(waveform.size() - 1);
    const float prevY = centerY - waveform[index - 1] * halfHeight * 0.82f;
    const float currY = centerY - waveform[index] * halfHeight * 0.82f;

    drawList->AddLine(ImVec2(prevX, prevY), ImVec2(currX, currY), IM_COL32(114, 181, 255, 255), 2.0f);
    drawList->AddLine(ImVec2(prevX, centerY), ImVec2(prevX, prevY), IM_COL32(114, 181, 255, 38), 1.0f);
  }
}

void drawSelectionOverlay(ImDrawList* drawList, const ImVec2& topLeft, const ImVec2& bottomRight)
{
  const float left = topLeft.x + (bottomRight.x - topLeft.x) * gUI.state.selectionStart;
  const float right = topLeft.x + (bottomRight.x - topLeft.x) * gUI.state.selectionEnd;
  drawList->AddLine(ImVec2(left, topLeft.y), ImVec2(left, bottomRight.y), IM_COL32(116, 181, 255, 255), 2.0f);
  drawList->AddLine(ImVec2(right, topLeft.y), ImVec2(right, bottomRight.y), IM_COL32(116, 181, 255, 255), 2.0f);
}

void drawPlaybackMarker(ImDrawList* drawList, const ImVec2& topLeft, const ImVec2& bottomRight)
{
  const float x = topLeft.x + (bottomRight.x - topLeft.x) * gUI.state.playbackPosition;
  drawList->AddLine(ImVec2(x, topLeft.y), ImVec2(x, bottomRight.y), IM_COL32(255, 208, 102, 255), 2.0f);
}

void drawFileDialogs()
{
  if (gUI.requestOpenPopup)
  {
    ImGui::OpenPopup("Open WAV");
    gUI.requestOpenPopup = false;
  }
  if (gUI.requestSavePopup)
  {
    ImGui::OpenPopup("Save WAV");
    gUI.requestSavePopup = false;
  }

  if (ImGui::BeginPopupModal("Open WAV", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
  {
    ImGui::TextUnformatted("Input wav path");
    ImGui::InputText("##open_path", gUI.openPath, sizeof(gUI.openPath));
    ImGui::Spacing();
    ImGui::PushTextWrapPos(420.0f);
    ImGui::TextUnformatted("Use full path or local path.");
    ImGui::PopTextWrapPos();
    if (ImGui::Button("Open", ImVec2(120.0f, 0.0f)))
    {
      try
      {
        openAudioFile();
        ImGui::CloseCurrentPopup();
      }
      catch (const std::exception& error)
      {
        setStatus(error.what());
      }
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f)))
    {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }

  if (ImGui::BeginPopupModal("Save WAV", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
  {
    ImGui::TextUnformatted("Output wav path");
    ImGui::InputText("##save_path", gUI.savePath, sizeof(gUI.savePath));
    if (ImGui::Button("Save", ImVec2(120.0f, 0.0f)))
    {
      try
      {
        saveAudioFile();
        ImGui::CloseCurrentPopup();
      }
      catch (const std::exception& error)
      {
        setStatus(error.what());
      }
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f)))
    {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}

float leftColumnWidth()
{
  return 280.0f;
}

float eqPanelHeight()
{
  return 250.0f;
}

float timelineHeight()
{
  return 170.0f;
}
}

void renderTopMenu()
{
  if (!ImGui::BeginMainMenuBar())
  {
    return;
  }

  if (ImGui::BeginMenu("File"))
  {
    if (ImGui::MenuItem("Open..."))
    {
      gUI.requestOpenPopup = true;
    }
    if (ImGui::MenuItem("Save As..."))
    {
      gUI.requestSavePopup = true;
    }
    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("Settings"))
  {
    ImGui::MenuItem("Audio settings", nullptr, false, false);
    ImGui::MenuItem("Theme", nullptr, false, false);
    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("Tools"))
  {
    if (ImGui::MenuItem("Reverse", nullptr, false, gUI.fileLoaded))
    {
      try
      {
        gUI.equalizer.reverse();
        finishAudioProcessing("Reverse done.");
      }
      catch (const std::exception& error)
      {
        setStatus(error.what());
      }
    }
    if (ImGui::MenuItem("Invert phase", nullptr, false, gUI.fileLoaded))
    {
      try
      {
        gUI.equalizer.inversion();
        finishAudioProcessing("Invert done.");
      }
      catch (const std::exception& error)
      {
        setStatus(error.what());
      }
    }
    if (ImGui::MenuItem("Convert to mono", nullptr, false, gUI.fileLoaded))
    {
      try
      {
        gUI.equalizer.StereoToMono();
        finishAudioProcessing("Mono done.");
      }
      catch (const std::exception& error)
      {
        setStatus(error.what());
      }
    }
    ImGui::EndMenu();
  }

  ImGui::EndMainMenuBar();
  drawFileDialogs();
}

void renderPlaybackControls()
{
  ImGui::BeginChild("PlaybackControls", ImVec2(0.0f, 96.0f), true);
  drawSectionTitle("Playback");

  const ImVec2 buttonSize(70.0f, 34.0f);
  if (ImGui::Button("<<", buttonSize))
  {
    gUI.state.playbackPosition = std::max(0.0f, gUI.state.playbackPosition - 0.05f);
  }
  ImGui::SameLine();
  if (ImGui::Button(gUI.state.isPlaying ? "Pause" : "Play", ImVec2(92.0f, 34.0f)))
  {
    gUI.state.isPlaying = !gUI.state.isPlaying;
  }
  ImGui::SameLine();
  if (ImGui::Button(">>", buttonSize))
  {
    gUI.state.playbackPosition = std::min(1.0f, gUI.state.playbackPosition + 0.05f);
  }

  ImGui::Spacing();
  ImGui::TextUnformatted(gUI.state.isPlaying ? "State: play" : "State: pause");
  ImGui::EndChild();
}

void renderInfoPanel()
{
  ImGui::BeginChild("InfoPanel", ImVec2(0.0f, 0.0f), true);
  drawSectionTitle("Info");

  ImGui::TextWrapped("File: %s", gUI.fileName.c_str());
  if (gUI.fileLoaded)
  {
    ImGui::Text("Sample rate: %u Hz", gUI.equalizer.sampleRate());
    ImGui::Text("Channels: %u", gUI.equalizer.numChannels());
  }
  else
  {
    ImGui::TextUnformatted("Sample rate: -");
    ImGui::TextUnformatted("Channels: -");
  }
  drawTimeLabel("Duration:", totalDurationSeconds());
  drawTimeLabel("Position:", currentTimeSeconds());

  ImGui::Spacing();
  if (ImGui::Button("Open WAV", ImVec2(-1.0f, 0.0f)))
  {
    gUI.requestOpenPopup = true;
  }
  if (ImGui::Button("Save WAV", ImVec2(-1.0f, 0.0f)))
  {
    gUI.requestSavePopup = true;
  }

  ImGui::Spacing();
  ImGui::TextUnformatted("Status");
  ImGui::PushTextWrapPos(0.0f);
  ImGui::TextUnformatted(gUI.status.c_str());
  ImGui::PopTextWrapPos();

  if (gUI.fileLoaded)
  {
    ImGui::Spacing();
    ImGui::TextUnformatted("Header");
    ImGui::BeginChild("HeaderInfo", ImVec2(0.0f, 140.0f), true, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::TextUnformatted(gUI.headerInfo.c_str());
    ImGui::EndChild();
  }

  ImGui::EndChild();
}

void renderWaveformPanel()
{
  ImGui::BeginChild("WaveformPanel", ImVec2(0.0f, 0.0f), true);
  drawSectionTitle("Waveform");

  const ImVec2 canvasPos = ImGui::GetCursorScreenPos();
  const ImVec2 canvasSize = ImGui::GetContentRegionAvail();
  const ImVec2 canvasBottomRight(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y);
  ImDrawList* drawList = ImGui::GetWindowDrawList();

  drawList->AddRectFilled(canvasPos, canvasBottomRight, IM_COL32(22, 24, 29, 255), 6.0f);
  drawList->AddRect(canvasPos, canvasBottomRight, IM_COL32(70, 74, 84, 255), 6.0f, 0, 1.0f);

  const ImVec2 innerMin(canvasPos.x + 12.0f, canvasPos.y + 12.0f);
  const ImVec2 innerMax(canvasBottomRight.x - 12.0f, canvasBottomRight.y - 12.0f);

  drawWaveformGrid(drawList, innerMin, innerMax);
  drawWaveformSignal(drawList, innerMin, innerMax);
  drawSelectionOverlay(drawList, innerMin, innerMax);
  drawPlaybackMarker(drawList, innerMin, innerMax);

  ImGui::InvisibleButton("waveform_canvas", canvasSize);
  if (ImGui::IsItemActive() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
  {
    const float mouseX = ImGui::GetIO().MousePos.x;
    const float position = (mouseX - innerMin.x) / (innerMax.x - innerMin.x);
    gUI.state.playbackPosition = std::clamp(position, 0.0f, 1.0f);
  }
  ImGui::EndChild();
}

void renderTimeline()
{
  ImGui::BeginChild("TimelinePanel", ImVec2(0.0f, timelineHeight()), true);
  drawSectionTitle("Timeline");

  float selectionStart = gUI.state.selectionStart;
  float selectionEnd = gUI.state.selectionEnd;

  ImGui::PushItemWidth(-1.0f);
  ImGui::SliderFloat("Position", &gUI.state.playbackPosition, 0.0f, 1.0f, "%.3f");
  ImGui::SliderFloat("Selection start", &selectionStart, 0.0f, 1.0f, "%.3f");
  ImGui::SliderFloat("Selection end", &selectionEnd, 0.0f, 1.0f, "%.3f");
  gUI.state.selectionStart = std::min(selectionStart, selectionEnd);
  gUI.state.selectionEnd = std::max(selectionStart, selectionEnd);
  ImGui::InputFloat("Cut seconds", &gUI.state.cutDurationSeconds, 0.1f, 1.0f, "%.2f");
  gUI.state.cutDurationSeconds = std::max(0.01f, gUI.state.cutDurationSeconds);
  ImGui::PopItemWidth();

  if (ImGui::Button("Trim to selection", ImVec2(160.0f, 0.0f)))
  {
    if (!gUI.fileLoaded)
    {
      setStatus("Open file before trim.");
    }
    else
    {
      try
      {
        gUI.equalizer.trimToRange(gUI.state.selectionStart, gUI.state.selectionEnd);
        finishAudioProcessing("Trim done.");
      }
      catch (const std::exception& error)
      {
        setStatus(error.what());
      }
    }
  }

  ImGui::SameLine();
  if (ImGui::Button("Cut at position", ImVec2(150.0f, 0.0f)))
  {
    if (!gUI.fileLoaded)
    {
      setStatus("Open file before cut.");
    }
    else
    {
      try
      {
        gUI.equalizer.cutSegment(gUI.state.playbackPosition, gUI.state.cutDurationSeconds);
        finishAudioProcessing("Cut done.");
      }
      catch (const std::exception& error)
      {
        setStatus(error.what());
      }
    }
  }

  drawTimeLabel("Current:", currentTimeSeconds());
  ImGui::SameLine();
  drawTimeLabel("Total:", totalDurationSeconds());
  drawTimeRangeLabel(
    "Selection:",
    totalDurationSeconds() * gUI.state.selectionStart,
    totalDurationSeconds() * gUI.state.selectionEnd
  );

  ImGui::EndChild();
}

void renderEqSliders()
{
  ImGui::BeginChild("EqPanel", ImVec2(0.0f, 0.0f), true);
  drawSectionTitle("Equalizer");

  const float sliderWidth = 56.0f;
  const float sliderHeight = 150.0f;
  bool eqChanged = false;

  ImGui::BeginGroup();
  ImGui::TextUnformatted("High");
  eqChanged |= ImGui::VSliderFloat("##high", ImVec2(sliderWidth, sliderHeight), &gUI.state.highDb, -12.0f, 12.0f, "%.1f dB");
  ImGui::Text("%.1f dB", gUI.state.highDb);
  ImGui::EndGroup();

  ImGui::SameLine(0.0f, 18.0f);

  ImGui::BeginGroup();
  ImGui::TextUnformatted("Mid");
  eqChanged |= ImGui::VSliderFloat("##mid", ImVec2(sliderWidth, sliderHeight), &gUI.state.midDb, -12.0f, 12.0f, "%.1f dB");
  ImGui::Text("%.1f dB", gUI.state.midDb);
  ImGui::EndGroup();

  ImGui::SameLine(0.0f, 18.0f);

  ImGui::BeginGroup();
  ImGui::TextUnformatted("Low");
  eqChanged |= ImGui::VSliderFloat("##low", ImVec2(sliderWidth, sliderHeight), &gUI.state.lowDb, -12.0f, 12.0f, "%.1f dB");
  ImGui::Text("%.1f dB", gUI.state.lowDb);
  ImGui::EndGroup();

  if (eqChanged)
  {
    applyEqFromUiChange();
  }

  ImGui::Spacing();
  if (ImGui::Button("Apply EQ", ImVec2(0.0f, 0.0f)))
  {
    try
    {
      applyEqFromSliders();
    }
    catch (const std::exception& error)
    {
      setStatus(error.what());
    }
  }

  ImGui::EndChild();
}

void renderExtraPanel()
{
  ImGui::BeginChild("ExtraPanel", ImVec2(0.0f, 0.0f), true);
  drawSectionTitle("More controls");

  if (ImGui::Checkbox("Enable filter", &gUI.state.enableFilter))
  {
    applyEqFromUiChange();
  }

  const char* presets[] = {"Flat", "Bass Boost", "Vocal", "Treble Boost"};
  const int previousPreset = gUI.state.presetIndex;
  ImGui::Combo("Preset", &gUI.state.presetIndex, presets, IM_ARRAYSIZE(presets));
  if (previousPreset != gUI.state.presetIndex)
  {
    applyPresetToSliders();
    applyEqFromUiChange();
  }

  if (ImGui::Button("Reset EQ", ImVec2(120.0f, 0.0f)))
  {
    resetEqSliders();
    if (gUI.fileLoaded)
    {
      gUI.equalizer.resetChanges();
      syncHeaderInfo();
    }
    setStatus("EQ reset.");
  }

  ImGui::Spacing();
  if (ImGui::Button("Reverse", ImVec2(130.0f, 0.0f)))
  {
    try
    {
      gUI.equalizer.reverse();
      finishAudioProcessing("Reverse done.");
    }
    catch (const std::exception& error)
    {
      setStatus(error.what());
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("Invert phase", ImVec2(130.0f, 0.0f)))
  {
    try
    {
      gUI.equalizer.inversion();
      finishAudioProcessing("Invert done.");
    }
    catch (const std::exception& error)
    {
      setStatus(error.what());
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("Stereo to mono", ImVec2(130.0f, 0.0f)))
  {
    try
    {
      gUI.equalizer.StereoToMono();
      finishAudioProcessing("Mono done.");
    }
    catch (const std::exception& error)
    {
      setStatus(error.what());
    }
  }

  ImGui::Spacing();
  ImGui::TextUnformatted("Save in File -> Save As.");
  ImGui::Dummy(ImVec2(0.0f, 80.0f));

  ImGui::EndChild();
}

void renderEqualizerUI()
{
  renderTopMenu();

  const ImGuiViewport* viewport = ImGui::GetMainViewport();
  const float menuBarHeight = ImGui::GetFrameHeight();
  const ImVec2 workPos(viewport->WorkPos.x, viewport->WorkPos.y + menuBarHeight);
  const ImVec2 workSize(viewport->WorkSize.x, viewport->WorkSize.y - menuBarHeight);

  ImGui::SetNextWindowPos(workPos, ImGuiCond_Always);
  ImGui::SetNextWindowSize(workSize, ImGuiCond_Always);

  const ImGuiWindowFlags flags =
    ImGuiWindowFlags_NoDecoration |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoSavedSettings;

  ImGui::Begin("EqualizerRoot", nullptr, flags);

  const float leftWidth = leftColumnWidth();
  const float bottomHeight = eqPanelHeight();
  const float timelinePanelHeight = timelineHeight();
  const float spacing = ImGui::GetStyle().ItemSpacing.x;

  ImGui::BeginChild("LeftColumn", ImVec2(leftWidth, 0.0f), false);
  renderPlaybackControls();
  renderInfoPanel();
  ImGui::EndChild();

  ImGui::SameLine();

  ImGui::BeginGroup();
  const float rightWidth = ImGui::GetContentRegionAvail().x;
  const float topHeight = ImGui::GetContentRegionAvail().y - bottomHeight - spacing;
  const float waveformHeight = topHeight - timelinePanelHeight - spacing;

  ImGui::BeginChild("WaveformArea", ImVec2(rightWidth, waveformHeight), false);
  renderWaveformPanel();
  ImGui::EndChild();

  ImGui::BeginChild("TimelineArea", ImVec2(rightWidth, timelinePanelHeight), false);
  renderTimeline();
  ImGui::EndChild();

  const float eqWidth = 220.0f;
  ImGui::BeginChild("BottomLeftArea", ImVec2(eqWidth, bottomHeight), false);
  renderEqSliders();
  ImGui::EndChild();

  ImGui::SameLine();

  ImGui::BeginChild("BottomRightArea", ImVec2(0.0f, bottomHeight), false);
  renderExtraPanel();
  ImGui::EndChild();
  ImGui::EndGroup();

  ImGui::End();
}
