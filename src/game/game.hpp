#pragma once
#include <cstdint>
#include "../other/memory.hpp"

// Standoff 2 — Unity il2cpp (com.axlebolt.standoff2)
// Офсеты меняются каждое обновление. Дампь через il2cppdumper или GameGuardian.
// Процедура: взять libil2cpp.so из /data/app/.../lib/arm64/, прогнать через
// il2cppdumper (или Frida il2cpp bridge), найти классы ниже в dump.cs

namespace game {

constexpr const char* PACKAGE = "com.axlebolt.standoff2";
constexpr const char* LIB_NAME = "libil2cpp.so";

// ========================
// OFFSETS — FILL из il2cppdumper
// Дампь: il2cppdumper libil2cpp.so global-metadata.dat
// Ищи в dump.cs классы ниже
// ========================

namespace offsets {
    // FPSController (или PlayerController) — основной класс игрока
    // dump.cs: class FPSController : MonoBehaviour
    constexpr uintptr_t FPSController_TypeInfo = 0x0;  // FILL: grep "FPSController_TypeInfo" в il2cpp output
    constexpr uintptr_t FPSController_Health = 0x0;     // FILL: поле health в FPSController
    constexpr uintptr_t FPSController_Position = 0x0;   // FILL: transform.position или cached pos
    constexpr uintptr_t FPSController_Team = 0x0;       // FILL: поле team (int)
    constexpr uintptr_t FPSController_IsAlive = 0x0;    // FILL: поле isAlive (bool)
    constexpr uintptr_t FPSController_Nickname = 0x0;   // FILL: поле nickname (string)

    // Camera
    constexpr uintptr_t Camera_Main = 0x0;              // FILL: Camera.main singleton
    constexpr uintptr_t Camera_ViewMatrix = 0x0;        // FILL: viewProjectionMatrix

    // PlayerList / GameManager
    constexpr uintptr_t GameManager_Instance = 0x0;     // FILL: GameManager singleton
    constexpr uintptr_t GameManager_PlayerList = 0x0;   // FILL: List<FPSController> players
    constexpr uintptr_t List_Count = 0x18;              // System.Collections.Generic.List<T>._size
    constexpr uintptr_t List_Items = 0x10;              // System.Collections.Generic.List<T>._items

    // Weapon
    constexpr uintptr_t Weapon_NoRecoil = 0x0;          // FILL: recoil float field
    constexpr uintptr_t Weapon_NoSpread = 0x0;          // FILL: spread float field
    constexpr uintptr_t Weapon_FireRate = 0x0;           // FILL: fireRate float field
}

struct Vector3 {
    float x, y, z;
    Vector3 operator-(const Vector3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    Vector3 operator+(const Vector3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    float length() const { return sqrtf(x * x + y * y + z * z); }
};

struct Matrix4x4 {
    float m[4][4];
};

inline uintptr_t il2cpp_base = 0;

inline bool init() {
    if (!mem::init(PACKAGE)) return false;
    il2cpp_base = mem::get_module_base(mem::game_pid, LIB_NAME);
    return il2cpp_base != 0;
}

inline bool world_to_screen(const Vector3& world, const Matrix4x4& vm, float sw, float sh, Vector3& out) {
    float w = vm.m[0][3] * world.x + vm.m[1][3] * world.y + vm.m[2][3] * world.z + vm.m[3][3];
    if (w < 0.001f) return false;
    float x = vm.m[0][0] * world.x + vm.m[1][0] * world.y + vm.m[2][0] * world.z + vm.m[3][0];
    float y = vm.m[0][1] * world.x + vm.m[1][1] * world.y + vm.m[2][1] * world.z + vm.m[3][1];
    out.x = (sw / 2.f) + (sw / 2.f) * x / w;
    out.y = (sh / 2.f) - (sh / 2.f) * y / w;
    out.z = w;
    return true;
}

// Читает список игроков. Возвращает количество.
inline int get_player_count() {
    uintptr_t mgr = mem::read<uintptr_t>(il2cpp_base + offsets::GameManager_Instance);
    if (!mgr) return 0;
    uintptr_t list = mem::read<uintptr_t>(mgr + offsets::GameManager_PlayerList);
    if (!list) return 0;
    return mem::read<int>(list + offsets::List_Count);
}

inline uintptr_t get_player(int idx) {
    uintptr_t mgr = mem::read<uintptr_t>(il2cpp_base + offsets::GameManager_Instance);
    if (!mgr) return 0;
    uintptr_t list = mem::read<uintptr_t>(mgr + offsets::GameManager_PlayerList);
    if (!list) return 0;
    uintptr_t items = mem::read<uintptr_t>(list + offsets::List_Items);
    if (!items) return 0;
    return mem::read<uintptr_t>(items + 0x20 + idx * 0x8); // array items start at +0x20
}

inline Vector3 get_position(uintptr_t player) {
    return mem::read<Vector3>(player + offsets::FPSController_Position);
}

inline int get_team(uintptr_t player) {
    return mem::read<int>(player + offsets::FPSController_Team);
}

inline bool is_alive(uintptr_t player) {
    return mem::read<bool>(player + offsets::FPSController_IsAlive);
}

inline float get_health(uintptr_t player) {
    return mem::read<float>(player + offsets::FPSController_Health);
}

inline Matrix4x4 get_view_matrix() {
    uintptr_t cam = mem::read<uintptr_t>(il2cpp_base + offsets::Camera_Main);
    if (!cam) return {};
    return mem::read<Matrix4x4>(cam + offsets::Camera_ViewMatrix);
}

} // namespace game
