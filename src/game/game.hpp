#pragma once
#include <cstdint>
#include <cmath>
#include "../other/memory.hpp"

// Standoff 2 — Halloween 2026 (Unity 6)
// Unity 6: libil2cpp.so + libunity.so merged → single libunity.so (217MB)
// Offsets: offsets_remap_FINAL_1 (il2cppdump v16, ARM64 disasm verified)

namespace game {

constexpr const char* PACKAGE = "com.axlebolt.standoff2";
constexpr const char* LIB_NAME = "libunity.so";

// ========================
// TypeInfo addresses — class pointers in libunity.so
// Pattern: base + typeinfo_addr -> Il2CppClass* -> static_fields -> singleton
// ========================
namespace typeinfo {
    constexpr uintptr_t kPlayer              = 0xCD32658;  // odq (was ake)
    constexpr uintptr_t kNetObject           = 0xCD359A0;  // wwv (was mfc)
    constexpr uintptr_t kClientWorld         = 0xCD34DC8;  // vch (was njh)
    constexpr uintptr_t kRegistry            = 0xCD32028;  // nba (was gat)
    constexpr uintptr_t kStore               = 0xCD303E8;  // ibc (was gzx)
    constexpr uintptr_t kActor               = 0xCD34C50;  // uwa (was stn)
    constexpr uintptr_t kStats               = 0xCD360F0;  // ydm (was ylw)
    constexpr uintptr_t kGrenadeCim          = 0xCD2EDA8;  // edu (was cim)
    constexpr uintptr_t kCharacterBindings   = 0xCD1FA60;
    constexpr uintptr_t kBonesDictionary     = 0xCD1F430;
    constexpr uintptr_t kWeaponIdsType       = 0xCD2C9E0;
    constexpr uintptr_t kTransformType       = 0xCD2BB30;
    constexpr uintptr_t kLockdownConfig      = 0xCD268D8;  // Halloween 2026 mode
}

// ========================
// Managed RVA — function offsets in libunity.so
// ========================
namespace rva {
    constexpr uintptr_t kPlayerGetPosition         = 0x9C9B170;  // odq.apss
    constexpr uintptr_t kStatsGetHp                = 0x9C98DD8;  // ydm.bdqp
    constexpr uintptr_t kStatsGetMaxHp             = 0x9C9A434;  // ydm.nfln
    constexpr uintptr_t kStatsGetArmor             = 0x9C83428;  // ydm.tgiz
    constexpr uintptr_t kStatsGetMaxArmor          = 0x9C89764;  // ydm.fidx
    constexpr uintptr_t kStatsGetHelmet            = 0x9C8A4F8;  // ydm.ljbj
    constexpr uintptr_t kWeaponPartGetContext      = 0x9C82FE0;  // uyg.qtck
    constexpr uintptr_t kWeaponPartGetActiveWeapon = 0x9C81184;  // uyg.lwgn
    constexpr uintptr_t kWeaponGetId               = 0x9CB3480;  // jrx.onxo
    constexpr uintptr_t kWeaponGetMetaId           = 0x9CA6F24;  // jrx.apsm
    constexpr uintptr_t kWeaponGetMagazineAmmo     = 0x9CB2E14;  // jrx.gqpy
    constexpr uintptr_t kWeaponGetReserveAmmo      = 0x9CB22C8;  // jrx.mrnk
}

// ========================
// Field offsets
// ========================
namespace field {
    constexpr uintptr_t kStaticFields = 0xB8;  // Il2CppClass -> static_fields
    constexpr uintptr_t kListItems    = 0x10;  // List<T>._items
    constexpr uintptr_t kListCount    = 0x18;  // List<T>._size
    constexpr uintptr_t kArrayStart   = 0x20;  // Il2CppArray first element

    // WeaponPart (uyg, was nrk)
    constexpr uintptr_t kWeaponContext = 0x30;
    constexpr uintptr_t kTeam          = 0x38;
    constexpr uintptr_t kHitTransform  = 0x40;
    constexpr uintptr_t kActiveWeapon  = 0x80; // was 0x78, shifted +8 Halloween 2026

