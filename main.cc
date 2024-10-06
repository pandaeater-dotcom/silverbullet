#include "silverbullet.h"
#include "image_tool.h"


int main(int argc, char* argv[]) {
    ImageTool processor("sample/neil-and-zulma-scott-pHztxoXCvAw-unsplash.jpg", "JPG");
    //ImageTool processor("sample/PNG_transparency_demonstration_1.png", "PNG");
    //processor.readPNG();
    //processor.writePNG();
    processor.readJPEG();
    processor.writeJPEG(100);
}
