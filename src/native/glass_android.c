#include <jni.h>
#include <android/native_window_jni.h>
#include "grandroid.h"
#include <stdlib.h>

#include <EGL/egl.h>
#include <GLES/gl.h>
#include "glass.h"

// comment this out to see it fails (at least the first invocation of eglMakeCurrent
#define ALL_NATIVE 1

ANativeWindow* getNativeWindow();
ANativeWindow* myNativeWindow;
EGLSurface myEglSurface;
EGLContext myEglContext;
EGLDisplay myEglDisplay;
EGLConfig myEglConfig;

#define asPtr(x) ((void *) (unsigned long) (x))
#define asJLong(x) ((jlong) (unsigned long) (x))
EGLenum API = 0x30A0;
void makeGreen () ;


JNIEXPORT jlong JNICALL Java_hello_HelloWorld__1getNativeHandle
  (JNIEnv *jenv, jobject obj) {
    myNativeWindow = getNativeWindow();
    return asJLong(myNativeWindow);
}

JNIEXPORT jlong JNICALL Java_hello_EGL_eglGetAndInitializeDisplay
    (JNIEnv *jenv, jclass clazz, jlong display) {
    myEglDisplay = eglGetDisplay(((EGLNativeDisplayType) (unsigned long)(display)));
    LOGE(stderr, "[GLERR] display at %p with value %p, err after eglGetDisplay?  %d\n",&myEglDisplay, myEglDisplay, eglGetError());
    EGLint major, minor;
    if (eglInitialize(myEglDisplay, &major, &minor)) {
        LOGE(stderr, "[GLERR] after eglInitialize, major = %d and minor = %d, err after eglGetDisplay?  %d\n",major, minor, eglGetError());
        if (eglBindAPI(API)) {
        LOGE(stderr, "[GLERR] after err after bindAPI to %d?  %d\n",API, eglGetError());
            return asJLong(myEglDisplay);
        }
    }
    LOGE(stderr, "[GLERR] major error in getAndInitializeDisplay, return -1");
    return asJLong(EGL_NO_DISPLAY);
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


JNIEXPORT jboolean JNICALL Java_hello_EGL_eglChooseConfig (JNIEnv *jenv, jclass clazz) {
    int eglAttrs[50]; 
    EGLint configSize = 1;

    setEGLAttrs(eglAttrs);
    EGLConfig *configArray = malloc(sizeof(EGLConfig) * configSize);
    jlong *longConfigArray = malloc(sizeof(long) * configSize);
    EGLint numConfigPtr=0;
    jboolean retval;

    if (!eglChooseConfig(myEglDisplay, eglAttrs, configArray, configSize,
                               &numConfigPtr)) {
        retval = JNI_FALSE;
    } else {
        retval = JNI_TRUE;
    }
    myEglConfig = configArray[0];
    LOGE(stderr, "myEglConfig = %p at address %p\n", myEglConfig, &myEglConfig);
    free(configArray);
    free(longConfigArray);
    LOGE(stderr, "GLERR after config?  %d\n",eglGetError());
    return retval;
}

jint once = 0;

JNIEXPORT jlong JNICALL Java_hello_EGL__1eglCreateWindowSurface (JNIEnv* env, jclass clazz) {
    LOGE(stderr, "[JNIEGL] createWindowSurface, env at %p, nativewindow at %p\n", env, myNativeWindow);
    LOGE(stderr, "[JNIEGL] display at %p and config at %p with addresses %p and %p\n", myEglDisplay,
                                          myEglConfig, &myEglDisplay, &myEglConfig);
    myEglSurface =  eglCreateWindowSurface(myEglDisplay, myEglConfig,
                                         (EGLNativeWindowType) myNativeWindow,
                                         (EGLint *) NULL);
    LOGE(stderr, "EGL Surface create at %p, errorcode is %d \n", myEglSurface, eglGetError());
#ifdef ALL_NATIVE
    LOGE(stderr, "BYPASS MODUS 1! eglCreateSurface done, now go to eglCreateContext");
Java_hello_EGL_eglCreateContext(env, clazz); // , asJLong(myEglDisplay), asJLong(myEglConfig));
LOGE(stderr, "EGLCREATEwindowSurface will now return\n");
once = 1;
#endif
    return asJLong(myEglSurface);
}

JNIEXPORT jlong JNICALL Java_hello_EGL_eglCreateContext
    (JNIEnv *env, jclass clazz) { //, jlong eglDisplayPtr, jlong eglConfigPtr) {

if (once == 1) {
LOGE(stderr, "DON't CREATE CONTEXT\n");
 return asJLong(myEglContext);
}
    LOGE(stderr, "eglCreateContext with env %p\n", env);
    EGLDisplay eglDisplay = myEglDisplay;
    EGLConfig eglConfig = myEglConfig;

    LOGE(stderr, "EGLcretecontext, egldisplay at %p and config %p\n", eglDisplay, eglConfig);
    EGLint contextAttrs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
    EGLContext context = eglCreateContext(eglDisplay, eglConfig,
                                          NULL, contextAttrs);
    LOGE(stderr, "eglContext created at %p\n", context);
    LOGE(stderr, "GLERR?  %d\n",eglGetError());

    myEglContext = context;
#ifdef ALL_NATIVE
    LOGE(stderr, "BYPASS MODUS! eglCreateContext done, now go to eglMakeCurrent");
    Java_hello_EGL_eglMakeCurrent(env, clazz); // , asJLong(myEglDisplay), asJLong(myEglSurface), asJLong(myEglSurface), asJLong(context));
#endif
    return asJLong(context);
}

JNIEXPORT jboolean JNICALL Java_hello_EGL_eglMakeCurrent
   (JNIEnv *env, jclass clazz) { //  , jlong eglDisplayPtr, jlong drawSurfacePtr, jlong readSurfacePtr, jlong eglContextPtr) {
if (once == 1) {
 return asJLong(myEglContext);
}
    LOGE(stderr, "eglMakeCurrent with env %p\n", env);
    LOGE(stderr, "GLERR?  %d\n",eglGetError());

    if (eglMakeCurrent(myEglDisplay, myEglSurface, myEglSurface, myEglContext)) {
        LOGE(stderr, "EGLMAKECURRENT SUCCEEDED, show a green area\n");
        makeGreen();
        return JNI_TRUE;
    } else {
        LOGE(stderr, "eglMakeCurrent failed\n");
        return JNI_FALSE;
    }
}

void makeGreen () {
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f); // Set background color to black and opaque
    glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer

    glFlush();  // Render now
    LOGE(stderr, "make green, swapBuffers disp = %p and surf = %p\n", myEglDisplay, myEglSurface);
    eglSwapBuffers(myEglDisplay, myEglSurface);
}