    // FILL from Cheat Engine / GameGuardian pointer scan on running game:
    // attach to com.axlebolt.standoff2, find Registry singleton,
    // scan for pointer to List<Player> inside it
    constexpr uintptr_t kRegistryPlayerList = 0x0;  // FILL: Registry -> players list
    constexpr uintptr_t kPlayerStats        = 0x0;  // FILL: Player(odq) -> Stats(ydm) ref
    constexpr uintptr_t kPlayerTeamId       = 0x0;  // FILL: Player(odq) -> team int
    constexpr uintptr_t kPlayerIsAlive      = 0x0;  // FILL: Player(odq) -> isAlive bool
    constexpr uintptr_t kPlayerPosition     = 0x0;  // FILL: Player(odq) -> cached Vector3 pos
    constexpr uintptr_t kStatsHp            = 0x0;  // FILL: Stats(ydm) -> hp float
    constexpr uintptr_t kStatsMaxHp         = 0x0;  // FILL: Stats(ydm) -> maxHp float
}

struct Vector3 {
    float x, y, z;
    Vector3 operator-(const Vector3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    Vector3 operator+(const Vector3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    float length() const { return sqrtf(x * x + y * y + z * z); }
    float distance(const Vector3& o) const { return (*this - o).length(); }
};

struct Matrix4x4 {
    float m[4][4];
};

inline uintptr_t lib_base = 0;

inline bool init() {
    if (!mem::init(PACKAGE)) return false;
    lib_base = mem::get_module_base(mem::game_pid, LIB_NAME);
    return lib_base != 0;
}

// TypeInfo -> Class -> static_fields -> first field (singleton instance)
inline uintptr_t get_singleton(uintptr_t ti_offset) {
    uintptr_t cls = mem::read<uintptr_t>(lib_base + ti_offset);
    if (!cls) return 0;
    uintptr_t sf = mem::read<uintptr_t>(cls + field::kStaticFields);
    if (!sf) return 0;
    return mem::read<uintptr_t>(sf);
}

inline int get_player_count() {
    uintptr_t reg = get_singleton(typeinfo::kRegistry);
    if (!reg || field::kRegistryPlayerList == 0) return 0;
    uintptr_t list = mem::read<uintptr_t>(reg + field::kRegistryPlayerList);
    if (!list) return 0;
    return mem::read<int>(list + field::kListCount);
}

inline uintptr_t get_player(int idx) {
    uintptr_t reg = get_singleton(typeinfo::kRegistry);
    if (!reg || field::kRegistryPlayerList == 0) return 0;
    uintptr_t list = mem::read<uintptr_t>(reg + field::kRegistryPlayerList);
    if (!list) return 0;
    uintptr_t items = mem::read<uintptr_t>(list + field::kListItems);
    if (!items) return 0;
    return mem::read<uintptr_t>(items + field::kArrayStart + idx * 0x8);
}

inline Vector3 get_position(uintptr_t player) {
    if (field::kPlayerPosition == 0) return {};
    return mem::read<Vector3>(player + field::kPlayerPosition);
}

inline int get_team(uintptr_t player) {
    if (field::kPlayerTeamId == 0) return -1;
    return mem::read<int>(player + field::kPlayerTeamId);
}

inline bool is_alive(uintptr_t player) {
    if (field::kPlayerIsAlive == 0) return true;
    return mem::read<bool>(player + field::kPlayerIsAlive);
}

inline float get_health(uintptr_t player) {
    if (field::kPlayerStats == 0 || field::kStatsHp == 0) return 100.f;
    uintptr_t stats = mem::read<uintptr_t>(player + field::kPlayerStats);
    if (!stats) return 0.f;
    return mem::read<float>(stats + field::kStatsHp);
}

inline float get_max_health(uintptr_t player) {
    if (field::kPlayerStats == 0 || field::kStatsMaxHp == 0) return 100.f;
    uintptr_t stats = mem::read<uintptr_t>(player + field::kPlayerStats);
    if (!stats) return 100.f;
    return mem::read<float>(stats + field::kStatsMaxHp);
}

// FILL: Camera.main -> viewProjectionMatrix
// Нужен TypeInfo адрес Camera или статический указатель на main camera
inline Matrix4x4 get_view_matrix() {
    return {};
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

} // namespace game
