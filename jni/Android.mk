LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE    := standoff2esp
LOCAL_CFLAGS    := -std=c++17 -O2 -fPIE -w
LOCAL_LDFLAGS   := -fPIE -pie -lEGL -lGLESv3 -landroid -llog
LOCAL_LDLIBS    := -lEGL -lGLESv3 -landroid -llog

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../includes/internal \
    $(LOCAL_PATH)/../includes/internal/ImGui \
    $(LOCAL_PATH)/../includes/internal/ImGui/backends \
    $(LOCAL_PATH)/../includes/internal/Android_draw \
    $(LOCAL_PATH)/../includes/internal/Android_touch \
    $(LOCAL_PATH)/../includes/internal/native_surface \
    $(LOCAL_PATH)/../src

LOCAL_SRC_FILES := \
    ../src/main.cpp \
    ../src/func/visuals.cpp \
    ../src/protect/oxorany.cpp \
    ../src/ui/menu.cpp \
    ../src/ui/bar.cpp \
    ../src/ui/widgets/widgets.cpp \
    ../includes/draw/Android_draw/draw.cpp \
    ../includes/draw/Android_touch/Touch.cpp \
    ../includes/draw/ImGui/imgui.cpp \
    ../includes/draw/ImGui/imgui_draw.cpp \
    ../includes/draw/ImGui/imgui_tables.cpp \
    ../includes/draw/ImGui/imgui_widgets.cpp \
    ../includes/draw/ImGui/backends/imgui_impl_android.cpp \
    ../includes/draw/ImGui/backends/imgui_impl_opengl3.cpp

include $(BUILD_EXECUTABLE)
