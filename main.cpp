#include "imageswindow.h"

#include <QApplication>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    int currentIndex=0;
    if (argc<2) {
        cout << "usage: ./imagesAligner images.txt" << endl;
        exit(1);
    }
    if (argc>2)
        currentIndex=atoi(argv[2])-1;
    ImagesWindow w(argv[1],currentIndex);
    w.show();
    return a.exec();
}
