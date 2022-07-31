#include "imageswindow.h"

#include <QApplication>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (argc<2) {
        cout << "usage: ./imagesAligner images.txt" << endl;
        exit(1);
    }
    ImagesWindow w(argv[1]);
    w.show();
    return a.exec();
}
