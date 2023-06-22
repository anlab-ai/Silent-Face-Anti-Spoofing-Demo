#include "face_detector.h"
#include "../android_log.h"
#include <opencv2/opencv.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/core.hpp>

// Constructor
BlurDetector::BlurDetector() {}

// Deconstruction
BlurDetector::~BlurDetector() {}


bool BlurDetector::Detect(cv::Mat &src, int previewWidth, int previewHeight, float threshold) {
    // Detect by fft

    // Convert to grayscale
    cv::Mat img_gray, img_gray_resize, fourierTransform;
    // IFFT (Inverse fast fourier transform)
    cv::Mat invFFT;
    cv::Mat logFFT;
    cv::cvtColor(src, img_gray, cv::COLOR_BGR2GRAY);
    cv::resize(img_gray, img_gray_resize, cv::Size(previewWidth, previewHeight));
    img_gray_resize.convertTo(img_gray_resize, CV_64F);
    int height = img_gray_resize.rows;
    int width = img_gray_resize.cols;
    int cx = width / 2;
    int cy = height / 2;

    cv::dft(img_gray_resize, fourierTransform, cv::DFT_COMPLEX_OUTPUT|cv::DFT_SCALE);
    cv::Mat q0(fourierTransform, cv::Rect(0, 0, cx, cy));       // Top-Left - Create a ROI per quadrant
    cv::Mat q1(fourierTransform, cv::Rect(cx, 0, cx, cy));      // Top-Right
    cv::Mat q2(fourierTransform, cv::Rect(0, cy, cx, cy));      // Bottom-Left
    cv::Mat q3(fourierTransform, cv::Rect(cx, cy, cx, cy));     // Bottom-Right

    cv::Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);

    // Block the low frequencies
    fourierTransform(cv::Rect(cx-60,cy-60,2*60,2*60)).setTo(0);

    //shuffle the quadrants to their original position
    cv::Mat orgFFT;
    fourierTransform.copyTo(orgFFT);
    cv::Mat p0(orgFFT, cv::Rect(0, 0, cx, cy));       // Top-Left - Create a ROI per quadrant
    cv::Mat p1(orgFFT, cv::Rect(cx, 0, cx, cy));      // Top-Right
    cv::Mat p2(orgFFT, cv::Rect(0, cy, cx, cy));      // Bottom-Left
    cv::Mat p3(orgFFT, cv::Rect(cx, cy, cx, cy));     // Bottom-Right
    p0.copyTo(tmp);
    p3.copyTo(p0);
    tmp.copyTo(p3);
    p1.copyTo(tmp); // swap quadrant (Top-Right with Bottom-Left)
    p2.copyTo(p1);
    tmp.copyTo(p2);

    double minVal,maxVal;
    cv::dft(orgFFT, invFFT, cv::DFT_INVERSE|cv::DFT_REAL_OUTPUT);
    invFFT = cv::abs(invFFT);
    cv::minMaxLoc(invFFT,&minVal,&maxVal,NULL,NULL);
    cv::log(invFFT,logFFT);
    logFFT *= 20;
    //result = numpy.mean(img_fft)
    cv::Scalar result= cv::mean(logFFT);

    // Consider value threshold
    if (result.val[0] < threshold){
        return false;
    }
    return true;
}