#pragma once
#include <cstdint>
#include <cmath>
#include "../other/memory.hpp"

// Standoff 2 — Halloween 2026 (Unity 6)
// Unity 6: libil2cpp.so + libunity.so merged → single libunity.so (217MB)
// Offsets: il2cppdump v16 + dump.cs field analysis, ARM64

namespace game {

constexpr const char* PACKAGE = "com.axlebolt.standoff2";
constexpr const char* LIB_NAME = "libunity.so";

// ========================
// TypeInfo addresses — class pointers in libunity.so
// Pattern: base + typeinfo_addr -> Il2CppClass* -> static_fields
// ========================
namespace typeinfo {
    constexpr uintptr_t kPlayer              = 0xCD32658;  // odq (Player)
    constexpr uintptr_t kNetObject           = 0xCD359A0;  // wwv (NetObject)
    constexpr uintptr_t kClientWorld         = 0xCD34DC8;  // ggk (ClientWorld impl)
    constexpr uintptr_t kRegistry            = 0xCD32028;  // nba (Registry)
    constexpr uintptr_t kStore               = 0xCD303E8;  // ibc (Store)
    constexpr uintptr_t kActor               = 0xCD34C50;  // uwa (Actor)
    constexpr uintptr_t kStats               = 0xCD360F0;  // ydm (Stats)
    constexpr uintptr_t kGrenadeCim          = 0xCD2EDA8;  // edu (Grenade)
    constexpr uintptr_t kCharacterBindings   = 0xCD1FA60;
    constexpr uintptr_t kBonesDictionary     = 0xCD1F430;
    constexpr uintptr_t kWeaponIdsType       = 0xCD2C9E0;
    constexpr uintptr_t kTransformType       = 0xCD2BB30;
    constexpr uintptr_t kLockdownConfig      = 0xCD268D8;
}

// ========================
// Managed RVA — function offsets in libunity.so
// ========================
namespace rva {
    // Player (odq)
    constexpr uintptr_t kPlayerGetPosition         = 0x9C9B170;  // odq.apss → Vector3
    constexpr uintptr_t kPlayerGetTeam             = 0x9C955F8;  // odq.uwwz → byte (kgb iface)
    constexpr uintptr_t kPlayerIsAlive             = 0x9C83AA0;  // odq.dupd → bool override

    // Stats (ydm)
    constexpr uintptr_t kStatsGetHp                = 0x9C98DD8;  // ydm.bdqp → ushort (espa @ 0x10)
    constexpr uintptr_t kStatsGetMaxHp             = 0x9C9A434;  // ydm.nfln → ushort (ktmm @ 0x14)
    constexpr uintptr_t kStatsGetArmor             = 0x9C83428;  // ydm.tgiz → ushort (ntyf @ 0x16)
    constexpr uintptr_t kStatsGetMaxArmor          = 0x9C89764;  // ydm.fidx → ushort (tuvr @ 0x1A)
    constexpr uintptr_t kStatsGetHelmet            = 0x9C8A4F8;  // ydm.ljbj → bool (wvhk @ 0x1C)

    // WeaponPart (uyg)
    constexpr uintptr_t kWeaponPartGetContext      = 0x9C82FE0;  // uyg.qtck
    constexpr uintptr_t kWeaponPartGetActiveWeapon = 0x9C81184;  // uyg.lwgn

    // WeaponInstance (jrx)
    constexpr uintptr_t kWeaponGetId               = 0x9CB3480;  // jrx.onxo
    constexpr uintptr_t kWeaponGetMetaId           = 0x9CA6F24;  // jrx.apsm
    constexpr uintptr_t kWeaponGetMagazineAmmo     = 0x9CB2E14;  // jrx.gqpy
    constexpr uintptr_t kWeaponGetReserveAmmo      = 0x9CB22C8;  // jrx.mrnk

    // Unity Camera
    constexpr uintptr_t kCameraGetMain             = 0x6BE9604;  // Camera.get_main (icall FindMainCamera)
    constexpr uintptr_t kCameraGetWorldToCam       = 0x6BE3C50;  // Camera.get_worldToCameraMatrix
    constexpr uintptr_t kCameraGetProjection       = 0x6BDEE10;  // Camera.get_projectionMatrix

