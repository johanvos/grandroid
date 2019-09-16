# grandroid
Real Java on AArch64

Hacking.

MainActivity is a java class running on the Dalvik/ART/Android platform. Hence, it needs to be compiled with the appropriate compiler (sic)

It calls into graallauncher.c which will launch the graal-compiled java app. This is the one that matters (not the Android activity)

The pre-compiled app is included in binariesfornow: hello.helloworld.o

Source of this class:

```
package hello;
  
public class HelloWorld {

    public static void main(String[] args) {
        String javaVersion = System.getProperty("java.version");
        System.out.println("Hello World on Graal, javaVersion = "+javaVersion);
    }

}
```

Compiled on an AArch64 Linux with the following command:
```
export GRAAL=/home/ubuntu/graal/vm/mxbuild/linux-aarch64/GRAALVM_SVM_BGU/graalvm-svm-19.3.0-dev
$GRAAL/bin/native-image \
	--no-fallback -H:TempDirectory=tmphw \
       	-H:+SpawnIsolates \
	-H:+ReportExceptionStackTraces \
	-H:+AllowIncompleteClasspath \
	-cp $CP \
	hello.HelloWorld
  ```
  
  As a result, tmphw contains a file hello.helloworld.o which is the one that is here placed in the binariesfornow directory
  
  Create a keystore:
  ```
  /opt/java/bin/keytool -genkey -alias server-alias -keyalg RSA -keypass foofoo -storepass barbar -keystore ~/android.keystore.jks
```

When asked for password, enter `barbar`

run `adb logcat -v threadtime` and start the app, you should see something like this:

```
09-16 13:34:11.792  3718  3718 W System.err: Hello WORLD ACTIVITY, onCreate called
09-16 13:34:11.818  3718  3733 D OpenGLRenderer: HWUI GL Pipeline
09-16 13:34:11.842  3718  3733 I Adreno  : QUALCOMM build                   : 8e59954, I0be83d0d26
09-16 13:34:11.842  3718  3733 I Adreno  : Build Date                       : 09/22/17
09-16 13:34:11.842  3718  3733 I Adreno  : OpenGL ES Shader Compiler Version: EV031.21.02.00
09-16 13:34:11.842  3718  3733 I Adreno  : Local Branch                     : O17A
09-16 13:34:11.842  3718  3733 I Adreno  : Remote Branch                    : 
09-16 13:34:11.842  3718  3733 I Adreno  : Remote Branch                    : 
09-16 13:34:11.842  3718  3733 I Adreno  : Reconstruct Branch               : 
09-16 13:34:11.843  3718  3733 D vndksupport: Loading /vendor/lib64/hw/gralloc.msm8992.so from current namespace instead of sphal namespace.
09-16 13:34:11.852  3718  3718 V GraalActivity: [MainActivity] Surface created in activity.
09-16 13:34:11.852  3718  3718 W System.err: loading lib
09-16 13:34:11.852  3718  3733 I Adreno  : PFP: 0x00000000, ME: 0x00000000
09-16 13:34:11.862   361   361 W /system/bin/hwservicemanager: getTransport: Cannot find entry android.hardware.configstore@1.0::ISurfaceFlingerConfigs/default in either framework or device manifest.
09-16 13:34:11.863  3718  3718 W System.err: loaded lib
09-16 13:34:11.863  3718  3718 W System.err: metrics = DisplayMetrics{density=2.625, width=1080, height=1794, scaledDensity=2.625, xdpi=422.03, ydpi=424.069}
09-16 13:34:11.863  3718  3718 W System.err: DENSITY??? 2.625
09-16 13:34:11.863  3718  3718 E GraalGluon: SurfaceReady, native window at 0x0
09-16 13:34:11.864  3718  3718 W System.err: Surface created, native code informed about it.
09-16 13:34:11.864  3718  3718 V GraalActivity: Surface created, native code informed about it.
09-16 13:34:11.864  3718  3718 V GraalActivity: [MainActivity] surfaceChange, format = 4, width = 1080, height = 1731
09-16 13:34:11.864  3718  3718 W System.err: [MainActivity] surfaceChange, format = 4, width = 1080, height = 1731
09-16 13:34:11.864  3718  3718 W System.err: metrics = DisplayMetrics{density=2.625, width=1080, height=1794, scaledDensity=2.625, xdpi=422.03, ydpi=424.069}
09-16 13:34:11.864  3718  3718 W System.err: DENSITY??? 2.625
09-16 13:34:11.864  3718  3718 W System.err: [MainActivity] surfaceRedrawNeeded, we will launch our app now
09-16 13:34:11.864  3718  3733 I zygote64: android::hardware::configstore::V1_0::ISurfaceFlingerConfigs::hasWideColorDisplay retrieved: 0
09-16 13:34:11.866  3718  3733 I OpenGLRenderer: Initialized EGL, version 1.4
09-16 13:34:11.866  3718  3733 D OpenGLRenderer: Swap behavior 2
09-16 13:34:11.869  3718  3734 E GraalGluon: Start GraalApp!
09-16 13:34:11.870  3718  3735 D GraalCompiled: ALLOCATE MEMORY asked for 1048576
09-16 13:34:11.871  3718  3735 D GraalCompiled: Hello World on Graal, javaVersion = 11.0.4
```
