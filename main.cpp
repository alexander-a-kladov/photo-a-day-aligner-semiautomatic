#include "imageswindow.h"

#include <QApplication>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    const char* crop_file_name;
    QApplication a(argc, argv);
    int currentIndex=0;
    if (argc<2) {
        cout << "usage: ./imagesAligner images.txt" << endl;
        exit(1);
    }
    if (argc>2)
        currentIndex=atoi(argv[2])-1;
    if (argc>3)
        crop_file_name = argv[3];
    else
        crop_file_name = nullptr;
    ImagesWindow w(argv[1],currentIndex,crop_file_name);
    w.show();
    return a.exec();
}
