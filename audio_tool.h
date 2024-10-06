// audio_tool.h
#include "itool.h"
#include <filesystem>

enum AudFormat {
  MP3,
  WAV
};

class AudioTool : public ITool {
private:
  string inputFilePath;
  string outputDir;
  AudFormat inputFormat;
  AudFormat outputFormat;

  vector<unsigned char> audioData;
  void determineType(string, AudFormat*);
public:
  AudioTool(string, string);
  AudioTool(string, string, string);

  void readMP3();
  void writeMP3();
  void writeWAV();
}; 
