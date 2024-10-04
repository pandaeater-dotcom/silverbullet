#include "image_tool.h"
#include <pngconf.h>

using namespace std;

Image8Bit::Image8Bit(string inputPath) : ImageTool(inputPath) {}

Image8Bit::Image8Bit(string inputPath, string outputPath)
    : ImageTool(inputPath, outputPath) {}

void Image8Bit::readJPEG() {
  FILE *infile;
  try {
    infile = openFile(inputFilePath, READ);
  } catch (...) {
    return;
  }

  struct jpeg_decompress_struct cinfo;
  struct JPEGErrorHandler jerr;
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = &JPEGErrorExit;

  if (setjmp(jerr.setjmp_buffer)) {
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return;
  }

  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, infile);
  jpeg_read_header(&cinfo, true);
  cinfo.out_color_space = JCS_RGB;

  jpeg_start_decompress(&cinfo);

  bitDepth = cinfo.data_precision;
  width = cinfo.output_width;
  height = cinfo.output_height;
  channels = cinfo.output_components;

  imageData.resize(height * width * channels);

  while (cinfo.output_scanline < height) {
    unsigned char *rowPtr =
        &imageData[cinfo.output_scanline * width * channels];
    jpeg_read_scanlines(&cinfo, &rowPtr, 1);
  }

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  fclose(infile);
  cout << "read file" << endl;
}

void Image8Bit::writeJPEG(int quality) const {
  string outputPath = outputDir + "outputimage.jpg";
  FILE *outfile;

  try {
    outfile = openFile(outputPath, WRITE);
  } catch (...) {
    return;
  }

  struct jpeg_compress_struct cinfo;

  struct JPEGErrorHandler jerr;
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = &JPEGErrorExit;

  if (setjmp(jerr.setjmp_buffer)) {
    jpeg_destroy_compress(&cinfo);
    fclose(outfile);
    return;
  }

  jpeg_create_compress(&cinfo);
  jpeg_stdio_dest(&cinfo, outfile);

  cinfo.image_width = width;
  cinfo.image_height = height;
  cinfo.input_components = channels;
  cinfo.in_color_space = channels == 3 ? JCS_RGB : JCS_EXT_RGBA;
  cout << "num channels: " << channels << endl;

  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, quality, TRUE);
  cout << "here" << endl;
  jpeg_start_compress(&cinfo, TRUE);

  while (cinfo.next_scanline < height) {
    JSAMPROW rowPtr =
        (JSAMPROW)&imageData[cinfo.next_scanline * width * channels];
    jpeg_write_scanlines(&cinfo, &rowPtr, 1);
  }

  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);
  fclose(outfile);

  cout << "wrote image" << endl;
}

void Image8Bit::readPNG() {
  FILE *infile;
  try {
    infile = openFile(inputFilePath, READ);
  } catch (...) {
    return;
  }

  png_structp png =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, PNGErrorExit, NULL);
  if (!png) {
    cerr << "Failed into create png structure" << endl;
    fclose(infile);
    return;
  }

  png_infop info = png_create_info_struct(png);
  if (!info) {
    cerr << "Failed to create png info structure" << endl;
    png_destroy_read_struct(&png, nullptr, nullptr);
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
  ImageTool::channels = png_get_channels(png, info);
  png_byte colorType = png_get_color_type(png, info);
  ImageTool::bitDepth = png_get_bit_depth(png, info);

  if (colorType == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png);

  if (png_get_valid(png, info, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png);

  if (colorType == PNG_COLOR_TYPE_GRAY && ImageTool::bitDepth < 8)
    png_set_expand_gray_1_2_4_to_8(png);

  if (bitDepth == 16) {
    png_set_strip_16(png);
  }

  png_read_update_info(png, info);

  imageData.resize(width * height * channels);

  vector<png_bytep> rowPtrs(height);
  for (int i = 0; i < height; ++i) {
    rowPtrs[i] = &imageData[i * width * channels];
  }

  png_read_image(png, rowPtrs.data());

  png_destroy_read_struct(&png, &info, nullptr);
  fclose(infile);
  cout << "read png" << endl;
}

void Image8Bit::writePNG() const {
  string outputPath = outputDir + "outputimage.png";
  FILE *outfile;
  try {
    outfile = openFile(outputPath, WRITE);
  } catch (...) {
    return;
  }

  png_structp png =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, PNGErrorExit, NULL);
  if (!png) {
    cerr << "Failed into create png structure" << endl;
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
    png_destroy_read_struct(&png, &info, nullptr);
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

  for (int i = 0; i < ImageTool::height; ++i) {
    rowPtrs[i] = const_cast<png_bytep>(&imageData[i * width * channels]);
  }

  png_write_image(png, rowPtrs);
  png_write_end(png, nullptr);

  png_destroy_write_struct(&png, &info);
  fclose(outfile);

  cout << "Wrote the png succesfully?" << endl;
}
