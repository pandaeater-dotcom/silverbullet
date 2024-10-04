#include "silverbullet.h"
#include "image_tool.h"


int main(int argc, char* argv[]) {
    //ImageTool processor("sample/neil-and-zulma-scott-pHztxoXCvAw-unsplash.jpg");
    Image8Bit processor("sample/PNG_transparency_demonstration_1.png");
    processor.readPNG();
    processor.writePNG();
    //processor.readPNG();
    //processor.writeJPEG(100);
}
