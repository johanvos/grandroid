JNIEXPORT jlong JNICALL Java_hello_EGL_eglCreateContext (JNIEnv *env, jclass clazz); //, jlong eglDisplayPtr, jlong eglConfigPtr);
JNIEXPORT jboolean JNICALL Java_hello_EGL_eglMakeCurrent (JNIEnv *env, jclass clazz, jlong eglDisplayPtr, jlong drawSurfacePtr, jlong readSurfacePtr, jlong eglContextPtr);

