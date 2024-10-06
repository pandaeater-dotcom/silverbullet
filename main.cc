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
  AudioTool atool("sample/sample-6s.mp3", "MP3");
  atool.readMP3();
}
