#!/system/bin/sh
# ============================================
# Standoff 2 ESP — сборка и запуск
# Открой этот файл через MT Manager -> Terminal
# или через Termux
# ============================================

PROJ="$(cd "$(dirname "$0")" && pwd)"
BIN="$PROJ/libs/arm64-v8a/standoff2esp"
INSTALL="/data/local/tmp/standoff2esp"

# --- поиск NDK ---
find_ndk() {
    for p in \
        "$ANDROID_NDK_HOME" \
        /data/user/0/com.aide.clone/no_backup/ndksupport-*/android-ndk-aide \
        /data/user/0/com.aide.web/no_backup/ndksupport-*/android-ndk-aide \
        /data/data/com.termux/files/home/android-ndk* \
        /sdcard/android-ndk* \
        /sdcard/Download/android-ndk* \
        "$HOME/android-ndk"
    do
        [ -d "$p" ] && [ -f "$p/ndk-build" ] && echo "$p" && return 0
    done
    return 1
}

NDK_PATH="$(find_ndk)"
if [ -z "$NDK_PATH" ]; then
    echo "[!] NDK не найден"
    echo ""
    echo "Установи одним из способов:"
    echo "  1) AIDE: Settings -> Build -> NDK Support"
    echo "  2) Termux: pkg install ndk-multilib"
    echo "  3) Скачай NDK и кинь в /sdcard/android-ndk"
    echo ""
    echo "Или укажи путь вручную:"
    echo "  ANDROID_NDK_HOME=/путь/к/ndk sh $0"
    exit 1
fi

echo "[*] NDK: $NDK_PATH"
echo "[*] Проект: $PROJ"
echo ""

# --- сборка ---
cd "$PROJ"
"$NDK_PATH/ndk-build" \
    NDK_PROJECT_PATH=. \
    APP_BUILD_SCRIPT=jni/Android.mk \
    NDK_APPLICATION_MK=jni/Application.mk \
    -j4

echo ""

if [ ! -f "$BIN" ]; then
    echo "[X] Сборка ПРОВАЛЕНА"
    exit 1
fi

echo "[OK] Собрано: $BIN"
echo ""

# --- установка и запуск ---
echo "[*] Копирую в $INSTALL..."
su -c "cp '$BIN' '$INSTALL' && chmod 755 '$INSTALL'"

if [ $? -ne 0 ]; then
    echo "[!] Нет рута. Скопируй вручную:"
    echo "  su"
    echo "  cp $BIN $INSTALL"
    echo "  chmod 755 $INSTALL"
    echo "  $INSTALL"
    exit 1
fi

echo "[*] Запускаю... Открой Standoff 2"
echo "[*] Для остановки: su -c 'killall standoff2esp'"
echo ""
su -c "$INSTALL" &

echo "[OK] Запущен в фоне (PID: $!)"
echo "[*] Меню появится поверх игры"
