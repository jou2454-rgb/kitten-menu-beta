#!/data/data/com.termux/files/usr/bin/sh
# ============================================
# Standoff 2 ESP — сборка через Termux clang++
# Не требует ndk-build, только pkg install ndk-multilib
# ============================================

PROJ="$(cd "$(dirname "$0")" && pwd)"
OUT="$PROJ/libs/arm64-v8a"
BIN="$OUT/standoff2esp"
INSTALL="/data/local/tmp/standoff2esp"

CC="clang++"

# проверка clang++
if ! command -v $CC >/dev/null 2>&1; then
    echo "[!] clang++ не найден"
    echo "Установи: pkg install clang"
    exit 1
fi

echo "[*] Компилятор: $(which $CC)"
echo "[*] Проект: $PROJ"
echo ""

# --- include пути ---
INCLUDES="\
 -I$PROJ/includes/internal \
 -I$PROJ/includes/internal/ImGui \
 -I$PROJ/includes/internal/ImGui/backends \
 -I$PROJ/includes/internal/Android_draw \
 -I$PROJ/includes/internal/Android_touch \
 -I$PROJ/includes/internal/native_surface \
 -I$PROJ/includes/fonts \
 -I$PROJ/src"

# --- исходники ---
SRCS="\
 $PROJ/src/main.cpp \
 $PROJ/src/func/visuals.cpp \
 $PROJ/src/protect/oxorany.cpp \
 $PROJ/src/ui/menu.cpp \
 $PROJ/src/ui/bar.cpp \
 $PROJ/src/ui/widgets/widgets.cpp \
 $PROJ/includes/draw/Android_draw/draw.cpp \
 $PROJ/includes/draw/Android_touch/Touch.cpp \
 $PROJ/includes/draw/ImGui/imgui.cpp \
 $PROJ/includes/draw/ImGui/imgui_draw.cpp \
 $PROJ/includes/draw/ImGui/imgui_tables.cpp \
 $PROJ/includes/draw/ImGui/imgui_widgets.cpp \
 $PROJ/includes/draw/ImGui/backends/imgui_impl_android.cpp \
 $PROJ/includes/draw/ImGui/backends/imgui_impl_opengl3.cpp"

# --- флаги ---
CFLAGS="-std=c++17 -O2 -fPIE -w -DANDROID -D__ANDROID_API__=26"
LDFLAGS="-fPIE -pie -lEGL -lGLESv3 -llog -landroid -static-libstdc++"

# --- сборка ---
mkdir -p "$OUT"

echo "[*] Компиляция..."
$CC $CFLAGS $INCLUDES $SRCS -o "$BIN" $LDFLAGS 2>&1

if [ $? -ne 0 ]; then
    echo ""
    echo "[X] Сборка ПРОВАЛЕНА"
    exit 1
fi

echo ""
echo "[OK] Собрано: $BIN"
echo ""

# --- установка и запуск ---
echo "[*] Копирую в $INSTALL..."
su -c "cp '$BIN' '$INSTALL' && chmod 755 '$INSTALL'"

if [ $? -ne 0 ]; then
    echo "[!] Нет рута. Запусти вручную:"
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
