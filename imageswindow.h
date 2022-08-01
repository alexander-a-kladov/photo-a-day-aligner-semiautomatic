#ifndef IMAGESWINDOW_H
#define IMAGESWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QStringList>
#include <QImage>
#include <QPushButton>
#include <QScrollArea>
#include <QLabel>
#include <QSpinBox>

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
    void updateCrops();
    QList<QStringList> imageReperCoordinates;
    QString imagesPath;
    QString currentFileName;
    double centerx,centery;
    int cropx,cropy;
    int cropw,croph;
    int cropset;
    double angle;
    double referenceAngle;
protected:
    void mousePressEvent(QMouseEvent *mouse);
    int mousePressCounter;
    QLabel *cropText;
    QLabel *zoomText;
    QSpinBox *zoomSelect;
    QSpinBox *cropxSpin,*cropySpin,*cropwSpin,*crophSpin;
    QPushButton *nextImage;
    QPushButton *savePoints;
    QPushButton *alignAllImages;
    QPushButton *cropSet;
    QScrollArea *viewImage;
    QLabel *imageLabel;
    int currentIndex;
    int zoom;
protected slots:
    void showNextImage();
    void saveReferencePoints();
    void alignImages();
    void changeCropSet();
    void changeZoom(int value);
};
#endif // IMAGESWINDOW_H
