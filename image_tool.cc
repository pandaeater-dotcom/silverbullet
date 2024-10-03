// image_tool.cc
#include "image_tool.h"
#include <csetjmp>
#include <cstdio>
#include <iostream>
#include <jpeglib.h>
#include <png.h>
#include <pngconf.h>
#include <string>

ImageTool::ImageTool(string inputPath) : inputFilePath{inputPath} {
  ImageTool::outputDir = inputPath.substr(0, inputPath.find_last_of('/') + 1);
}

ImageTool::ImageTool(string inputPath, string outputPath)
    : inputFilePath{inputPath}, outputDir(outputPath) {}

void ImageTool::readJPEG() {
  FILE *infile = fopen(inputFilePath.c_str(), "rb");

  if (!infile) {
    cerr << "Error! File could not be read" << endl;
    return;
  }

  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);

  jpeg_stdio_src(&cinfo, infile);

  jpeg_read_header(&cinfo, true);
  ImageTool::bitDepth = cinfo.data_precision;

  jpeg_start_decompress(&cinfo);

  ImageTool::width = cinfo.output_width;
  ImageTool::height = cinfo.output_height;
  ImageTool::channels = cinfo.output_components;

  if (ImageTool::bitDepth == 8) {
    ImageTool::imageData8Bit.resize(
        height, vector<unsigned char>(ImageTool::width * ImageTool::channels));
    while (cinfo.output_scanline < height) {
      unsigned char *rowPtr =
          ImageTool::imageData8Bit[cinfo.output_scanline].data();
      jpeg_read_scanlines(&cinfo, &rowPtr, 1);
    }
  } else if (ImageTool::bitDepth > 8 && ImageTool::bitDepth <= 16) {
    ImageTool::imageData16Bit.resize(
        height, vector<unsigned short>(ImageTool::width * ImageTool::channels));
    while (cinfo.output_scanline < height) {
      unsigned short *rowPtr =
          ImageTool::imageData16Bit[cinfo.output_scanline].data();
      jpeg16_read_scanlines(&cinfo, &rowPtr, 1);
    }
  }

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  fclose(infile);
}

void ImageTool::writeJPEG() {
  string outputPath = ImageTool::outputDir + "outputimage.jpg";
  FILE *outfile = fopen(outputPath.c_str(), "wb");

  if (!outfile) {
    cerr << "Could not open file for output" << endl;
    return;
  }

  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
}

void ImageTool::readPNG() {
  FILE *infile = fopen(ImageTool::inputFilePath.c_str(), "rb");

  if (!infile) {
    cerr << "Could not read input file" << endl;
    return;
  }

  png_structp png =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info = png_create_info_struct(png);
  if (!info) {
    cerr << "Failed to create png info structure" << endl;
    png_destroy_write_struct(&png, nullptr);
    fclose(infile);
    return;
  }

  if (setjmp(png_jmpbuf(png))) {
    cerr << "Error encountered during png creation" << endl;
    png_destroy_read_struct(&png, &info, nullptr);
    fclose(infile);
    return;
  }

  png_init_io(png, infile);

  png_read_info(png, info);
  ImageTool::width = png_get_image_width(png, info);
  ImageTool::height = png_get_image_height(png, info);
  png_byte colorType = png_get_color_type(png, info);
  ImageTool::bitDepth = png_get_bit_depth(png, info);

  if (colorType == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png);

  if (png_get_valid(png, info, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png);

  if (colorType == PNG_COLOR_TYPE_GRAY && ImageTool::bitDepth < 8)
    png_set_expand_gray_1_2_4_to_8(png);

  png_read_update_info(png, info);

  ImageTool::channels = png_get_channels(png, info);

  if (ImageTool::bitDepth == 8) {
    ImageTool::imageData8Bit.resize(height, vector<unsigned char>(width * channels));
  } else if (ImageTool::bitDepth > 8 && ImageTool::bitDepth <= 16) {
    ImageTool::imageData16Bit.resize(height, vector<unsigned short>(width * channels));
  } else {
    cerr << "Invalid or unsupported bit depth" << endl;
    png_destroy_read_struct(&png, &info, nullptr);
    fclose(infile);
    return;
  }

  vector<png_bytep> rowPtrs(height);
  if (ImageTool::bitDepth == 8) {
    for (int i = 0; i < ImageTool::height; ++i) {
      rowPtrs[i] = ImageTool::imageData8Bit[i].data();
    }
  } else {
    for (int i = 0; i < ImageTool::height; ++i) {
      rowPtrs[i] = (png_bytep) ImageTool::imageData16Bit[i].data();
    }
  }
  
  png_read_image(png, rowPtrs.data());
  png_destroy_read_struct(&png, &info,nullptr);
  fclose(infile);
}

void ImageTool::writePNG() {
  string outputPath = ImageTool::outputDir + "outputimage.png";
  FILE *outfile = fopen(outputPath.c_str(), "wb");
  if (!outfile) {
    cerr << "Error opening file to write output" << endl;
    return;
  }

  png_structp png =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) {
    cerr << "Failed to create png structure" << endl;
    fclose(outfile);
    return;
  }

  png_infop info = png_create_info_struct(png);
  if (!info) {
    cerr << "Failed to create png info structure" << endl;
    png_destroy_write_struct(&png, nullptr);
    fclose(outfile);
    return;
  }

  if (setjmp(png_jmpbuf(png))) {
    cerr << "Error encountered during png creation" << endl;
    png_destroy_write_struct(&png, &info);
    fclose(outfile);
    return;
  }

  png_init_io(png, outfile);

  int colorType = channels == 1   ? PNG_COLOR_TYPE_GRAY
                  : channels == 3 ? PNG_COLOR_TYPE_RGB
                                  : PNG_COLOR_TYPE_RGBA;
  png_set_IHDR(png, info, ImageTool::width, ImageTool::height,
               ImageTool::bitDepth, colorType, PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

  png_write_info(png, info);

  png_bytep rowPtrs[height];

  if (ImageTool::bitDepth == 8) {
    for (int i = 0; i < ImageTool::height; ++i) {
      rowPtrs[i] = ImageTool::imageData8Bit[i].data();
    }
  } else {
    for (int i = 0; i < ImageTool::height; ++i) {
      rowPtrs[i] = (png_bytep)ImageTool::imageData16Bit[i].data();
    }
  }

  png_write_image(png, rowPtrs);
  png_write_end(png, nullptr);

  png_destroy_write_struct(&png, &info);
  fclose(outfile);

  cout << "Wrote the png succesfully?" << endl;
}

void ImageTool::execute(const vector<string> &args) {
  // Implementation for execute
}

string ImageTool::getDescription() const {
  return "ImageTool for handling JPEG files.";
}
