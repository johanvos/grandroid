#include <jni.h>
#include <android/native_window_jni.h>
#include "grandroid.h"
#include <stdlib.h>

#include <EGL/egl.h>
#include "glass.h"

// comment this out to see it work (at least the first invocation of eglMakeCurrent
#define ALL_NATIVE 1

ANativeWindow* getNativeWindow();
EGLSurface myEglSurface;
EGLContext myEglContext;
EGLDisplay myEglDisplay;
EGLConfig myEglConfig;

#define asPtr(x) ((void *) (unsigned long) (x))
#define asJLong(x) ((jlong) (unsigned long) (x))


JNIEXPORT jlong JNICALL Java_hello_HelloWorld__1getNativeHandle
  (JNIEnv *env, jobject obj) {
    return (jlong)getNativeWindow();
}

JNIEXPORT jlong JNICALL Java_hello_EGL_eglGetDisplay
    (JNIEnv *env, jclass clazz, jlong display) {
    LOGE(stderr, "[GLASS] getDisplay, dislay = %ld or %p, env at %p\n", display, (void *)display, env);
    EGLDisplay eglDisplay = eglGetDisplay(((EGLNativeDisplayType) (unsigned long)(display)));
    LOGE(stderr, "[GLASS] getDisplay, got display = %ld or %p at address %p\n", (long)eglDisplay, eglDisplay, &eglDisplay);
LOGE(stderr, "before assign, m = %p and e = %p and ma = %p, me = %p\n",myEglDisplay, eglDisplay, &myEglDisplay, &eglDisplay); 
    myEglDisplay = eglDisplay;
LOGE(stderr, "after assign, m = %p and e = %p and ma = %p, me = %p\n",myEglDisplay, eglDisplay, &myEglDisplay, &eglDisplay); 
    return asJLong(eglDisplay);
}

JNIEXPORT jboolean JNICALL Java_hello_EGL_eglInitialize
    (JNIEnv *env, jclass clazz, jlong eglDisplayPtr) {
    EGLDisplay eglDisplay = asPtr(eglDisplayPtr);
    LOGE(stderr, "[GLASS] eglInitialize, egldislay = %ld or %p, address at %p\n", eglDisplayPtr, eglDisplay, &eglDisplay);

    EGLint major, minor;
    if (eglInitialize(eglDisplay, &major, &minor)) {
        return JNI_TRUE;
    } else {
        LOGE(stderr, "[GLASS] major issue! eglInitialize failed!!!\n\n\n");
        return JNI_FALSE;
    }
}

JNIEXPORT jboolean JNICALL Java_hello_EGL_eglBindAPI
    (JNIEnv *env, jclass clazz, jint api) {
    // if (eglBindAPI(api)) {
    return JNI_TRUE;
}

int setEGLAttrs(int *eglAttrs) {
    int index = 0;
    eglAttrs[index++] = EGL_SURFACE_TYPE;
    eglAttrs[index++] = EGL_WINDOW_BIT;
    eglAttrs[index++] = EGL_RED_SIZE;
    eglAttrs[index++] = 8;
    eglAttrs[index++] = EGL_GREEN_SIZE;
    eglAttrs[index++] = 8;
    eglAttrs[index++] = EGL_BLUE_SIZE;
    eglAttrs[index++] = 8;
    eglAttrs[index++] = EGL_ALPHA_SIZE;
    eglAttrs[index++] = 8;
    eglAttrs[index++] = EGL_DEPTH_SIZE;
    eglAttrs[index++] = 16;
    eglAttrs[index++] = EGL_RENDERABLE_TYPE;
    eglAttrs[index++] = EGL_OPENGL_ES2_BIT;
    eglAttrs[index] = EGL_NONE;
    return index;
}


JNIEXPORT jboolean JNICALL Java_hello_EGL_eglChooseConfig
    (JNIEnv *env, jclass clazz, jlong eglDisplayPtr, jintArray attribs,
     jlongArray configs, jint configSize, jintArray numConfigs) {
    LOGE(stderr, "[GLASS] eglChooseConfig, env at %p, configSize = %d, edlDisplayPtr = %p at %p\n", env, configSize, asPtr(eglDisplayPtr), &eglDisplayPtr);
    EGLDisplay eglDisplay = asPtr(eglDisplayPtr);
    int i=0;

    int eglAttrs[50]; // value, attr pair plus a None

    setEGLAttrs(eglAttrs);
    EGLConfig *configArray = malloc(sizeof(EGLConfig) * configSize);
    jlong *longConfigArray = malloc(sizeof(long) * configSize);
    EGLint numConfigPtr=0;
    jboolean retval;

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
myEglConfig = configArray[0];
LOGE(stderr, "myEglConfig = %p at address %p\n", myEglConfig, &myEglConfig);
    free(configArray);
    free(longConfigArray);
LOGE(stderr, "GLERR after config?  %d\n",eglGetError());
    return retval;
}

