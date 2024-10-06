// image_tool.cc
#include "image_tool.h"
#include <unordered_set>

namespace fs = filesystem;

ImageTool::ImageTool(string inputPath, string outputType) : inputFilePath{inputPath} {
  if (!fs::is_regular_file(inputPath)) {
    throw;
  }
  determineType(fs::path(inputFilePath).extension().string(), &inputFormat);
  outputDir = inputPath.substr(0, inputPath.find_last_of('/') + 1);
  determineType(outputType, &outputFormat); 
}

ImageTool::ImageTool(string inputPath, string outputPath, string outputType)
    : inputFilePath{inputPath}, outputDir(outputPath) {
  if (!fs::is_regular_file(inputPath)) {
    throw;
  }
  determineType(fs::path(inputFilePath).extension().string(), &inputFormat);
  determineType(outputType, &outputFormat);
}

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

bool ImageTool::determineType(string format, ImgFormat* var) {
  unordered_set<string> validJPEG = {"jpeg", "jpg", ".jpeg", ".jpg", "JPEG", ".JPG"};
  unordered_set<string> validPNG = {"PNG", "png", ".png"};
  if (validJPEG.find(format) != validJPEG.end()) {
    *var = JPEG;
  } else if (validPNG.find(format) != validPNG.end()) {
    *var = PNG;
  } else {
    return false;
  }
  return true;
}

