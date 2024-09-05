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
    ImagesWindow(const char *imagesFileName, unsigned int firstIndex=0, const char* crop_file_name = nullptr, QWidget *parent = nullptr);
    ~ImagesWindow();
protected:
    bool loadCropFile();
    void saveCropFile();
    void readImagesFile();
    void initCrops(int cropset=0);
    void calcCenterAndAngle();
    void showTransformationData();
    void imageConverter(QImage &img, int cx, int cy, double angle);
    QList<QStringList> imageReperCoordinates;
    QString imagesPath;
    QString currentFileName, cropFileName;
    double centerx,centery;
    double angle;
    double referenceAngle;
protected:
    void mousePressEvent(QMouseEvent *mouse);
    void showImage();
    int mousePressCounter;
    QLabel *cropText;
    QLabel *zoomText;
    QSpinBox *zoomSelect;
    QSpinBox *cropxSpin,*cropySpin,*cropwSpin,*crophSpin;
    QPushButton *nextImage, *prevImage;
    QPushButton *savePoints;
    QPushButton *alignAllImages;
    QScrollArea *viewImage;
    QLabel *imageLabel;
    int currentIndex;
    int zoom;
protected slots:
    void showNextImage();
    void showPrevImage();
    void saveReferencePoints();
    void alignImages();
    void changeZoom(int value);
};
#endif // IMAGESWINDOW_H
