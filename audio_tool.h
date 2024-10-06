// audio_tool.h
#include "itool.h"
#include <cstdint>
#include <filesystem>
#include <mpg123.h>

enum AudFormat { MP3, WAV };

class AudioTool : public ITool {
private:
  mpg123_handle *mh;
  string inputFilePath;
  string outputDir;
  int channels, encoding;
  long sampleRate;
  int bitDepth;
  AudFormat inputFormat;
  AudFormat outputFormat;

  vector<unsigned char> audioData;
  void determineType(string, AudFormat *);

  void writeWAVHeader(ofstream&);
  void updateWAVHeader(ofstream&);
  void write16BitLE(ofstream&, uint16_t);
  void write32BitLE(ofstream&, uint32_t);

public:
  AudioTool(string, string);
  AudioTool(string, string, string);

  ~AudioTool();

  void readMP3();
  void writeMP3();
  void readWAV();
  void writeWAV();

  void execute(const vector<string> &args) override;
  string getDescription() const override;
};
