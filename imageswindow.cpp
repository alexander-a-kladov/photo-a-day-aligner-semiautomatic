#include "imageswindow.h"
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <QTransform>
#include <QPainter>
#include <QFont>
#include <QScrollBar>
#include <QPixmap>
#include <QMessageBox>
#include <math.h>
#include <iostream>

using namespace std;

ImagesWindow::ImagesWindow(const char *imagesFileName, unsigned int firstIndex, QWidget *parent)
    : QMainWindow(parent)
{
    currentFileName = imagesFileName;
    readImagesFile();
    calcCenterAndAngle();
    showTransformationData();

    nextImage = new QPushButton(this);
    nextImage->setText("Next Image");
    nextImage->setGeometry(10,10,120,30);
    prevImage = new QPushButton(this);
    prevImage->setText("Prev Image");
    prevImage->setGeometry(150,10,120,30);
    savePoints = new QPushButton(this);
    savePoints->setText("Save Points");
    savePoints->setGeometry(300,10,150,30);
    alignAllImages = new QPushButton(this);
    alignAllImages->setText("Align Images");
    alignAllImages->setGeometry(500,10,150,30);

    zoom = 3;
    referenceAngle = 0.0;
    zoomText = new QLabel(this);
    zoomText->setText("Zoom:");
    zoomText->setGeometry(700,10,100,30);
    zoomSelect = new QSpinBox(this);
    zoomSelect->setMinimum(1);
    zoomSelect->setMaximum(5);
    zoomSelect->setValue(zoom);
    zoomSelect->setGeometry(805,10,40,30);

    cropText = new QLabel(this);
    cropText->setText("Crops:");
    cropText->setGeometry(860,10,50,30);
    cropxSpin = new QSpinBox(this);
    cropxSpin->setRange(0,1500);
    cropxSpin->setGeometry(915,10,80,30);
    cropySpin = new QSpinBox(this);
    cropySpin->setRange(0,1500);
    cropySpin->setGeometry(1000,10,80,30);
    cropwSpin = new QSpinBox(this);
    cropwSpin->setRange(0,1500);
    cropwSpin->setGeometry(1085,10,80,30);
    crophSpin = new QSpinBox(this);
    crophSpin->setRange(0,1500);
    crophSpin->setGeometry(1170,10,80,30);
    cropSet = new QPushButton(this);
    cropSet->setText("Change Crops");
    cropSet->setGeometry(1260,10,100,30);
    cropset = 1;
    changeCropSet();

    if (!firstIndex)
        currentIndex = 0;
    else
        currentIndex = firstIndex;
    mousePressCounter = 0;
    connect(nextImage,SIGNAL(clicked()),this,SLOT(showNextImage()));
    connect(prevImage,SIGNAL(clicked()),this,SLOT(showPrevImage()));
    connect(savePoints,SIGNAL(clicked()), this,SLOT(saveReferencePoints()));
    connect(alignAllImages,SIGNAL(clicked()),this,SLOT(alignImages()));
    connect(zoomSelect,SIGNAL(valueChanged(int)),this,SLOT(changeZoom(int)));
    connect(cropSet,SIGNAL(clicked()),this,SLOT(changeCropSet()));
    setFixedSize(1600,1000);
    viewImage = new QScrollArea(this);
    viewImage->setGeometry(0,50,1600,950);
    showImage();
}

void ImagesWindow::changeCropSet()
{
    cropset+=1;
    cropset%=2;
    if (cropset)
    { // Vertical photos
        cropxSpin->setValue(586);
        cropySpin->setValue(556);
        cropwSpin->setValue(1162);
        crophSpin->setValue(1095);
    } else { // Horizontal photos
        cropxSpin->setValue(200);
        cropySpin->setValue(210);
        cropwSpin->setValue(400);
        crophSpin->setValue(765);
    }
    updateCrops();
}

void ImagesWindow::mousePressEvent(QMouseEvent *mouse)
{
    if (currentIndex<0) return;
    if (mouse->y()<50) return;
    int x = (mouse->x()+viewImage->horizontalScrollBar()->value())*zoom;
    int y = (mouse->y()-viewImage->y()+viewImage->verticalScrollBar()->value())*zoom;
    QStringList tempList;
    tempList = imageReperCoordinates.at(currentIndex);
    tempList.replace(mousePressCounter*2+1,QString::number(x));
    tempList.replace(mousePressCounter*2+2,QString::number(y));
    imageReperCoordinates.replace(currentIndex,tempList);
    mousePressCounter++;
    mousePressCounter%=2;
    showImage();
}

