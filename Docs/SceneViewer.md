# SceneViewer – Build Guide

## Windows

SceneViewer can be generated and built using the same workflow as ResourceEditor. No additional configuration is required for Windows builds.

---

## Android

SceneViewer can also be built for Android.

During the integration process, several Gradle configuration files were updated to ensure compatibility with modern Android build tooling.

> ⚠️ **These changes may affect other Android projects within the framework. They should therefore be reviewed carefully before being merged or reused.**

### Build command

The Android build has been successfully tested on Ubuntu using:

```bash
./gradlew :SceneViewer:assembleDebug
```

The generated APK will be available in the standard Android build output directory.

### Android SDK configuration

To define the Android SDK path, create the following file:

```
dava.revanced/Programs/SceneViewer/Platforms/Android/local.properties
```

With the following content (change path if necessary):

```properties
sdk.dir=/home/user/Android/Sdk
```

### Build environment

The current configuration was tested with:

| Component       | Version                            |
|-----------------|------------------------------------|
| Java            | OpenJDK 11 (11.0.30, 2026-01-20)   |
| Android NDK     | 16.1.4479499 *(may vary)*          |
| Android SDK     | API 33 *(may vary)*                |

---

## Notes

- The APK was built on Ubuntu and tested on an Android ARM device.
- Gradle configuration adjustments may be required depending on the versions of:
  - Android SDK
  - Gradle
  - Android Gradle Plugin
- Any changes made for Android compatibility in SceneViewer should be tested against other engine Android projects.