    // Unity Transform
    constexpr uintptr_t kTransformGetPosition      = 0x6937790;  // Transform.get_position
    constexpr uintptr_t kComponentGetTransform     = 0x791CF08;  // Component.get_transform
}

// ========================
// Field offsets (from dump.cs class field analysis)
// ========================
namespace field {
    // Il2Cpp internal
    constexpr uintptr_t kStaticFields = 0xB8;  // Il2CppClass -> static_fields
    constexpr uintptr_t kListItems    = 0x10;  // List<T>._items
    constexpr uintptr_t kListCount    = 0x18;  // List<T>._size
    constexpr uintptr_t kArrayStart   = 0x20;  // Il2CppArray first element

    // WeaponPart (uyg) instance fields
    constexpr uintptr_t kWeaponContext = 0x30;  // uyg.kbdx: WeaponContext
    constexpr uintptr_t kTeam          = 0x38;
    constexpr uintptr_t kHitTransform  = 0x40;
    constexpr uintptr_t kActiveWeapon  = 0x80;

    // Player (odq) instance fields — from dump.cs
    constexpr uintptr_t kPlayerStats   = 0x70;  // odq.djka: ydm (Stats ref)
    constexpr uintptr_t kPlayerTeamId  = 0x90;  // odq.<pley>k__BackingField: byte (kgb.late)
    constexpr uintptr_t kPlayerIsAlive = 0x44;  // wwv.<nxqe>k__BackingField: bool (inherited, cmpd)

    // Stats (ydm) instance fields — from dump.cs
    // NOTE: HP/MaxHP are ushort in the game, NOT float
    constexpr uintptr_t kStatsHp       = 0x10;  // ydm.<espa>k__BackingField: ushort
    constexpr uintptr_t kStatsMaxHp    = 0x14;  // ydm.<ktmm>k__BackingField: ushort

    // FILL: need GG/CE pointer scan on running game
    // Нет прямого List<Player> — nba (Registry) хранит объекты в Dictionary<NetObjectTypeId, npg>
    // ggk (ClientWorld) хранит nba в static_fields. Цепочка указателей:
    //   lib_base + kClientWorld → Il2CppClass → static_fields → [offset ~0x30] → nba instance
    //   nba instance → ovvj (Dictionary) → entries → найти тип Player → span
    // Нужно сканировать в GG: найти Registry singleton → найти List/Array игроков внутри
    constexpr uintptr_t kRegistryPlayerList = 0x0;  // FILL

    // FILL: позиция игрока
    // odq.apss() читает позицию из Transform (нативный Unity объект)
    // Для внешнего чтения: Component→Transform→native position
    // Сканируй в GG: найди позицию своего персонажа, scan pointer chain
    constexpr uintptr_t kPlayerPosition = 0x0;  // FILL
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

// TypeInfo -> Class -> static_fields -> read at offset
inline uintptr_t read_static_field(uintptr_t ti_offset, uintptr_t field_offset) {
    uintptr_t cls = mem::read<uintptr_t>(lib_base + ti_offset);
    if (!cls) return 0;
    uintptr_t sf = mem::read<uintptr_t>(cls + field::kStaticFields);
    if (!sf) return 0;
    return mem::read<uintptr_t>(sf + field_offset);
}

// shortcut: read first static field (singleton pattern)
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

// FILL: Camera view-projection matrix
// Camera.get_main (icall FindMainCamera) — RVA 0x6BE9604
// Camera.get_worldToCameraMatrix — RVA 0x6BE3C50
// Camera.get_projectionMatrix — RVA 0x6BDEE10
// Для внешнего чтения нужно:
// 1) Найти Camera.main через GG (искать указатель на камеру)
// 2) Прочитать viewProjectionMatrix из нативной структуры камеры
//    (оффсет зависит от версии Unity, обычно ~0x2C0-0x300)
// 3) Или: через GG найти матрицу (16 float подряд) и зафиксировать цепочку
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
