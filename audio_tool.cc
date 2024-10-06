// audio_tool.cc
#include "audio_tool.h"
#include <iostream>
#include <mpg123.h>
#include <lame/lame.h>
#include <unordered_set>

namespace fs = filesystem;

AudioTool::AudioTool(string inputPath, string outputType)
    : inputFilePath{inputPath} {
  if (!fs::is_regular_file(inputPath)) {
    throw;
  }
  outputDir = inputPath.substr(0, inputPath.find_last_of('/') + 1);
  determineType(fs::path(inputFilePath).extension().string(), &inputFormat);
  determineType(outputType, &outputFormat);
}

AudioTool::AudioTool(string inputPath, string outputPath, string outputType)
    : inputFilePath{inputPath}, outputDir(outputPath) {
  if (!fs::is_regular_file(inputPath)) {
    throw;
  }
  determineType(fs::path(inputFilePath).extension().string(), &inputFormat);
  determineType(outputType, &outputFormat);
}

void AudioTool::execute(const vector<string> &args) {}

string AudioTool::getDescription() const { return ""; }

void AudioTool::determineType(string format, AudFormat *var) {
  unordered_set<string> validMP3 = {"MP3", "mp3", ".mp3"};
  unordered_set<string> validWAV = {"WAV", "wav", ".wav"};
  if (validMP3.find(format) != validMP3.end()) {
    *var = MP3;
  } else if (validWAV.find(format) != validWAV.end()) {
    *var = WAV;
  } else {
    cerr << "Invalid format" << endl;
    throw;
  }
}
void AudioTool::readMP3() {
  if (mpg123_init() != MPG123_OK) {
    cerr << "Error initializing mpg123 library for reading file" << endl;
    return;
  }

  mpg123_handle *mh = mpg123_new(NULL, NULL);
  if (mpg123_open(mh, inputFilePath.c_str()) != MPG123_OK) {
    cerr << "Error opening file" << std::endl;
    mpg123_delete(mh);
    return;
  }

  size_t done;

  if (mpg123_getformat(mh, &sampleRate, &channels, &encoding) != MPG123_OK) {
    cerr << "Error getting the MP3 format" << endl;
    mpg123_close(mh);
    mpg123_delete(mh);
    return;
  }

  // TODO: add the thing to get bitrate

    size_t bufferSize = mpg123_outblock(mh);
  audioData.resize(bufferSize);
  int readResult;
  while ((readResult = mpg123_read(mh, audioData.data(), bufferSize, &done)) ==
         MPG123_OK) {
    size_t curSize = audioData.size();
    if (done == bufferSize) {
      audioData.resize(curSize + bufferSize);
    }
  }

  if (readResult != MPG123_DONE) {
    cerr << "Error reading MP3: " << mpg123_strerror(mh) << std::endl;
  }

  mpg123_close(mh);
  mpg123_delete(mh);
  mpg123_exit();
}

void AudioTool::writeMP3() const {
  lame_t lame = lame_init();
  if (lame == NULL) {
    cerr << "Failed to initialize LAME encoder" << endl;
  }

  lame_set_in_samplerate(lame, sampleRate);
  lame_set_num_channels(lame, channels);
  lame_set_brate(lame, 128);

}
