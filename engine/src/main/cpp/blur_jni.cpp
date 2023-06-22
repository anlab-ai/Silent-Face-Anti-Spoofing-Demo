#include <android/asset_manager_jni.h>
#include "jni_long_field.h"
#include "detection/face_detector.h"
#include "android_log.h"
#include "img_process.h"
#include <opencv2/opencv.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

// Define method with blur detector
extern "C" {
    JNIEXPORT jlong JNICALL
    BLUR_DETECTOR_METHOD(allocate)(JNIEnv *env, jobject instance);

    JNIEXPORT void JNICALL
    BLUR_DETECTOR_METHOD(deallocate)(JNIEnv *env, jobject instance);

    JNIEXPORT jboolean JNICALL
    BLUR_DETECTOR_METHOD(nativeDetectBlurYuv)(JNIEnv *env, jobject instance, jbyteArray yuv,
                                          jint preview_width, jint preview_height);
}

JniLongField blur_detector_field("nativeHandler");
BlurDetector* get_blur(JNIEnv* env, jobject instance) {
    BlurDetector* const blur = reinterpret_cast<BlurDetector*>(blur_detector_field.get(env, instance));
    return blur;
}

void set_blur(JNIEnv* env, jobject instance, BlurDetector* blur) {
    blur_detector_field.set(env, instance, reinterpret_cast<intptr_t>(blur));
}


JNIEXPORT jlong JNICALL
BLUR_DETECTOR_METHOD(allocate)(JNIEnv *env, jobject instance) {
    auto * const blur = new BlurDetector();
    set_blur(env, instance, blur);
    return reinterpret_cast<intptr_t> (blur);
}


JNIEXPORT void JNICALL
BLUR_DETECTOR_METHOD(deallocate)(JNIEnv *env, jobject instance) {
    delete get_blur(env, instance);
    set_blur(env, instance, nullptr);
}


JNIEXPORT jboolean JNICALL
BLUR_DETECTOR_METHOD(nativeDetectBlurYuv)(JNIEnv *env, jobject instance, jbyteArray yuv,
                                          jint preview_width, jint preview_height){
    jbyte *yuv_ = env->GetByteArrayElements(yuv, nullptr);
    cv::Mat bgr;
    BlurDetector *blur = get_blur(env, instance);
    Yuv420sp2bgr(reinterpret_cast<unsigned char *>(yuv_), preview_width, preview_height, 1, bgr);

    // Check size
    int width_blur = blur->get_size_image();
    int height_blur = blur->get_size_image();
    float threshold_blur = blur->get_threshold();

    bool detect_blur = blur->Detect(bgr, width_blur, height_blur, threshold_blur);
    env->ReleaseByteArrayElements(yuv, yuv_, 0);
    return detect_blur;
}

