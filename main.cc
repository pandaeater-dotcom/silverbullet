#include "image_tool.h"
#include "audio_tool.h"
#include "silverbullet.h"

int main(int argc, char *argv[]) {
  //ImageTool processor("sample/inputimage.jpg", "JPG");
  ImageTool processor("sample/PNG_transparency_demonstration_1.png", "PNG");
  processor.readPNG();
  // processor.writePNG();
  //processor.readJPEG();
  processor.writeJPEG(100);
  AudioTool atool("sample/gs-16b-1c-44100hz.mp3", "WAV");
  atool.readMP3();
  //atool.writeMP3();
  atool.writeWAV();
}