void ImagesWindow::changeZoom(int value)
{
    zoom=value;
    if (currentIndex<0) return;
    showImage();
}

void ImagesWindow::showNextImage()
{
    currentIndex++;
    currentIndex%=imageReperCoordinates.size();
    showImage();
}

void ImagesWindow::showPrevImage()
{
    currentIndex--;
    if (currentIndex<0) currentIndex+=imageReperCoordinates.size();
    showImage();
}

void ImagesWindow::showImage()
{
    setWindowTitle(QString("imagesAligner %1 %2/%3").arg(imageReperCoordinates.at(currentIndex).at(0)).arg(currentIndex+1).arg(imageReperCoordinates.size()));
    QImage img;
    QString imageName;
    imageName = imagesPath+"/"+imageReperCoordinates.at(currentIndex).at(0);
    if (img.load(imageName)) {
        int x1,y1,x2,y2,xc,yc;
        updateCrops();
        imageLabel = new QLabel;
        QPixmap pixmap = QPixmap::fromImage(img);
        QPainter p(&pixmap);
        QFont font = p.font();
        font.setPointSize(144);
        p.setFont(font);
        p.setPen(Qt::green);
        p.drawText(QRect(img.size().width()/2,200,400,200),QString::number(currentIndex+1));
        x1 = imageReperCoordinates.at(currentIndex).at(1).toInt();
        y1 = imageReperCoordinates.at(currentIndex).at(2).toInt();
        x2 = imageReperCoordinates.at(currentIndex).at(3).toInt();
        y2 = imageReperCoordinates.at(currentIndex).at(4).toInt();
        p.save();
        p.setPen(QPen(Qt::black,6));
        p.drawLine(x1,y1,x2,y2);
        p.restore();
        p.setBrush(QBrush(Qt::red));
        p.drawEllipse(x1-10,y1-10,20,20);
        p.drawRect(x2-10,y2-10,20,20);
        xc = (x1+x2)/2;
        yc = (y1+y2)/2;
        QPolygon centerTriangle;
        centerTriangle.append(QPoint(xc-15,yc-15));
        centerTriangle.append(QPoint(xc+15,yc));
        centerTriangle.append(QPoint(xc-15,yc+15));
        p.drawPolygon(centerTriangle);
        p.setPen(Qt::blue);
        p.drawRect(centerx-5-cropx,centery-20-cropy,10,40);
        p.drawRect(centerx-20-cropx,centery-5-cropy,40,10);
        p.end();
        imageLabel->setPixmap(pixmap.scaled(QSize(pixmap.width()/zoom,pixmap.height()/zoom)));
        viewImage->setWidget(imageLabel);
    }
}

void ImagesWindow::saveReferencePoints()
{
    QFile refPoints(currentFileName);
    if (refPoints.open(QIODevice::ReadOnly)) {
        refPoints.copy(currentFileName+".bak");
        refPoints.close();
    }
    if (refPoints.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QTextStream out(&refPoints);
        out << imagesPath;
        out << "\n";
        for (int i=0;i<imageReperCoordinates.size();++i)
        {
            out << imageReperCoordinates.at(i).join(" ");
            out << "\n";
        }
        refPoints.close();
        QMessageBox::information(this,"Save Reference Points","Successful");
    }
}

void ImagesWindow::readImagesFile()
{
    QFile imagesFile(currentFileName);
    qDebug() << currentFileName;
    if (imagesFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&imagesFile);
        in >> imagesPath;
        qDebug() << imagesPath;
        while (!in.atEnd()) {
            QString imagesData;
            QStringList dataList;
            imagesData = in.readLine();
            qDebug() << imagesData;
            dataList = imagesData.split(' ');
            qDebug() << "dataList.size = " << dataList.size();
            if (dataList.size() == 5)
                imageReperCoordinates.append(dataList);
        }
        if (!imageReperCoordinates.size()) {
            QDir dir;
            QStringList filters;
            filters << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp";
            dir.setPath(imagesPath);
            dir.setNameFilters(filters);
            dir.setSorting(QDir::Name);
            foreach (QFileInfo file, dir.entryInfoList()) {
                QStringList dataList;
                dataList << file.fileName() << "0" << "0" << "0" << "0";
                imageReperCoordinates.append(dataList);
            }
        }

        imagesFile.close();
    }

}

