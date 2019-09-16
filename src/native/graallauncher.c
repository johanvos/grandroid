#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


#include <errno.h>
#include <android/native_window_jni.h>
#include <android/log.h>

#define  LOG_TAG "GraalGluon"

#define  LOGD(ignore, ...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  LOGE(ignore, ...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern void *IsolateEnterStub__JavaMainWrapper__run__5087f5482cc9a6abc971913ece43acb471d2631b__a61fe6c26e84dd4037e4629852b5488bfcc16e7e();

ANativeWindow *window;
jfloat density;

static int pfd[2];
static pthread_t thr;
static const char *tag = "myapp";

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



JNIEXPORT void JNICALL Java_com_gluonhq_helloandroid_MainActivity_startGraalApp
(JNIEnv *env, jobject activity) {
    LOGE(stderr, "Start GraalApp!\n");
    start_logger("GraalCompiled");
    (*IsolateEnterStub__JavaMainWrapper__run__5087f5482cc9a6abc971913ece43acb471d2631b__a61fe6c26e84dd4037e4629852b5488bfcc16e7e)(1);
}

JNIEXPORT jlong JNICALL Java_com_gluonhq_helloandroid_MainActivity_surfaceReady
(JNIEnv *env, jobject activity, jobject surface, jfloat mydensity) {
    LOGE(stderr, "SurfaceReady, native window at %p\n", window);
    window   = ANativeWindow_fromSurface(env, surface);
    density = mydensity;
    return (jlong)window;
}


int * __errno_location(void) {
    int *a = &errno;
    return a;
}

