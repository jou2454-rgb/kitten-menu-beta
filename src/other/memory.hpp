#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <unistd.h>
#include <dirent.h>
#include <sys/uio.h>

namespace mem {

static pid_t game_pid = -1;

inline pid_t find_pid(const char* package) {
    DIR *dir = opendir("/proc");
    if (!dir) return -1;
    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (entry->d_type != DT_DIR) continue;
        int pid = atoi(entry->d_name);
        if (pid <= 0) continue;
        char path[256], buf[256];
        snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);
        FILE *f = fopen(path, "r");
        if (!f) continue;
        memset(buf, 0, sizeof(buf));
        fread(buf, 1, sizeof(buf) - 1, f);
        fclose(f);
        if (strcmp(buf, package) == 0) {
            closedir(dir);
            return pid;
        }
    }
    closedir(dir);
    return -1;
}

inline uintptr_t get_module_base(pid_t pid, const char* module) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/maps", pid);
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    char line[512];
    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, module)) {
            uintptr_t base = 0;
            sscanf(line, "%lx", &base);
            fclose(f);
            return base;
        }
    }
    fclose(f);
    return 0;
}

inline bool read_mem(pid_t pid, uintptr_t addr, void* buf, size_t len) {
    iovec local{buf, len};
    iovec remote{reinterpret_cast<void*>(addr), len};
    return process_vm_readv(pid, &local, 1, &remote, 1, 0) == (ssize_t)len;
}

template<typename T>
inline T read(uintptr_t addr) {
    T val{};
    read_mem(game_pid, addr, &val, sizeof(T));
    return val;
}

template<typename T>
inline bool write(uintptr_t addr, T val) {
    iovec local{&val, sizeof(T)};
    iovec remote{reinterpret_cast<void*>(addr), sizeof(T)};
    return process_vm_writev(game_pid, &local, 1, &remote, 1, 0) == (ssize_t)sizeof(T);
}

inline bool init(const char* package) {
    game_pid = find_pid(package);
    return game_pid > 0;
}

} // namespace mem
