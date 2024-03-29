#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


#include <errno.h>
#include <android/native_window_jni.h>
#include "grandroid.h"

#define  LOG_TAG "GraalGluon"

#define  LOGD(ignore, ...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  LOGE(ignore, ...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern void *IsolateEnterStub__JavaMainWrapper__run__5087f5482cc9a6abc971913ece43acb471d2631b__a61fe6c26e84dd4037e4629852b5488bfcc16e7e();

ANativeWindow *window;
jfloat density;

int start_logger(const char *app_name);
static int pfd[2];
static pthread_t thr;
static const char *tag = "myapp";

// === called from DALVIK. Minize work/dependencies here === // 

JNIEXPORT void JNICALL Java_com_gluonhq_helloandroid_MainActivity_startGraalApp
(JNIEnv *env, jobject activity) {
    LOGE(stderr, "Start GraalApp, env at %p pointing to %p\n", env, *env);
int ev = (*env)->GetVersion(env);
LOGE(stderr, "EV = %d\n", ev);
    start_logger("GraalCompiled");
    (*IsolateEnterStub__JavaMainWrapper__run__5087f5482cc9a6abc971913ece43acb471d2631b__a61fe6c26e84dd4037e4629852b5488bfcc16e7e)(1);
}

JNIEXPORT void JNICALL Java_com_gluonhq_helloandroid_MainActivity_nativeSetSurface
(JNIEnv *env, jobject activity, jobject surface) {
    LOGE(stderr, "nativeSetSurface called, env at %p and size %ld, surface at %p\n", env, sizeof(JNIEnv), surface);
    window = ANativeWindow_fromSurface(env, surface);
    LOGE(stderr, "native setSurface Ready, native window at %p\n", window);
}

JNIEXPORT jlong JNICALL Java_com_gluonhq_helloandroid_MainActivity_surfaceReady
(JNIEnv *env, jobject activity, jobject surface, jfloat mydensity) {
    LOGE(stderr, "SurfaceReady, surface at %p\n", surface);
    window = ANativeWindow_fromSurface(env, surface);
    LOGE(stderr, "SurfaceReady, native window at %p\n", window);
    density = mydensity;
    return (jlong)window;
}

// == expose window functionality to JavaFX native code == //

ANativeWindow* getNativeWindow() {
    return window;
}

// ===== loading native JavaFX libs ===== //

JNIEXPORT jint JNICALL JNI_OnLoad_prism_es2_monocle(JavaVM *vm, void *reserved) {
    LOGE(stderr, "Loading asked for prism_es2_monocle\n");
    return JNI_VERSION_1_8;
}

JNIEXPORT jint JNICALL JNI_OnLoad_glass_monocle(JavaVM *vm, void *reserved) {
    LOGE(stderr, "Loading asked for glass_monocle\n");
    return JNI_VERSION_1_8;
}

JNIEXPORT jint JNICALL JNI_OnLoad_glass_android(JavaVM *vm, void *reserved) {
    LOGE(stderr, "Loading asked for glass_android\n");
    return JNI_VERSION_1_8;
}


// ======== missing functions ==== //

int * __errno_location(void) {
    int *a = &errno;
    return a;
}

void getEnviron() {
    LOGE(stderr, "\n\ngetEnviron NYI\n\n");
}

// we need this and the start_logger since android eats fprintf
static void *thread_func()
{
    ssize_t rdsz;
    char buf[128];
    while((rdsz = read(pfd[0], buf, sizeof buf - 1)) > 0) {
        if(buf[rdsz - 1] == '\n') --rdsz;
        buf[rdsz] = 0;  /* add null-terminator */
        __android_log_write(ANDROID_LOG_DEBUG, tag, buf);
        // __android_log_print(ANDROID_LOG_DEBUG, tag, buf);
    }
    return 0;
}

int start_logger(const char *app_name)
{
    tag = app_name;

    /* make stdout line-buffered and stderr unbuffered */
    setvbuf(stdout, 0, _IOLBF, 0);
    setvbuf(stderr, 0, _IONBF, 0);

    /* create the pipe and redirect stdout and stderr */
    pipe(pfd);
    dup2(pfd[1], 1);
    dup2(pfd[1], 2);

    /* spawn the logging thread */
    if(pthread_create(&thr, 0, thread_func, 0) == -1)
        return -1;
    pthread_detach(thr);
    return 0;
}


