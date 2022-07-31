#include "imageswindow.h"
#include <QFile>
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

ImagesWindow::ImagesWindow(const char *imagesFileName, QWidget *parent)
    : QMainWindow(parent)
{
    currentFileName = imagesFileName;
    readImagesFile();
    calcCenterAndAngle();
    showTransformationData();
    nextImage = new QPushButton(this);
    nextImage->setText("Next Image");
    nextImage->setGeometry(10,10,120,30);
    savePoints = new QPushButton(this);
    savePoints->setText("Save Points");
    savePoints->setGeometry(150,10,150,30);
    alignAllImages = new QPushButton(this);
    alignAllImages->setText("Align Images");
    alignAllImages->setGeometry(350,10,150,30);
    currentIndex = -1;
    mousePressCounter = 0;
    connect(nextImage,SIGNAL(clicked()),this,SLOT(showNextImage()));
    connect(savePoints,SIGNAL(clicked()), this,SLOT(saveReferencePoints()));
    connect(alignAllImages,SIGNAL(clicked()),this,SLOT(alignImages()));
    setFixedSize(1600,1000);
    viewImage = new QScrollArea(this);
    viewImage->setGeometry(0,50,1600,950);
}

void ImagesWindow::mousePressEvent(QMouseEvent *mouse)
{
    if (currentIndex<0) return;
    int x = (mouse->x()+viewImage->horizontalScrollBar()->value())*3;
    int y = (mouse->y()-viewImage->y()+viewImage->verticalScrollBar()->value())*3;
    QStringList tempList;
    tempList = imageReperCoordinates.at(currentIndex);
    tempList.replace(mousePressCounter*2+1,QString::number(x));
    tempList.replace(mousePressCounter*2+2,QString::number(y));
    imageReperCoordinates.replace(currentIndex,tempList);
    mousePressCounter++;
    mousePressCounter%=2;
    currentIndex-=1;
    showNextImage();
}

void ImagesWindow::showNextImage()
{
    currentIndex++;
    currentIndex%=imageReperCoordinates.size();
    setWindowTitle(QString("imagesAligner %1 %2/%3").arg(imageReperCoordinates.at(currentIndex).at(0)).arg(currentIndex+1).arg(imageReperCoordinates.size()));
    QImage img;
    QString imageName;
    imageName = imagesPath+"/"+imageReperCoordinates.at(currentIndex).at(0);
    if (img.load(imageName)) {
        imageLabel = new QLabel;
        QPixmap pixmap = QPixmap::fromImage(img);
        QPainter p(&pixmap);
        p.setBrush(QBrush(Qt::red));
        QFont font = p.font();
        font.setPointSize(144);
        p.setFont(font);
        p.setPen(Qt::green);
        p.drawText(QRect(img.size().width()/2,200,400,200),QString::number(currentIndex+1));
        p.drawEllipse(imageReperCoordinates.at(currentIndex).at(1).toInt()-10, imageReperCoordinates.at(currentIndex).at(2).toInt()-10,20,20);
        p.drawRect(imageReperCoordinates.at(currentIndex).at(3).toInt()-10, imageReperCoordinates.at(currentIndex).at(4).toInt()-10,20,20);
        p.setPen(Qt::blue);
        p.drawRect(centerx-5,centery-20-200,10,40);
        p.drawRect(centerx-20,centery-5-200,40,10);
        p.end();
        imageLabel->setPixmap(pixmap.scaled(QSize(pixmap.width()/3,pixmap.height()/3)));
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

void ImagesWindow::alignImages()
{
    double cx,cy;
    double dx,dy;
    QString imageName, alignImageName;
    for (int i=0;i<imageReperCoordinates.size();++i)
    {
        cx = (imageReperCoordinates.at(i).at(1).toInt()+imageReperCoordinates.at(i).at(3).toInt())/2.0;
        cy = (imageReperCoordinates.at(i).at(2).toInt()+imageReperCoordinates.at(i).at(4).toInt())/2.0;
        dx = (imageReperCoordinates.at(i).at(1).toInt()-imageReperCoordinates.at(i).at(3).toInt());
        dy = (imageReperCoordinates.at(i).at(2).toInt()-imageReperCoordinates.at(i).at(4).toInt());
        imageName = imagesPath+"/"+imageReperCoordinates.at(i).at(0);
        alignImageName = "/tmp/alignedImages/"+imageReperCoordinates.at(i).at(0);
        qDebug() << "imageName = " << imageName;
        QImage img;
        if (img.load(imageName)) {
            QSize imgSize = img.size();
            imageConverter(img, cx,cy,-(180.0/M_PI)*atan(dy/dx));
            if (img.save(alignImageName,"jpg",80))
                qDebug() << alignImageName << " saved";
        }
    }
}

void ImagesWindow::imageConverter(QImage &img, int cx, int cy, double angle)
{
    QMatrix matrix;
    QPixmap pixmap(img.size().width(),img.size().height()-546);
    QPainter p(&pixmap);
    //p.setWindow(-centerx,-centery,pixmap.width(),pixmap.height());
    matrix.translate(centerx-cx, centery-cy-200);
    //matrix.rotate(angle);
    p.setMatrix(matrix);
    p.drawImage(0,0,img);
    p.end();
    /*QPixmap pixmap1(img.size());
    p.begin(&pixmap1);
    p.setWindow(-centerx,-centery,pixmap1.width(),pixmap1.height());
    matrix.reset();
    matrix.translate(-centerx, -centery);
    matrix.rotate(angle);
    p.setMatrix(matrix);
    p.drawPixmap(0,0,pixmap);
    p.end();*/
    img = pixmap.toImage();
}

ImagesWindow::~ImagesWindow()
{
}

