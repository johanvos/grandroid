#include <jni.h>
#include <android/native_window_jni.h>
#include "grandroid.h"
#include <stdlib.h>

#include <EGL/egl.h>
#include "glass.h"

// comment this out to see it work (at least the first invocation of eglMakeCurrent
// #define ALL_NATIVE 1

ANativeWindow* getNativeWindow();
EGLSurface myEglSurface;

#define asJLong(x) (jlong)x

/*
 * Class:     com_sun_glass_ui_monocle_AndroidScreen
 * Method:    _getNativeHandle
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_sun_glass_ui_monocle_AndroidScreen__1getNativeHandle
  (JNIEnv *env, jobject obj) {
    return (jlong)getNativeWindow();
}

JNIEXPORT jlong JNICALL Java_hello_HelloWorld__1getNativeHandle
  (JNIEnv *env, jobject obj) {
    return (jlong)getNativeWindow();
}

JNIEXPORT void  JNICALL Java_com_sun_glass_ui_monocle_EGL_testGraal
    (JNIEnv *env, jclass clazz, jlong nativeWindow, jintArray major, jintArray minor, jintArray attributes, jlongArray eglConfigs, jintArray configcounts) {
    LOGE(stderr, "JNI testGraal asked. Ignore. java layer has native window at %lx and we have it at %p.\n", nativeWindow, getNativeWindow());
}

JNIEXPORT jlong JNICALL Java_com_sun_glass_ui_monocle_EGL_eglGetDisplay
    (JNIEnv *env, jclass clazz, jlong display) {
    LOGE(stderr, "[GLASS] getDisplay, dislay = %ld or %p, env at %p\n", display, (void *)display, env);
    EGLDisplay eglDisplay = eglGetDisplay(((EGLNativeDisplayType) (unsigned long)(display)));
    LOGE(stderr, "[GLASS] getDisplay, got dislay = %ld or %p\n", (long)eglDisplay, eglDisplay);
    return (jlong)(eglDisplay);
}

JNIEXPORT jlong JNICALL Java_hello_EGL_eglGetDisplay
    (JNIEnv *env, jclass clazz, jlong display) {
    return Java_com_sun_glass_ui_monocle_EGL_eglGetDisplay(env, clazz, display);
}

JNIEXPORT jboolean JNICALL Java_com_sun_glass_ui_monocle_EGL_eglInitialize
    (JNIEnv *env, jclass clazz, jlong eglDisplayPtr, jintArray majorArray,
     jintArray minorArray){
    EGLDisplay eglDisplay = (EGLDisplay)eglDisplayPtr;
    LOGE(stderr, "[GLASS] eglInitialize, egldislay = %ld or %p, env at %p\n", eglDisplayPtr, eglDisplay, env);

    EGLint major, minor;
    if (eglInitialize(eglDisplay, &major, &minor)) {
         (*env)->SetIntArrayRegion(env, majorArray, 0, 1, &major);
         (*env)->SetIntArrayRegion(env, minorArray, 0, 1, &minor);
        return JNI_TRUE;
    } else {
        LOGE(stderr, "[GLASS] major issue! eglInitialize failed!!!\n\n\n");
        return JNI_FALSE;
    }
}

JNIEXPORT jboolean JNICALL Java_hello_EGL_eglInitialize
    (JNIEnv *env, jclass clazz, jlong eglDisplayPtr, jintArray majorArray,
     jintArray minorArray){
return Java_com_sun_glass_ui_monocle_EGL_eglInitialize(env, clazz, eglDisplayPtr, majorArray, minorArray);
}

JNIEXPORT jboolean JNICALL Java_com_sun_glass_ui_monocle_EGL_eglBindAPI
    (JNIEnv *env, jclass clazz, jint api) {
    LOGE(stderr, "[GLASS] bindApi, env at %p\n", env);

    if (eglBindAPI(api)) {
        return JNI_TRUE;
    } else {
        return JNI_FALSE;
    }
}

JNIEXPORT jboolean JNICALL Java_hello_EGL_eglBindAPI
    (JNIEnv *env, jclass clazz, jint api) {
return  JNICALL Java_com_sun_glass_ui_monocle_EGL_eglBindAPI(env, clazz, api);
}
int setEGLAttrs(jint *attrs, int *eglAttrs) {
    int index = 0;

    eglAttrs[index++] = EGL_SURFACE_TYPE;
    if (attrs[6] != 0) {
        eglAttrs[index++] = (EGL_WINDOW_BIT);
    } else {
        eglAttrs[index++] = EGL_PBUFFER_BIT;
    }

    // TODO:  We are depending on the order of attributes defined in
    // GLPixelFormat - we need a better way to manage this

    if (attrs[0] == 5 && attrs[1] == 6
            && attrs[2] == 5 && attrs[3] == 0) {
        // Optimization for Raspberry Pi model B. Even though the result
        // of setting EGL_BUFFER_SIZE to 16 should be the same as setting
        // component sizes separately, we get less per-frame overhead if we
        // only set EGL_BUFFER_SIZE.
        eglAttrs[index++] = EGL_BUFFER_SIZE;
        eglAttrs[index++] = 16;
    } else {
        eglAttrs[index++] = EGL_RED_SIZE;
        eglAttrs[index++] = attrs[0];
        eglAttrs[index++] = EGL_GREEN_SIZE;
        eglAttrs[index++] = attrs[1];
        eglAttrs[index++] = EGL_BLUE_SIZE;
        eglAttrs[index++] = attrs[2];
        eglAttrs[index++] = EGL_ALPHA_SIZE;
        eglAttrs[index++] = attrs[3];
    }

    eglAttrs[index++] = EGL_DEPTH_SIZE;
    eglAttrs[index++] = attrs[4];
    eglAttrs[index++] = EGL_RENDERABLE_TYPE;
    eglAttrs[index++] = EGL_OPENGL_ES2_BIT;
    eglAttrs[index] = EGL_NONE;
return index;
}



JNIEXPORT jboolean JNICALL Java_com_sun_glass_ui_monocle_EGL_eglChooseConfig
    (JNIEnv *env, jclass clazz, jlong eglDisplayPtr, jintArray attribs,
     jlongArray configs, jint configSize, jintArray numConfigs) {
    LOGE(stderr, "[GLASS] eglChooseConfig, env at %p\n", env);
    EGLDisplay eglDisplay = (EGLDisplay)eglDisplayPtr;
    int i=0;

    int eglAttrs[50]; // value, attr pair plus a None
    jint *attrArray;

    attrArray = (*env)->GetIntArrayElements(env, attribs, JNI_FALSE);
    int cnt = setEGLAttrs(attrArray, eglAttrs);
    (*env)->ReleaseIntArrayElements(env, attribs, attrArray, JNI_ABORT);
    EGLConfig *configArray = malloc(sizeof(EGLConfig) * configSize);
    jlong *longConfigArray = malloc(sizeof(long) * configSize);
    EGLint numConfigPtr=0;
    jboolean retval;
     for (int i =0; i < cnt; i++) {
         LOGE(stderr, "attributes for config %d: %d\n", i, eglAttrs[i]);
     }


    if (!eglChooseConfig(eglDisplay, eglAttrs, configArray, configSize,
                               &numConfigPtr)) {
        retval = JNI_FALSE;
    } else {
        retval = JNI_TRUE;
        (*env)->SetIntArrayRegion(env, numConfigs, 0, 1, &numConfigPtr);
        for (i = 0; i < numConfigPtr; i++) {
            longConfigArray[i] = asJLong(configArray[i]);
        }

        (*env)->SetLongArrayRegion(env, configs, 0, configSize, longConfigArray);
    }
    free(configArray);
    free(longConfigArray);
LOGE(stderr, "GLERR after config?  %d\n",eglGetError());
    return retval;
}
JNIEXPORT jboolean JNICALL Java_hello_EGL_eglChooseConfig
    (JNIEnv *env, jclass clazz, jlong eglDisplayPtr, jintArray attribs,
     jlongArray configs, jint configSize, jintArray numConfigs) {
return Java_com_sun_glass_ui_monocle_EGL_eglChooseConfig(env, clazz, eglDisplayPtr, attribs, configs, configSize, numConfigs);
}

JNIEXPORT jlong JNICALL Java_com_sun_glass_ui_monocle_EGL__1eglCreateWindowSurface
    (JNIEnv *env, jclass clazz, jlong eglDisplayPtr, jlong eglConfigPtr,
     jlong nativeWindow, jintArray attribs) {

    fprintf(stderr, "[JNIEGL] createWindowSurface, env at %p\n", env);
EGLDisplay eglDisplay = (EGLDisplay)eglDisplayPtr;
EGLConfig eglConfig = (EGLConfig)eglConfigPtr;
    EGLSurface eglSurface;
    EGLint *attrArray = NULL;

    if (attribs != NULL) {
        attrArray = (*env)->GetIntArrayElements(env, attribs, JNI_FALSE);
    }

    eglSurface =  eglCreateWindowSurface(eglDisplay, eglConfig,
                                         (EGLNativeWindowType) nativeWindow,
                                         (EGLint *) NULL);
    if (attrArray != NULL) {
        (*env)->ReleaseIntArrayElements(env, attribs, attrArray, JNI_ABORT);
    }
    LOGE(stderr, "EGL Surface create at %p, errorcode is %d \n", eglSurface, eglGetError());
    myEglSurface = eglSurface;
#ifdef ALL_NATIVE
    LOGE(stderr, "BYPASS MODUS! eglCreateSurface done, now go to eglCreateContext");
Java_com_sun_glass_ui_monocle_EGL_eglCreateContext(env, clazz, eglDisplayPtr, eglConfigPtr, 0, 0);
#endif
    return asJLong(eglSurface);
}

JNIEXPORT jlong JNICALL Java_hello_EGL__1eglCreateWindowSurface
    (JNIEnv *env, jclass clazz, jlong eglDisplayPtr, jlong eglConfigPtr,
     jlong nativeWindow, jintArray attribs) {
    return Java_com_sun_glass_ui_monocle_EGL__1eglCreateWindowSurface (env, clazz, eglDisplayPtr, eglConfigPtr, nativeWindow, attribs);
}
JNIEXPORT jlong JNICALL Java_com_sun_glass_ui_monocle_EGL_eglCreateContext
    (JNIEnv *env, jclass clazz, jlong eglDisplayPtr, jlong eglConfigPtr,
      jlong shareContext, jintArray attribs){

    LOGE(stderr, "eglCreateContext with env %p\n", env);
    EGLDisplay eglDisplay = (EGLDisplay)eglDisplayPtr;
    EGLConfig eglConfig = (EGLConfig)eglConfigPtr;

    LOGE(stderr, "EGLcretecontext, egldisplay at %p with val %ld\n", eglDisplay, eglDisplayPtr);
    LOGE(stderr, "EGLcretecontext, config at %p with val %ld\n", eglConfig, eglConfigPtr);
    // we don't support any passed-in context attributes presently
    // we don't support any share context presently
    EGLint contextAttrs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
    EGLContext context = eglCreateContext(eglDisplay, eglConfig,
                                          NULL, contextAttrs);
    LOGE(stderr, "eglContext created at %p\n", context);
    LOGE(stderr, "GLERR?  %d\n",eglGetError());

#ifdef ALL_NATIVE
    LOGE(stderr, "BYPASS MODUS! eglCreateContext done, now go to eglMakeCurrent");
Java_com_sun_glass_ui_monocle_EGL_eglMakeCurrent(env, clazz, eglDisplayPtr, asJLong(myEglSurface), asJLong(myEglSurface), asJLong(context));
#endif
    return asJLong(context);
}

JNIEXPORT jboolean JNICALL Java_com_sun_glass_ui_monocle_EGL_eglMakeCurrent
   (JNIEnv *env, jclass clazz, jlong eglDisplayPtr, jlong drawSurfacePtr,
    jlong readSurfacePtr, jlong eglContextPtr) {
    LOGE(stderr, "eglMakeCurrent with env %p\n", env);
    EGLDisplay eglDisplay = (EGLDisplay)eglDisplayPtr;
    EGLSurface eglDrawSurface = (EGLSurface)drawSurfacePtr;
    EGLSurface eglReadSurface = (EGLSurface)readSurfacePtr;
    EGLContext eglContext = (EGLSurface)eglContextPtr;
    LOGE(stderr, "EGL MakeCurrent! disp = %p, surface = %p, readS = %p, context = %p\n", eglDisplay, eglDrawSurface, eglReadSurface, eglContext);
    LOGE(stderr, "GLERR?  %d\n",eglGetError());


    if (eglMakeCurrent(eglDisplay, eglDrawSurface, eglReadSurface,
                   eglContext)) {
        LOGE(stderr, "EGLMAKECURRENT SUCCEEDED\n");
        return JNI_TRUE;
    } else {
        LOGE(stderr, "eglMakeCurrent failed\n");
        return JNI_FALSE;
    }
}
JNIEXPORT jboolean JNICALL Java_com_sun_glass_ui_monocle_EGL_eglMakeCurrent
   (JNIEnv *env, jclass clazz, jlong eglDisplayPtr, jlong drawSurfacePtr,
    jlong readSurfacePtr, jlong eglContextPtr) {
    return Java_com_sun_glass_ui_monocle_EGL_eglMakeCurrent (env, clazz, eglDisplayPtr, d
}


