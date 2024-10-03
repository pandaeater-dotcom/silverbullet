#include "silverbullet.h"
#include "image_tool.h"


int main(int argc, char* argv[]) {
    ImageTool processor("sample/neil-and-zulma-scott-pHztxoXCvAw-unsplash.jpg");
    processor.readJPEG();
    processor.writePNG();
}
