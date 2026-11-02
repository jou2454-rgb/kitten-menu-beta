#pragma once
#include <android/native_window.h>
#include <dlfcn.h>
#include <cstdint>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>

namespace android {
    class ANativeWindowCreator {
    public:
        struct DisplayInfo {
            uint32_t width;
            uint32_t height;
            uint32_t orientation;
        };

        static DisplayInfo GetDisplayInfo() {
            DisplayInfo info{0};
            FILE *fp = popen("dumpsys display | grep -oP 'mBaseDisplayInfo=DisplayInfo\\{.+?real \\K[0-9]+ x [0-9]+'", "r");
            if (fp) {
                char buf[64];
                if (fgets(buf, sizeof(buf), fp)) {
                    sscanf(buf, "%u x %u", &info.width, &info.height);
                }
                pclose(fp);
            }
            if (info.width == 0 || info.height == 0) {
                fp = popen("wm size", "r");
                if (fp) {
                    char buf[64];
                    if (fgets(buf, sizeof(buf), fp)) {
                        char *x = strstr(buf, ":");
                        if (x) sscanf(x + 1, "%ux%u", &info.width, &info.height);
                    }
                    pclose(fp);
                }
            }
            if (info.width > info.height) info.orientation = 1;
            return info;
        }

        static ANativeWindow* Create(const char* name, uint32_t w, uint32_t h, bool secure) {
            using func_t = ANativeWindow*(*)(const char*, int32_t, int32_t, int32_t, int32_t, bool);
            void *handle = dlopen("libsurfaceflinger_client.so", RTLD_LAZY);
            if (!handle) handle = dlopen("libgui.so", RTLD_LAZY);
            if (!handle) {
                // fallback: SurfaceComposerClient via binder
                return CreateViaSurfaceControl(name, w, h);
            }
            return nullptr;
        }

    private:
        static ANativeWindow* CreateViaSurfaceControl(const char* name, uint32_t w, uint32_t h) {
            char cmd[256];
            snprintf(cmd, sizeof(cmd),
                "service call SurfaceFlinger 1 i32 1 i32 %u i32 %u i32 1 s16 %s",
                w, h, name);
            system(cmd);
            return nullptr;
        }
    };
}
