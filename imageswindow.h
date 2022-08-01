#ifndef IMAGESWINDOW_H
#define IMAGESWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QStringList>
#include <QImage>
#include <QPushButton>
#include <QScrollArea>
#include <QLabel>

class ImagesWindow : public QMainWindow
{
    Q_OBJECT
public:
    ImagesWindow(const char *imagesFileName, QWidget *parent = nullptr);
    ~ImagesWindow();
protected:
    void readImagesFile();
    void calcCenterAndAngle();
    void showTransformationData();
    void imageConverter(QImage &img, int cx, int cy, double angle);
    QList<QStringList> imageReperCoordinates;
    QString imagesPath;
    QString currentFileName;
    double centerx,centery;
    int cropx,cropy;
    int cropw,croph;
    double angle;
protected:
    void mousePressEvent(QMouseEvent *mouse);
    int mousePressCounter;
    QPushButton *nextImage;
    QPushButton *savePoints;
    QPushButton *alignAllImages;
    QScrollArea *viewImage;
    QLabel *imageLabel;
    int currentIndex;
protected slots:
    void showNextImage();
    void saveReferencePoints();
    void alignImages();

};
#endif // IMAGESWINDOW_H
