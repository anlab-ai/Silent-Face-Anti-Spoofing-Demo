package com.mv.engine

import androidx.annotation.Keep

class BlurDetector : Component() {

    @Keep
    private var nativeHandler: Long

    init {
        nativeHandler = createInstance()
    }

    override fun createInstance(): Long = allocate()

    override fun destroy() {
        deallocate()
    }

    fun detect_blur(yuv: ByteArray, previewWidth: Int, previewHeight: Int): Boolean {
        return nativeDetectBlurYuv(
            yuv,
            previewWidth,
            previewHeight
        )
    }


    /////////////////////////////////////////// Native ///////////////////////////////////
    @Keep
    private external fun allocate(): Long

    @Keep
    private external fun deallocate()

    @Keep
    private external fun nativeDetectBlurYuv(
        yuv: ByteArray,
        previewWidth: Int,
        previewHeight: Int
    ): Boolean
}