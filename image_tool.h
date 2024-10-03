// image_tool.h
#include "itool.h"
#include <jpeglib.h>
#include <png.h>
#include <vector>

class ImageTool : public ITool {
private:
  string inputFilePath;
  string outputDir;
  unsigned int width, height, channels;
  int bitDepth;

  vector<vector<unsigned char>> imageData8Bit;
  vector<vector<unsigned short>> imageData16Bit;

public:
  ImageTool(string inputFilePath, string outputDir);
  ImageTool(string inputFilePath);
  void execute(const vector<string> &args) override;
  string getDescription() const override;

  void readJPEG();
  void writeJPEG();
  void readPNG();
  void writePNG();
};
