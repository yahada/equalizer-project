#ifndef EQUALIZER_UI_HPP
#define EQUALIZER_UI_HPP

struct EqualizerUIState {
  bool isPlaying = false;
  float playbackPosition = 0.28f;
  float selectionStart = 0.10f;
  float selectionEnd = 0.90f;
  float cutDurationSeconds = 1.0f;
  float highDb = 0.0f;
  float midDb = 0.0f;
  float lowDb = 0.0f;
  bool enableFilter = true;
  int presetIndex = 0;
};

void renderEqualizerUI();
void renderTopMenu();
void renderPlaybackControls();
void renderInfoPanel();
void renderWaveformPanel();
void renderTimeline();
void renderEqSliders();
void renderExtraPanel();

#endif
