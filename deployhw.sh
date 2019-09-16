export PROJ=`pwd`
export BUILDT=/opt/android-sdk/build-tools/27.0.3
export ndk=/opt/android-ndk
export ndk_target=android-21
export platform="aarch64-linux-android21"
export compiler="$ndk/toolchains/llvm/prebuilt/linux-x86_64/bin/clang"
rm -rf bin
rm -rf obj
rm -rf lib
mkdir bin
mkdir obj
mkdir -p lib/arm64-v8a
mkdir -p graalobj

$compiler -target $platform -c -o graalobj/StrictMath.o  src/native/StrictMath.c 
$compiler -target $platform -c -o graalobj/graallauncher.o  src/native/graallauncher.c 
$compiler -target $platform -fPIC -Wl,--gc-sections -o lib/arm64-v8a/libmygraal.so  -Lbinariesfornow -landroid -llog -lz -lstrictmath -shared  binariesfornow/hello.helloworld.o graalobj/StrictMath.o graalobj/graallauncher.o
cp binariesfornow/libstrictmath.so lib/arm64-v8a

/opt/jdk1.8.0_162/bin/javac -d obj/ -source 1.7 -target 1.7 -cp src/dalvik -bootclasspath /opt/android-sdk/platforms/android-25/android.jar src/dalvik/com/gluonhq/helloandroid/*.java
~/android/android-sdk-linux/build-tools/23.0.1/dx --dex --output=$PROJ/bin/classes.dex $PROJ/obj
aapt package -f -m -F bin/hello.unaligned.apk -M AndroidManifest.xml -I ~/android/android-sdk-linux/platforms/android-21/android.jar
cp bin/classes.dex .
$BUILDT/aapt add bin/hello.unaligned.apk classes.dex
$BUILDT/aapt add bin/hello.unaligned.apk lib/**/*
$BUILDT/zipalign -f 4 bin/hello.unaligned.apk bin/hello.apk
$BUILDT/apksigner sign --ks ~/android.keystore bin/hello.apk
$BUILDT/../../platform-tools/adb install -r bin/hello.apk
