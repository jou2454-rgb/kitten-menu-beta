#pragma once
#include <cstdint>
#include <cmath>
#include "../other/memory.hpp"

// Standoff 2 — Halloween 2026 (Unity 6)
// Unity 6: libil2cpp.so + libunity.so merged → single libunity.so (217MB)
// Offsets: il2cppdump v16 + dump.cs + script.json

namespace game {

constexpr const char* PACKAGE = "com.axlebolt.standoff2";
constexpr const char* LIB_NAME = "libunity.so";

// ========================
// TypeInfo addresses
// ========================
namespace typeinfo {
    constexpr uintptr_t kPlayer              = 0xCD32658;  // odq
    constexpr uintptr_t kNetObject           = 0xCD359A0;  // wwv
    constexpr uintptr_t kClientWorld         = 0xCD2F9B0;  // ggk (from script.json)
    constexpr uintptr_t kRegistry            = 0xCD32028;  // nba
    constexpr uintptr_t kStore               = 0xCD303E8;  // ibc
    constexpr uintptr_t kActor               = 0xCD34C50;  // uwa
    constexpr uintptr_t kStats               = 0xCD360F0;  // ydm
    constexpr uintptr_t kCamera              = 0xCD1F798;  // UnityEngine.Camera
    constexpr uintptr_t kTransform           = 0xCD2BB30;  // UnityEngine.Transform
    constexpr uintptr_t kGameObject          = 0xCD226D8;  // UnityEngine.GameObject
    constexpr uintptr_t kCameraController    = 0xCD2F128;  // etx (mainCameraController)
}

// ========================
// Managed RVA
// ========================
namespace rva {
    constexpr uintptr_t kPlayerGetPosition         = 0x9C9B170;  // odq.apss → Vector3
    constexpr uintptr_t kPlayerGetTeam             = 0x9C955F8;  // odq.uwwz → byte
    constexpr uintptr_t kPlayerIsAlive             = 0x9C83AA0;  // odq.dupd → bool

    constexpr uintptr_t kStatsGetHp                = 0x9C98DD8;  // ydm.bdqp → ushort
    constexpr uintptr_t kStatsGetMaxHp             = 0x9C9A434;  // ydm.nfln → ushort
    constexpr uintptr_t kStatsGetArmor             = 0x9C83428;  // ydm.tgiz
    constexpr uintptr_t kStatsGetMaxArmor          = 0x9C89764;  // ydm.fidx
    constexpr uintptr_t kStatsGetHelmet            = 0x9C8A4F8;  // ydm.ljbj

    constexpr uintptr_t kWeaponPartGetContext      = 0x9C82FE0;
    constexpr uintptr_t kWeaponPartGetActiveWeapon = 0x9C81184;
    constexpr uintptr_t kWeaponGetId               = 0x9CB3480;
    constexpr uintptr_t kWeaponGetMetaId           = 0x9CA6F24;
    constexpr uintptr_t kWeaponGetMagazineAmmo     = 0x9CB2E14;
    constexpr uintptr_t kWeaponGetReserveAmmo      = 0x9CB22C8;

    constexpr uintptr_t kCameraGetMain             = 0x6BE9604;
    constexpr uintptr_t kCameraGetWorldToCam       = 0x6BE3C50;
    constexpr uintptr_t kCameraGetProjection       = 0x6BDEE10;
    constexpr uintptr_t kTransformGetPosition      = 0x6937790;
    constexpr uintptr_t kComponentGetTransform     = 0x791CF08;

    constexpr uintptr_t kCameraCtrlGetCamera       = 0xA58B210;  // cgy.skjp → Camera
}

// ========================
// Field offsets
// ========================
namespace field {
    // Il2Cpp internal
    constexpr uintptr_t kStaticFields = 0xB8;
    constexpr uintptr_t kListItems    = 0x10;
    constexpr uintptr_t kListCount    = 0x18;
    constexpr uintptr_t kArrayStart   = 0x20;

    // UnityEngine.Object → native C++ ptr
    constexpr uintptr_t kCachedPtr    = 0x10;  // IntPtr m_CachedPtr

