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
  int channels, encoding; 
  long sampleRate;
  long bitRate;
  AudFormat inputFormat;
  AudFormat outputFormat;

  vector<unsigned char> audioData;
  void determineType(string, AudFormat*);
public:
  AudioTool(string, string);
  AudioTool(string, string, string);

  void readMP3();
  void writeMP3() const;
  void readWAV();
  void writeWAV() const;

  void execute(const vector<string> &args) override;
  string getDescription() const override;

}; 
