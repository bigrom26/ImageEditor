#include <QApplication>
#include <QImage>
#include <QWidget>
#include <QLabel>
#include <QBoxLayout>
#include <QPixmap>
#include <iostream>
#include <QColor>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QProgressBar>
#include <QDebug>
#include <QGroupBox>
#include <QButtonGroup>
#include "canny.h"
#include "dct.h"
#include "extrafilters.h"
#include "gradients.h"
#include "histograms.h"
#include "image_interface.h"
#include "snake.h"
#include "segmentationgraph.h"
#include "bisegmentationgraph.h"
#include "colortransform.h"
#include "fouriertransform.h"
#include "fogeffect.h"
#include "graytransform.h"
#include "threshold.h"
#include "mergeimages.h"
#include "myqlineedit.h"

#include <cuda.h>
#include <builtin_types.h>
#include <drvapi_error_string.h>

void printArray(const float* a, const unsigned int n) {
    QString s = "(";
    unsigned int ii;
    for (ii = 0; ii < n - 1; ++ii)
        s.append(QString::number(a[ii])).append(", ");
    s.append(QString::number(a[ii])).append(")");

    qDebug() << s;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QBoxLayout* phbxLayout0 = new QBoxLayout(QBoxLayout::LeftToRight);
        QPushButton *loadButt = new QPushButton("Load");
        QPushButton *copyButt = new QPushButton("Filtered->Source");
        phbxLayout0->addWidget(loadButt);
        phbxLayout0->addWidget(copyButt);

    QBoxLayout* phbxLayout1 = new QBoxLayout(QBoxLayout::LeftToRight);
        QPushButton *rescaleButt = new QPushButton("rescale input");
        myQLineEdit* ptxtSrcWidth = new myQLineEdit("Rescale source image with this width", "640");
        myQLineEdit* ptxtSrcHeight = new myQLineEdit("Rescale source image with this height", "480");
        phbxLayout1->addWidget(rescaleButt);
        phbxLayout1->addWidget(ptxtSrcWidth);
        phbxLayout1->addWidget(ptxtSrcHeight);

    QBoxLayout* phbxLayout2 = new QBoxLayout(QBoxLayout::LeftToRight);
        QPushButton *cannyButt = new QPushButton("Canny");
        myQLineEdit* ptxtCannyUpperThresh = new myQLineEdit("Threshold for edge detection", "10");
        myQLineEdit* ptxtCannyLowerThresh = new myQLineEdit("Threshold for edge continuation detection", "-10");
        phbxLayout2->addWidget(cannyButt);
        phbxLayout2->addWidget(ptxtCannyLowerThresh);
        phbxLayout2->addWidget(ptxtCannyUpperThresh);

    QBoxLayout* phbxLayout3 = new QBoxLayout(QBoxLayout::LeftToRight);
        QPushButton *heButt = new QPushButton("Histogram Equalization (HE)");
        QPushButton *aheButt = new QPushButton("Adaptive HE (AHE)");
        phbxLayout3->addWidget(heButt);
        phbxLayout3->addWidget(aheButt);

    QBoxLayout* phbxLayout4 = new QBoxLayout(QBoxLayout::LeftToRight);
        QPushButton *dctButt = new QPushButton("Show frequency:");
        QCheckBox* pchkShowDC = new QCheckBox("DC");
        QCheckBox* pchkShowLAC = new QCheckBox("Low");
        QCheckBox* pchkShowMAC = new QCheckBox("Medium");
        QCheckBox* pchkShowHAC = new QCheckBox("High");
        pchkShowDC->setChecked(true);
        pchkShowLAC->setChecked(true);
        pchkShowMAC->setChecked(true);
        pchkShowHAC->setChecked(true);
        phbxLayout4->addWidget(dctButt);
        phbxLayout4->addWidget(pchkShowDC);
        phbxLayout4->addWidget(pchkShowLAC);
        phbxLayout4->addWidget(pchkShowMAC);
        phbxLayout4->addWidget(pchkShowHAC);

    QBoxLayout* phbxLayout5 = new QBoxLayout(QBoxLayout::LeftToRight);
        QPushButton *avgButt = new QPushButton("AVG");
        QPushButton *lfButt = new QPushButton("LF coef");
        QPushButton *mfButt = new QPushButton("MF coef");
        QPushButton *hfButt = new QPushButton("HF coef");
        phbxLayout5->addWidget(avgButt);
        phbxLayout5->addWidget(lfButt);
        phbxLayout5->addWidget(mfButt);
        phbxLayout5->addWidget(hfButt);

    QBoxLayout* phbxLayout6 = new QBoxLayout(QBoxLayout::LeftToRight);
        QPushButton *bGradientButt = new QPushButton("gradient");
        QPushButton *nGradientButt = new QPushButton("normalized gradient");
        myQLineEdit* ptxtNGradientRadius = new myQLineEdit("Normalization radius", "8");
        phbxLayout6->addWidget(bGradientButt);
        phbxLayout6->addWidget(nGradientButt);
        phbxLayout6->addWidget(ptxtNGradientRadius);

    QBoxLayout* phbxLayout7 = new QBoxLayout(QBoxLayout::LeftToRight);
        QPushButton *gvfMagnitudeButt = new QPushButton("gvf magnitude");
        myQLineEdit* ptxtGVFmu0 = new myQLineEdit("u0", "0.8");
        myQLineEdit* ptxtGVFiter0 = new myQLineEdit("Number of iterations", "1");
        phbxLayout7->addWidget(gvfMagnitudeButt);
        phbxLayout7->addWidget(ptxtGVFmu0);
        phbxLayout7->addWidget(ptxtGVFiter0);

    QBoxLayout* phbxLayout8 = new QBoxLayout(QBoxLayout::LeftToRight);
        QPushButton *smoothButt = new QPushButton("smooth");
        phbxLayout8->addWidget(smoothButt);

    QBoxLayout* phbxLayout9 = new QBoxLayout(QBoxLayout::LeftToRight);
        QPushButton *longlinesButt = new QPushButton("longlines");
        myQLineEdit* ptxtMinLineLength = new myQLineEdit("Minimum line length to be left on picture", "8");
        phbxLayout9->addWidget(longlinesButt);
        phbxLayout9->addWidget(ptxtMinLineLength);

    QBoxLayout* phbxLayout10 = new QBoxLayout(QBoxLayout::LeftToRight);
        QPushButton *snakeOneButt = new QPushButton("one snake");
        phbxLayout10->addWidget(snakeOneButt);

    QBoxLayout* phbxLayout11 = new QBoxLayout(QBoxLayout::LeftToRight);
        QPushButton *segmentGraphButt = new QPushButton("Segment Graph");
        myQLineEdit* ptxtSegGraphK = new myQLineEdit("Treshold growth coefficient K", "10");
        myQLineEdit* ptxtSegGraphSigma = new myQLineEdit("Sigma --- TODO", "10");
        myQLineEdit* ptxtSegGraphMinSize = new myQLineEdit("Minimum segment length to be left on picture", "100");
        phbxLayout11->addWidget(segmentGraphButt);
        phbxLayout11->addWidget(ptxtSegGraphK);
        phbxLayout11->addWidget(ptxtSegGraphSigma);
        phbxLayout11->addWidget(ptxtSegGraphMinSize);

    QBoxLayout* phbxLayout12 = new QBoxLayout(QBoxLayout::LeftToRight);
        QPushButton *biSegmentGraphButt = new QPushButton("biSegment Graph");
        myQLineEdit* ptxtbiSegGraphLevel = new myQLineEdit("Separation value between lower and higher bi-segments", "10");
        myQLineEdit* ptxtbiSegGraphMinSize = new myQLineEdit("Minimum bisegment length to be left on picture", "100");
        QStringList list_of_filters;
        list_of_filters<< "brightness" << "red" << "green" << "blue";
        QComboBox* pcbxbiSegGraphSource = new QComboBox;
        pcbxbiSegGraphSource->addItems(list_of_filters);
        pcbxbiSegGraphSource->setCurrentIndex(0);
        phbxLayout12->addWidget(biSegmentGraphButt);
        phbxLayout12->addWidget(ptxtbiSegGraphLevel);
        phbxLayout12->addWidget(ptxtbiSegGraphMinSize);
        phbxLayout12->addWidget(pcbxbiSegGraphSource);

    QBoxLayout* phbxLayout17 = new QBoxLayout(QBoxLayout::LeftToRight);
        QPushButton *grayButt = new QPushButton("Gray Format");
        phbxLayout17->addWidget(grayButt);
    QBoxLayout* phbxLayout18 = new QBoxLayout(QBoxLayout::LeftToRight);
        QPushButton *binaryButt = new QPushButton("Threshold");
        myQLineEdit* ptxtBinaryRadius = new myQLineEdit("Binary Umbral", "100");
        phbxLayout18->addWidget(binaryButt);
        phbxLayout18->addWidget(ptxtBinaryRadius);
    QBoxLayout* phbxLayout19 = new QBoxLayout(QBoxLayout::LeftToRight);
        QPushButton *histeqButt = new QPushButton("Histogram Equalization (HE)");
        phbxLayout19->addWidget(histeqButt);
    QBoxLayout* phbxLayout20 = new QBoxLayout(QBoxLayout::LeftToRight);
        QPushButton *mergeButt = new QPushButton("Add Images");
        QPushButton *mergeAddButt = new QPushButton("Load");
        myQLineEdit *ptxtmergeIndexRadius = new myQLineEdit("Merge Index", "0.5");
        phbxLayout20->addWidget(mergeButt);
        phbxLayout20->addWidget(mergeAddButt);
        phbxLayout20->addWidget(ptxtmergeIndexRadius);
    QBoxLayout* phbxLayout13 = new QBoxLayout(QBoxLayout::LeftToRight);
        QPushButton *fogButt = new QPushButton("Fog Effect");
        phbxLayout13->addWidget(fogButt);
    QBoxLayout* phbxLayout14 = new QBoxLayout(QBoxLayout::LeftToRight);
        QPushButton *gradButt = new QPushButton("Gradient");
        phbxLayout14->addWidget(gradButt);
    QBoxLayout* phbxLayout15 = new QBoxLayout(QBoxLayout::LeftToRight);
        QPushButton *yuvButt = new QPushButton("YUV");
        QGroupBox *groupBox = new QGroupBox;
        QButtonGroup *buttonGroup = new QButtonGroup;
        QHBoxLayout *vbox = new QHBoxLayout;
        QCheckBox* pchkY = new QCheckBox("Y");
        QCheckBox* pchkCb = new QCheckBox("Cb");
        QCheckBox* pchkCr = new QCheckBox("Cr");
        pchkY->setChecked(true);
        pchkCb->setChecked(false);
        pchkCr->setChecked(false);
        buttonGroup->addButton(pchkY);
        buttonGroup->addButton(pchkCb);
        buttonGroup->addButton(pchkCr);
        buttonGroup->setExclusive(true);
        phbxLayout15->addWidget(yuvButt);
        vbox->addWidget(pchkY);
        vbox->addWidget(pchkCb);
        vbox->addWidget(pchkCr);
        groupBox->setLayout(vbox);
        phbxLayout15->addWidget(groupBox);
    QBoxLayout* phbxLayout16 = new QBoxLayout(QBoxLayout::LeftToRight);
        QPushButton *fftButt = new QPushButton("FFT");
        phbxLayout16->addWidget(fftButt);

    QProgressBar * ppbrProgress = new QProgressBar();
    ppbrProgress->setMaximum(100);
    ppbrProgress->setMinimum(0);
    ppbrProgress->setValue(0);
    QLineEdit * ptxtInfo = new QLineEdit();
    ptxtInfo->setReadOnly(true);
    ptxtInfo->setAlignment(Qt::AlignCenter);

    QBoxLayout* pbxLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    pbxLayout->addLayout(phbxLayout0);
    pbxLayout->addLayout(phbxLayout1);
    pbxLayout->addLayout(phbxLayout2);
    pbxLayout->addLayout(phbxLayout3);
    pbxLayout->addLayout(phbxLayout4);
    pbxLayout->addLayout(phbxLayout5);
    pbxLayout->addLayout(phbxLayout6);
    pbxLayout->addLayout(phbxLayout7);
    pbxLayout->addLayout(phbxLayout8);
    pbxLayout->addLayout(phbxLayout9);
    pbxLayout->addLayout(phbxLayout10);
    pbxLayout->addLayout(phbxLayout11);
    pbxLayout->addLayout(phbxLayout12);
    pbxLayout->addLayout(phbxLayout17);
    pbxLayout->addLayout(phbxLayout18);
    pbxLayout->addLayout(phbxLayout19);
    pbxLayout->addLayout(phbxLayout20);
    pbxLayout->addLayout(phbxLayout13);
    pbxLayout->addLayout(phbxLayout14);
    pbxLayout->addLayout(phbxLayout15);
    pbxLayout->addLayout(phbxLayout16);
    pbxLayout->addWidget(ppbrProgress);
    pbxLayout->addWidget(ptxtInfo);

    QLabel originalLabel; QImage originalImage;
    QLabel filteredLabel; QImage filteredImage;

    image_interface ii(&originalLabel, &filteredLabel, &originalImage, &filteredImage);
    canny cnn(&originalImage, &filteredImage);
    dct dc(&originalImage, &filteredImage);
    extraFilters ef(&originalImage, &filteredImage);
    gradients gs(&originalImage, &filteredImage);
    histograms hs(&originalImage, &filteredImage);
    snake snk(&originalImage, &filteredImage);
    segmentationGraph sg(&originalImage, &filteredImage);
    biSegmentationGraph bsg(&originalImage, &filteredImage);
    GrayTransform graytrans(&originalImage, &filteredImage);
    Threshold bintrans(&originalImage, &filteredImage);
    histograms histeq(&originalImage, &filteredImage);
    MergeImages mergimag(&originalImage, &filteredImage);
    FogEffect fogeff(&originalImage, &filteredImage);
    gradients gradtrans(&originalImage, &filteredImage);
    ColorTransform coltrans(&originalImage, &filteredImage);
    FourierTransform foutrans(&originalImage, &filteredImage);
    //segmentationLevelSet sls(&originalImage, &filteredImage);

    QObject::connect(loadButt, SIGNAL(clicked()), &ii, SLOT(load()));
    QObject::connect(copyButt, SIGNAL(clicked()), &ii, SLOT(copy()));
    QObject::connect(rescaleButt, SIGNAL(clicked()), &ii, SLOT(rescaleSrcImage()));
        QObject::connect(ptxtSrcWidth, SIGNAL(textChanged(const QString &)), &ii, SLOT(updateWidth(const QString &)));
        QObject::connect(ptxtSrcHeight, SIGNAL(textChanged(const QString &)), &ii, SLOT(updateHeight(const QString &)));
    QObject::connect(cannyButt, SIGNAL(clicked()), &cnn, SLOT(apply_canny()));
        QObject::connect(ptxtCannyLowerThresh, SIGNAL(textChanged(const QString &)), &cnn, SLOT(updateLowerThreshold(const QString &)));
        QObject::connect(ptxtCannyUpperThresh, SIGNAL(textChanged(const QString &)), &cnn, SLOT(updateUpperThreshold(const QString &)));
    QObject::connect(heButt, SIGNAL(clicked()), &hs, SLOT(apply_histogramEqualization()));
    QObject::connect(aheButt, SIGNAL(clicked()), &hs, SLOT(apply_histogramAdaptiveEqualization()));
    QObject::connect(dctButt, SIGNAL(clicked()), &dc, SLOT(apply_dctidct()));
        QObject::connect(pchkShowDC, SIGNAL(stateChanged(const int)), &dc, SLOT(set_DC(const int)));
        QObject::connect(pchkShowLAC, SIGNAL(stateChanged(const int)), &dc, SLOT(set_LAC(const int)));
        QObject::connect(pchkShowMAC, SIGNAL(stateChanged(const int)), &dc, SLOT(set_MAC(const int)));
        QObject::connect(pchkShowHAC, SIGNAL(stateChanged(const int)), &dc, SLOT(set_HAC(const int)));

    QObject::connect(avgButt, SIGNAL(clicked()), &dc, SLOT(apply_AVG()));
    QObject::connect(lfButt, SIGNAL(clicked()), &dc, SLOT(apply_LF()));
    QObject::connect(mfButt, SIGNAL(clicked()), &dc, SLOT(apply_MF()));
    QObject::connect(hfButt, SIGNAL(clicked()), &dc, SLOT(apply_HF()));

    QObject::connect(bGradientButt, SIGNAL(clicked()), &gs, SLOT(apply_gradient()));
    QObject::connect(nGradientButt, SIGNAL(clicked()), &gs, SLOT(apply_nGradient()));
        QObject::connect(ptxtNGradientRadius, SIGNAL(textChanged(const QString&)), &gs, SLOT(update_gradius(const QString&)));

    QObject::connect(gvfMagnitudeButt, SIGNAL(clicked()), &gs, SLOT(apply_GVF()));
        QObject::connect(ptxtGVFmu0, SIGNAL(textChanged(const QString &)), &gs, SLOT(update_mu0(const QString &)));
        QObject::connect(ptxtGVFiter0, SIGNAL(textChanged(const QString &)), &gs, SLOT(update_iter0(const QString &)));
    QObject::connect(smoothButt, SIGNAL(clicked()), &ef, SLOT(apply_smooth()));
    QObject::connect(longlinesButt, SIGNAL(clicked()), &ef, SLOT(apply_deleteShortLines()));
        QObject::connect(ptxtMinLineLength, SIGNAL(textChanged(const QString &)), &ef, SLOT(update_minLineLength(const QString &)));
    QObject::connect(snakeOneButt, SIGNAL(clicked()), &snk, SLOT(apply_snake()));

    QObject::connect(segmentGraphButt, SIGNAL(clicked()), &sg, SLOT(apply_segmentationGraph()));
        QObject::connect(ptxtSegGraphK, SIGNAL(textChanged(const QString &)), &sg, SLOT(update_K(const QString &)));
        QObject::connect(ptxtSegGraphSigma, SIGNAL(textChanged(const QString &)), &sg, SLOT(update_sigma(const QString &)));
        QObject::connect(ptxtSegGraphMinSize, SIGNAL(textChanged(const QString &)), &sg, SLOT(update_minSegSize(const QString &)));

    QObject::connect(biSegmentGraphButt, SIGNAL(clicked()), &bsg, SLOT(apply_bisegmentationGraph()));
        QObject::connect(ptxtbiSegGraphLevel, SIGNAL(textChanged(const QString &)), &bsg, SLOT(update_level(const QString &)));
        QObject::connect(ptxtbiSegGraphMinSize, SIGNAL(textChanged(const QString &)), &bsg, SLOT(update_minSegSize(const QString &)));
        QObject::connect(pcbxbiSegGraphSource, SIGNAL(currentIndexChanged(const int)), &bsg, SLOT(set_source(const int)));

    QObject::connect(grayButt, SIGNAL(clicked()), &graytrans, SLOT(convertToGray()));
    QObject::connect(binaryButt, SIGNAL(clicked()), &bintrans, SLOT(convertToBinary()));
        QObject::connect(ptxtBinaryRadius, SIGNAL(textChanged(const QString&)), &bintrans, SLOT(setUmbral(const QString&)));
    QObject::connect(histeqButt, SIGNAL(clicked()), &histeq, SLOT(apply_histogramEqualization()));
    QObject::connect(mergeButt, SIGNAL(clicked()), &mergimag, SLOT(mergeImages()));
        QObject::connect(mergeAddButt, SIGNAL(clicked()), &mergimag, SLOT(loadKernel()));
        QObject::connect(ptxtmergeIndexRadius, SIGNAL(textChanged(const QString&)), &mergimag, SLOT(setIndex(const QString&)));
    QObject::connect(gradButt, SIGNAL(clicked()), &gradtrans, SLOT(apply_gradient()));
    QObject::connect(fogButt, SIGNAL(clicked()), &fogeff, SLOT(calcFogEffect()));
    QObject::connect(yuvButt, SIGNAL(clicked()), &coltrans, SLOT(convertToYUV()));
        QObject::connect(pchkY, SIGNAL(stateChanged(const int)), &coltrans, SLOT(setY(const int)));
        QObject::connect(pchkCb, SIGNAL(stateChanged(const int)), &coltrans, SLOT(setCb(const int)));
        QObject::connect(pchkCr, SIGNAL(stateChanged(const int)), &coltrans, SLOT(setCr(const int)));
    QObject::connect(fftButt, SIGNAL(clicked()), &foutrans, SLOT(calcFFT()));

    QObject::connect(&cnn, SIGNAL(image_ready()), &ii, SLOT(updateDstImage()));
    QObject::connect(&hs, SIGNAL(image_ready()), &ii, SLOT(updateDstImage()));
    QObject::connect(&dc, SIGNAL(image_ready()), &ii, SLOT(updateDstImage()));
    QObject::connect(&gs, SIGNAL(image_ready()), &ii, SLOT(updateDstImage()));
    QObject::connect(&ef, SIGNAL(image_ready()), &ii, SLOT(updateDstImage()));
    QObject::connect(&snk, SIGNAL(image_ready()), &ii, SLOT(updateDstImage()));
    QObject::connect(&sg, SIGNAL(image_ready()), &ii, SLOT(updateDstImage()));
    QObject::connect(&bsg, SIGNAL(image_ready()), &ii, SLOT(updateDstImage()));
    QObject::connect(&graytrans, SIGNAL(image_ready()), &ii, SLOT(updateDstImage()));
    QObject::connect(&bintrans, SIGNAL(image_ready()), &ii, SLOT(updateDstImage()));
    QObject::connect(&histeq, SIGNAL(image_ready()), &ii, SLOT(updateDstImage()));
    QObject::connect(&mergimag, SIGNAL(image_ready()), &ii, SLOT(updateDstImage()));
    QObject::connect(&fogeff, SIGNAL(image_ready()), &ii, SLOT(updateDstImage()));
    QObject::connect(&gradtrans, SIGNAL(image_ready()), &ii, SLOT(updateDstImage()));
    QObject::connect(&coltrans, SIGNAL(image_ready()), &ii, SLOT(updateDstImage()));
    QObject::connect(&foutrans, SIGNAL(image_ready()), &ii, SLOT(updateDstImage()));

    QObject::connect(&cnn, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(&hs, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(&dc, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(&gs, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(&ef, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(&snk, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(&sg, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(&bsg, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(&ii, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(ptxtSrcWidth, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(ptxtSrcHeight, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(ptxtCannyUpperThresh, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(ptxtCannyLowerThresh, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(ptxtNGradientRadius, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(ptxtGVFiter0, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(ptxtGVFmu0, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(ptxtMinLineLength, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(ptxtSegGraphK, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(ptxtSegGraphMinSize, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(ptxtSegGraphSigma, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(ptxtbiSegGraphLevel, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(ptxtbiSegGraphMinSize, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(&graytrans, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(&bintrans, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(&histeq, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(&mergimag, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(ptxtBinaryRadius, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(&fogeff, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(&gradtrans, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(&coltrans, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));
    QObject::connect(&foutrans, SIGNAL(print_message(const QString&)), ptxtInfo, SLOT(setText(const QString&)));

    QObject::connect(&cnn, SIGNAL(print_progress(const int)), ppbrProgress, SLOT(setValue(const int)));
    QObject::connect(&hs, SIGNAL(print_progress(const int)), ppbrProgress, SLOT(setValue(const int)));
    QObject::connect(&dc, SIGNAL(print_progress(const int)), ppbrProgress, SLOT(setValue(const int)));
    QObject::connect(&gs, SIGNAL(print_progress(const int)), ppbrProgress, SLOT(setValue(const int)));
    QObject::connect(&ef, SIGNAL(print_progress(const int)), ppbrProgress, SLOT(setValue(const int)));
    QObject::connect(&snk, SIGNAL(print_progress(const int)), ppbrProgress, SLOT(setValue(const int)));
    QObject::connect(&sg, SIGNAL(print_progress(const int)), ppbrProgress, SLOT(setValue(const int)));
    QObject::connect(&bsg, SIGNAL(print_progress(const int)), ppbrProgress, SLOT(setValue(const int)));
    QObject::connect(&graytrans, SIGNAL(print_progress(const int)), ppbrProgress, SLOT(setValue(const int)));
    QObject::connect(&bintrans, SIGNAL(print_progress(const int)), ppbrProgress, SLOT(setValue(const int)));
    QObject::connect(&histeq, SIGNAL(print_progress(const int)), ppbrProgress, SLOT(setValue(const int)));
    QObject::connect(&mergimag, SIGNAL(print_progress(const int)), ppbrProgress, SLOT(setValue(const int)));
    QObject::connect(&fogeff, SIGNAL(print_progress(const int)), ppbrProgress, SLOT(setValue(const int)));
    QObject::connect(&gradtrans, SIGNAL(print_progress(const int)), ppbrProgress, SLOT(setValue(const int)));
    QObject::connect(&coltrans, SIGNAL(print_progress(const int)), ppbrProgress, SLOT(setValue(const int)));
    QObject::connect(&foutrans, SIGNAL(print_progress(const int)), ppbrProgress, SLOT(setValue(const int)));

    QWidget wgt;
    wgt.resize(256, 100);
    wgt.setLayout(pbxLayout);
    wgt.show();
    wgt.move(1400, 20);
    originalLabel.move(0,0);
    filteredLabel.move(0,100);

    int cudaDevice = 0;
    char cudaDeviceName [100];

    cuInit(0);
    cuDeviceGet(&cudaDevice, 0);
    cuDeviceGetName(cudaDeviceName, 100, cudaDevice);

    wgt.setWindowTitle(QString("ImageEditor - ")+QString(cudaDeviceName));
    return app.exec();
}
