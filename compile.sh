#!/bin/bash
# Сборка на ПК: установи NDK, укажи путь
# https://developer.android.com/ndk/downloads

NDK_PATH="${ANDROID_NDK_HOME:-$HOME/android-ndk}"

if [ ! -d "$NDK_PATH" ]; then
    echo "NDK not found at $NDK_PATH"
    echo "Set ANDROID_NDK_HOME or put NDK in ~/android-ndk"
    exit 1
fi

cd "$(dirname "$0")"
"$NDK_PATH/ndk-build" \
    NDK_PROJECT_PATH=. \
    APP_BUILD_SCRIPT=jni/Android.mk \
    NDK_APPLICATION_MK=jni/Application.mk \
    -j$(nproc)

echo ""
if [ -f libs/arm64-v8a/standoff2esp ]; then
    echo "Build OK: libs/arm64-v8a/standoff2esp"
    echo ""
    echo "Deploy:"
    echo "  adb push libs/arm64-v8a/standoff2esp /data/local/tmp/"
    echo "  adb shell chmod +x /data/local/tmp/standoff2esp"
    echo "  adb shell su -c /data/local/tmp/standoff2esp"
else
    echo "Build FAILED"
fi
