# grandroid
Real Java on AArch64

Hacking.

MainActivity is a java class running on the Dalvik/ART/Android platform. Hence, it needs to be compiled with the appropriate compiler (sic)

It calls into graallauncher.c which will launch the graal-compiled java app.

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
  
