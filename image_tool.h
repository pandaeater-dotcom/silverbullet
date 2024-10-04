// image_tool.h
#include "itool.h"
#include <csetjmp>
#include <jpeglib.h>
#include <png.h>
#include <pngconf.h>
#include <vector>
#include <iostream>

enum FileMode { READ, WRITE };

void JPEGErrorExit(j_common_ptr);
void PNGErrorExit(png_structp png, png_const_charp err_msg);

struct JPEGErrorHandler {
  jpeg_error_mgr pub;
  jmp_buf setjmp_buffer;
};

class ImageTool : public ITool {
protected:
  string inputFilePath;
  string outputDir;
  unsigned int width, height, channels;
  int bitDepth;

  FILE *openFile(string, FileMode) const;

public:
  ImageTool(string inputFilePath, string outputDir);
  ImageTool(string inputFilePath);

  virtual ~ImageTool() = default;

  virtual void readJPEG() = 0;
  virtual void writeJPEG(int) const = 0;
  virtual void readPNG() = 0;
  virtual void writePNG() const = 0;

  void execute(const vector<string> &args) override;
  string getDescription() const override;
};

class Image8Bit : public ImageTool {
private:
  vector<unsigned char> imageData;

public:
  Image8Bit(string inputFilePath, string outputDir);
  Image8Bit(string inputFilePath);

  void readJPEG() override;
  void writeJPEG(int) const override;
  void readPNG() override;
  void writePNG() const override;
};


