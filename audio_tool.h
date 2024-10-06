// audio_tool.h

#include "itool.h"
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

  vector<float> audioData;

public:
  AudioTool(string, string);
  AudioTool(string, string, string);

  void readMP3();
  void writeMP3();
  void writeWAV();
}; 