    // CameraController base (cgy)
    constexpr uintptr_t kCgyCamera    = 0x20;  // cgy.<kmek>k__BackingField: Camera

    // WeaponPart (uyg)
    constexpr uintptr_t kWeaponContext = 0x30;
    constexpr uintptr_t kTeam          = 0x38;
    constexpr uintptr_t kHitTransform  = 0x40;
    constexpr uintptr_t kActiveWeapon  = 0x80;

    // Player (odq)
    constexpr uintptr_t kPlayerStats   = 0x70;  // odq.djka → ydm
    constexpr uintptr_t kPlayerTeamId  = 0x90;  // odq.pley: byte (kgb.late)
    constexpr uintptr_t kPlayerIsAlive = 0x44;  // wwv.nxqe: bool (cmpd)

    // Stats (ydm) — HP/MaxHP are ushort!
    constexpr uintptr_t kStatsHp       = 0x10;  // ydm.espa: ushort
    constexpr uintptr_t kStatsMaxHp    = 0x14;  // ydm.ktmm: ushort

    // ========================
    // FILL: нужен GG pointer scan
    // ========================

    // Список игроков: nba хранит в Dictionary<NetObjectTypeId, npg>
    // GG: найди lib_base + 0xCD32028 → [ptr] → [+0xB8] → static_fields
    //     внутри static_fields ищи указатели на массивы ohn (NetObject)
    constexpr uintptr_t kRegistryPlayerList = 0x0;  // FILL

    // Позиция: Player → Transform → native → position
    // GG: поиск своих координат (float), pointer scan → оффсет от Player
    constexpr uintptr_t kPlayerPosition = 0x0;  // FILL

    // Камера: native Camera object → viewProjection matrix
    // GG: повернуть камеру → искать 16 меняющихся float → pointer scan
    // Типичный оффсет в Unity 6: ~0x2C0-0x310 от native Camera ptr
    constexpr uintptr_t kNativeCamViewMatrix = 0x0;  // FILL
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

inline uintptr_t read_static_field(uintptr_t ti_offset, uintptr_t field_offset) {
    uintptr_t cls = mem::read<uintptr_t>(lib_base + ti_offset);
    if (!cls) return 0;
    uintptr_t sf = mem::read<uintptr_t>(cls + field::kStaticFields);
    if (!sf) return 0;
    return mem::read<uintptr_t>(sf + field_offset);
}

inline uintptr_t get_singleton(uintptr_t ti_offset) {
    return read_static_field(ti_offset, 0);
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
    return (int)mem::read<uint8_t>(player + field::kPlayerTeamId);
}

inline bool is_alive(uintptr_t player) {
    if (field::kPlayerIsAlive == 0) return true;
    return mem::read<bool>(player + field::kPlayerIsAlive);
}

inline float get_health(uintptr_t player) {
    if (field::kPlayerStats == 0 || field::kStatsHp == 0) return 100.f;
    uintptr_t stats = mem::read<uintptr_t>(player + field::kPlayerStats);
    if (!stats) return 0.f;
    return (float)mem::read<uint16_t>(stats + field::kStatsHp);
}

inline float get_max_health(uintptr_t player) {
    if (field::kPlayerStats == 0 || field::kStatsMaxHp == 0) return 100.f;
    uintptr_t stats = mem::read<uintptr_t>(player + field::kPlayerStats);
    if (!stats) return 100.f;
    return (float)mem::read<uint16_t>(stats + field::kStatsMaxHp);
}

// Камера: CameraController (etx/cgy) → Camera → m_CachedPtr → native → matrix
// Когда найдёшь kNativeCamViewMatrix через GG, эта функция заработает
inline Matrix4x4 get_view_matrix() {
    if (field::kNativeCamViewMatrix == 0) return {};

    // cgy (CameraController base) → Camera managed object at +0x20
    // Camera managed → m_CachedPtr at +0x10 → native Camera C++ object
    // native Camera → viewProjectionMatrix at kNativeCamViewMatrix
    //
    // Цепочка: нужен указатель на CameraController singleton
    // etx_TypeInfo = 0xCD2F128 → но etx это интерфейс, не синглтон
    // Нужно найти через GG: указатель на текущую камеру в памяти

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
