// image_tool.h
#include "itool.h"
#include <csetjmp>
#include <filesystem>
#include <iostream>
#include <jpeglib.h>
#include <png.h>
#include <pngconf.h>
#include <vector>

enum ImgFormat { JPEG, PNG };

void JPEGErrorExit(j_common_ptr);
void PNGErrorExit(png_structp png, png_const_charp err_msg);

struct JPEGErrorHandler {
  jpeg_error_mgr pub;
  jmp_buf setjmp_buffer;
};

class ImageTool : public ITool {
private:
  bool determineType(string, ImgFormat *);

  string inputFilePath;
  string outputDir;
  unsigned int width, height, channels;
  int bitDepth;
  ImgFormat inputFormat;
  ImgFormat outputFormat;
  vector<unsigned short> imageData;

  FILE *openFile(string, FileMode) const;

public:
  ImageTool(string, string);
  ImageTool(string, string, string);

  void readJPEG();
  void writeJPEG(int) const;
  void readPNG();
  void writePNG() const;

  void execute(const vector<string> &args) override;
  string getDescription() const override;
};
