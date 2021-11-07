# Edopro-android

## Building & Installation
Assuming `ndk-build` is in your PATH
```
ndk-build
```

Then you can build with `gradlew` 

```
./gradlew build
```

You can also directly install to a plugged Android device
```
./gradlew installDebug
```

You can also open the project in Android Studio and build the app from there.

## Note
All the prebuilt libraries have been built with android NDK r21d, so they might not work properly if the main project is built using a different NDK version
