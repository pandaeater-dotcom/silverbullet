// audio_tool.cc
#include "audio_tool.h"
#include <fstream>
#include <iostream>
#include <lame/lame.h>
#include <mpg123.h>
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

  if (mpg123_init() != MPG123_OK) {
    cerr << "Error initializing mpg123 library for reading file" << endl;
    return;
  }
  mh = mpg123_new(nullptr, nullptr);
  if (!mh) {
    cerr << "Unable to create MP3 handle" << endl;
    throw;
  }
}

AudioTool::AudioTool(string inputPath, string outputPath, string outputType)
    : inputFilePath{inputPath}, outputDir(outputPath) {
  if (!fs::is_regular_file(inputPath)) {
    throw;
  }
  determineType(fs::path(inputFilePath).extension().string(), &inputFormat);
  determineType(outputType, &outputFormat);
}

AudioTool::~AudioTool() {
  if (mh != nullptr) {
    mpg123_close(mh);
    mpg123_delete(mh);
  }
  mpg123_exit();
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

  bitDepth = mpg123_encsize(encoding) * 8;
  size_t bufferSize = mpg123_outblock(mh);
  audioData.resize(bufferSize);
  vector<unsigned char> buffer(bufferSize);
  int readResult;


  while ((readResult = mpg123_read(mh, buffer.data(), bufferSize, &done)) == MPG123_OK) {
    audioData.insert(audioData.end(), buffer.begin(), buffer.begin() + done);
  }

  if (readResult != MPG123_DONE) {
    cerr << "Error reading MP3: " << mpg123_strerror(mh) << std::endl;
  }
}

void AudioTool::writeMP3() {
  lame_t lame = lame_init();
  if (!lame) {
    cerr << "Failed to initialize LAME encoder" << endl;
  }

  lame_set_in_samplerate(lame, sampleRate);
  lame_set_num_channels(lame, channels);
  lame_set_brate(lame, 128);
  lame_set_quality(lame, 1);

  if (lame_init_params(lame) < 0) {
    cerr << "LAME parameter intialization failed" << endl;
    lame_close(lame);
    return;
  }

  string outputPath = outputDir + "outputaudio.mp3";
  std::ofstream outfile(outputPath, std::ios::binary);
  if (!outfile) {
    throw std::runtime_error("Unable to open output file");
  }

  const int PCM_SIZE = 8192;
  const int MP3_SIZE = 8192;

  std::vector<short int> pcmBuffer(PCM_SIZE * 2);
  std::vector<unsigned char> mp3Buffer(MP3_SIZE);

  size_t pcmSamples = audioData.size() / (channels * (bitDepth / 8));
  const unsigned char* audioPtr = audioData.data();

  while (pcmSamples > 0) {
    size_t samplesToProcess = std::min(static_cast<size_t>(PCM_SIZE), pcmSamples);

    for (size_t i = 0; i < samplesToProcess * channels; ++i) {
      pcmBuffer[i] = *reinterpret_cast<const short*>(audioPtr);
      audioPtr += 2;
    }
    
    int written;
    if (channels == 2) {
      written = lame_encode_buffer_interleaved(lame, pcmBuffer.data(),samplesToProcess, mp3Buffer.data(), MP3_SIZE);
    } else if (channels == 1) {
      written = lame_encode_buffer(lame, pcmBuffer.data(), nullptr, samplesToProcess, mp3Buffer.data(), MP3_SIZE);
    }

    if (written < 0) {
      throw std::runtime_error("LAME encoding failed");
    }

    outfile.write(reinterpret_cast<char*>(mp3Buffer.data()), written);
    pcmSamples -= samplesToProcess;
  }

  int final_written = lame_encode_flush(lame, mp3Buffer.data(), MP3_SIZE);
  if (final_written > 0) {
    outfile.write(reinterpret_cast<char*>(mp3Buffer.data()), final_written);
  }


  lame_close(lame);
  outfile.close();
}
void AudioTool::writeWAV() {
  string outputPath = outputDir + "outputaudio.wav";
  std::ofstream outfile(outputPath, std::ios::binary);
  if (!outfile) {
    throw std::runtime_error("Unable to open output WAV file");
  }

  // Write WAV header
  writeWAVHeader(outfile);

  // Write audio data
  outfile.write(reinterpret_cast<const char*>(audioData.data()), audioData.size());

  // Update file size in header
  updateWAVHeader(outfile);

  outfile.close();
}
void AudioTool::writeWAVHeader(std::ofstream& outfile) {
  // RIFF chunk
  outfile.write("RIFF", 4);
  write32BitLE(outfile, 0); // File size (to be updated later)
  outfile.write("WAVE", 4);

  // Format chunk
  outfile.write("fmt ", 4);
  write32BitLE(outfile, 16); // Format chunk size
  write16BitLE(outfile, 1); // Audio format (1 for PCM)
  write16BitLE(outfile, channels);
  write32BitLE(outfile, sampleRate);
  write32BitLE(outfile, sampleRate * channels * (bitDepth / 8)); // Byte rate
  write16BitLE(outfile, channels * (bitDepth / 8)); // Block align
  write16BitLE(outfile, bitDepth);

  // Data chunk
  outfile.write("data", 4);
  write32BitLE(outfile, audioData.size()); // Data size
}

void AudioTool::updateWAVHeader(std::ofstream& outfile) {
  outfile.seekp(4, std::ios::beg);
  write32BitLE(outfile, 36 + audioData.size()); // File size
}

void AudioTool::write16BitLE(std::ofstream& outfile, uint16_t value) {
  outfile.put(value & 0xFF);
  outfile.put((value >> 8) & 0xFF);
}

void AudioTool::write32BitLE(std::ofstream& outfile, uint32_t value) {
  outfile.put(value & 0xFF);
  outfile.put((value >> 8) & 0xFF);
  outfile.put((value >> 16) & 0xFF);
  outfile.put((value >> 24) & 0xFF);
}
