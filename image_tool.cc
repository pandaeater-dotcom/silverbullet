// image_tool.cc
#include "image_tool.h"

ImageTool::ImageTool(string inputPath) : inputFilePath{inputPath} {
  ImageTool::outputDir = inputPath.substr(0, inputPath.find_last_of('/') + 1);
}

ImageTool::ImageTool(string inputPath, string outputPath)
    : inputFilePath{inputPath}, outputDir(outputPath) {}

FILE *ImageTool::openFile(string filePath, FileMode fileMode) const {
  string mode = fileMode == READ ? "rb" : "wb";
  FILE *file = fopen(filePath.c_str(), mode.c_str());

  if (file != nullptr)
    return file;
  switch (errno) {
  case ENOENT:
    cerr << "File not found" << endl;
    throw;
  case EACCES:
    cerr << "Permission denied" << endl;
    throw;
  case EMFILE:
  case ENFILE:
    cerr << "Too many open files" << endl;
    throw;
  case EINVAL:
    cerr << "Invalid argument for mode" << endl;
    throw;
  default:
    cerr << "Unknown error while opening file" << endl;
    throw;
  }
}

void JPEGErrorExit(j_common_ptr cinfo) {
  auto *err_ptr = (JPEGErrorHandler *)cinfo->err;
  (*cinfo->err->output_message)(cinfo);
  longjmp(err_ptr->setjmp_buffer, 1);
}

void PNGErrorExit(png_structp png, png_const_charp err_msg) {
  cout << "PNG Error: " << err_msg << endl;
}

void ImageTool::execute(const vector<string> &args) {
  // Implementation for execute
}

string ImageTool::getDescription() const {
  return "ImageTool for handling JPEG files.";
}