void ImagesWindow::calcCenterAndAngle()
{
    centerx = 0.0;
    centery = 0.0;
    angle = 0.0;
    qDebug() << "size " << imageReperCoordinates.size();
    for (int i=0;i<imageReperCoordinates.size();++i)
    {
        centerx += (imageReperCoordinates.at(i).at(1).toUInt()+imageReperCoordinates.at(i).at(3).toUInt())/2.0;
        centery += (imageReperCoordinates.at(i).at(2).toUInt()+imageReperCoordinates.at(i).at(4).toUInt())/2.0;

    }
    centerx /= imageReperCoordinates.size();
    centery /= imageReperCoordinates.size();
    angle = (180.0/M_PI)*atan(centery/centerx);
    qDebug() << "centerx = " << centerx << " centery = " << centery << " angle = " << angle;
}

void ImagesWindow::showTransformationData()
{
    double cx,cy;
    double dx,dy;
    for (int i=0;i<imageReperCoordinates.size();++i)
    {
        cx = (imageReperCoordinates.at(i).at(1).toInt()+imageReperCoordinates.at(i).at(3).toInt())/2.0;
        cy = (imageReperCoordinates.at(i).at(2).toInt()+imageReperCoordinates.at(i).at(4).toInt())/2.0;
        dx = (imageReperCoordinates.at(i).at(1).toInt()-imageReperCoordinates.at(i).at(3).toInt());
        dy = (imageReperCoordinates.at(i).at(2).toInt()-imageReperCoordinates.at(i).at(4).toInt());
        cout << imageReperCoordinates.at(i).at(0).toStdString() << " " << centerx - cx << " " << centery - cy << " " <<
                (180.0/M_PI)*atan(dy/dx) << endl;
    }
}

void ImagesWindow::updateCrops()
{
    cropx = cropxSpin->value();
    cropy = cropySpin->value();
    cropw = cropwSpin->value();
    croph = crophSpin->value();
}

void ImagesWindow::alignImages()
{
    double cx,cy;
    double dx,dy;
    double angle;
    QString imageName, alignImageName;
    calcCenterAndAngle();
    const char* alignedImagesPath = "/tmp/alignedImages/";
    QDir alignPath(alignedImagesPath);
    alignPath.removeRecursively();
    alignPath.mkpath(alignedImagesPath);
    int alignedImagesCounter=0;
    updateCrops();
    for (int i=0;i<imageReperCoordinates.size();++i)
    {
        cx = (imageReperCoordinates.at(i).at(1).toInt()+imageReperCoordinates.at(i).at(3).toInt())/2.0;
        cy = (imageReperCoordinates.at(i).at(2).toInt()+imageReperCoordinates.at(i).at(4).toInt())/2.0;
        dx = (imageReperCoordinates.at(i).at(1).toInt()-imageReperCoordinates.at(i).at(3).toInt());
        dy = (imageReperCoordinates.at(i).at(2).toInt()-imageReperCoordinates.at(i).at(4).toInt());
        imageName = imagesPath+"/"+imageReperCoordinates.at(i).at(0);
        alignImageName = alignedImagesPath+imageReperCoordinates.at(i).at(0);
        qDebug() << "imageName = " << imageName;
        QImage img;
        if (img.load(imageName)) {
            QSize imgSize = img.size();
            if (fabs(dx)>fabs(dy)) {
                angle = -(180.0/M_PI)*atan(dy/dx);
                referenceAngle=0.0;
            }
            else {
                angle = -(180.0/M_PI)*atan2(dy,dx);
                referenceAngle=90.0;
            }
            imageConverter(img, cx,cy, angle);
            if (img.save(alignImageName,"jpg",80)) {
                alignedImagesCounter++;
                qDebug() << alignImageName << " saved";
            }
        }
    }
    QMessageBox::information(this,"Aligned Images", QString::number(alignedImagesCounter)+QString(" Saved in %1").arg(alignedImagesPath));
}

void ImagesWindow::imageConverter(QImage &img, int cx, int cy, double angle)
{
    QPixmap pixmap(img.size().width()-cropw,img.size().height()-croph);
    QPainter p;
    p.begin(&pixmap);
    p.translate(2*centerx-cx,2*centery-cy);
    p.rotate(angle+referenceAngle);
    p.translate(-centerx-cropx,-centery-cropy);
    p.drawImage(0,0,img);
    p.end();
    img = pixmap.toImage();
}

ImagesWindow::~ImagesWindow()
{
}