JNIEXPORT jlong JNICALL Java_hello_EGL__1eglCreateWindowSurface
    (JNIEnv *env, jclass clazz, jlong eglDisplayPtr, jlong eglConfigPtr,
     jlong nativeWindow) {

     LOGE(stderr, "[JNIEGL] createWindowSurface, env at %p, nativewindow at %lx\n", env, nativeWindow);
#ifdef ALL_NATIVE
EGLDisplay eglDisplay = myEglDisplay;
EGLConfig eglConfig = myEglConfig;
#else
EGLDisplay eglDisplay = (EGLDisplay)eglDisplayPtr;
EGLConfig eglConfig = (EGLConfig)eglConfigPtr;
#endif
LOGE(stderr, "[JNIEGL] provided display at %p and provided config at %p with addresses %p and %p\n", eglDisplay, eglConfig, &eglDisplay, &eglConfig);
    EGLSurface eglSurface;

    eglSurface =  eglCreateWindowSurface(eglDisplay, eglConfig,
                                         (EGLNativeWindowType) nativeWindow,
                                         (EGLint *) NULL);
    LOGE(stderr, "EGL Surface create at %p, errorcode is %d \n", eglSurface, eglGetError());
    myEglSurface = eglSurface;
#ifdef ALL_NATIVE
    LOGE(stderr, "BYPASS MODUS! eglCreateSurface done, now go to eglCreateContext");
Java_hello_EGL_eglCreateContext(env, clazz, eglDisplayPtr, eglConfigPtr);
#endif
    return asJLong(eglSurface);
}

JNIEXPORT jlong JNICALL Java_hello_EGL_eglCreateContext
    (JNIEnv *env, jclass clazz, jlong eglDisplayPtr, jlong eglConfigPtr) {

    LOGE(stderr, "eglCreateContext with env %p\n", env);
#ifdef ALL_NATIVE
    EGLDisplay eglDisplay = myEglDisplay;
    EGLConfig eglConfig = myEglConfig;
#else
    EGLDisplay eglDisplay = (EGLDisplay)eglDisplayPtr;
    EGLConfig eglConfig = (EGLConfig)eglConfigPtr;
#endif

    LOGE(stderr, "EGLcretecontext, egldisplay at %p and config %p\n", eglDisplay, eglConfig);
    // we don't support any passed-in context attributes presently
    // we don't support any share context presently
    EGLint contextAttrs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
    EGLContext context = eglCreateContext(eglDisplay, eglConfig,
                                          NULL, contextAttrs);
    LOGE(stderr, "eglContext created at %p\n", context);
    LOGE(stderr, "GLERR?  %d\n",eglGetError());

    myEglContext = context;
#ifdef ALL_NATIVE
    LOGE(stderr, "BYPASS MODUS! eglCreateContext done, now go to eglMakeCurrent");
    Java_hello_EGL_eglMakeCurrent(env, clazz, eglDisplayPtr, asJLong(myEglSurface), asJLong(myEglSurface), asJLong(context));
#endif
    return asJLong(context);
}

JNIEXPORT jboolean JNICALL Java_hello_EGL_eglMakeCurrent
   (JNIEnv *env, jclass clazz, jlong eglDisplayPtr, jlong drawSurfacePtr,
    jlong readSurfacePtr, jlong eglContextPtr) {
    LOGE(stderr, "eglMakeCurrent with env %p\n", env);
#ifdef ALL_NATIVE
    EGLDisplay eglDisplay = myEglDisplay;
    EGLSurface eglDrawSurface = myEglSurface;
    EGLSurface eglReadSurface = myEglSurface;
    EGLContext eglContext = myEglContext;
#else 
    EGLDisplay eglDisplay = asPtr(eglDisplayPtr);
    EGLSurface eglDrawSurface = asPtr(drawSurfacePtr);
    EGLSurface eglReadSurface = asPtr(readSurfacePtr);
    EGLContext eglContext = asPtr(eglContextPtr);
#endif
    LOGE(stderr, "EGL MakeCurrent! disp = %p, surface = %p, readS = %p, context = %p\n", eglDisplay, eglDrawSurface, eglReadSurface, eglContext);
    LOGE(stderr, "EGL MakeCurrent! adddisp = %p, addsurface = %p, addreadS = %p, addcontext = %p\n", &eglDisplay, &eglDrawSurface, &eglReadSurface, &eglContext);
    LOGE(stderr, "GLERR?  %d\n",eglGetError());


    // if (eglMakeCurrent(eglDisplay, eglDrawSurface, eglReadSurface, eglContext)) {
    if (eglMakeCurrent(myEglDisplay, eglDrawSurface, eglReadSurface, eglContext)) {
        LOGE(stderr, "EGLMAKECURRENT SUCCEEDED\n");
        return JNI_TRUE;
    } else {
        LOGE(stderr, "eglMakeCurrent failed\n");
        return JNI_FALSE;
    }
}
