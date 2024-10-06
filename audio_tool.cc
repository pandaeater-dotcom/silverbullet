// audio_tool.cc
#include "audio_tool.h"
#include <iostream>
#include <mpg123.h>

namespace fs = filesystem;

AudioTool::AudioTool(string inputPath, string outputType) : inputFilePath{inputPath} {
  if (!fs::is_regular_file(inputPath)) {
    throw;
  }
  determineType(fs::path(inputFilePath).extension().string(), &inputFormat);
  outputDir = inputPath.substr(0, inputPath.find_last_of('/') + 1);
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

  unsigned char* buffer;
  size_t done;
  int channels, encoding;
  long rate;

  if (mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK) {
    cerr << "Error getting the MP3 format" << endl;
    mpg123_close(mh);
    mpg123_delete(mh);
    return;
  }

  size_t bufferSize = mpg123_outblock(mh);
  audioData.resize(bufferSize);
  int readResult;
  while ((readResult = mpg123_read(mh, buffer, bufferSize, &done)) == MPG123_OK) {
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
