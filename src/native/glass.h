JNIEXPORT jlong JNICALL Java_com_sun_glass_ui_monocle_EGL_eglCreateContext (JNIEnv *env, jclass clazz, jlong eglDisplayPtr, jlong eglConfigPtr, jlong shareContext, jintArray attribs);
JNIEXPORT jboolean JNICALL Java_com_sun_glass_ui_monocle_EGL_eglMakeCurrent (JNIEnv *env, jclass clazz, jlong eglDisplayPtr, jlong drawSurfacePtr, jlong readSurfacePtr, jlong eglContextPtr);

