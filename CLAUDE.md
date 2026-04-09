# VipModular — Project Context

Modular VIP/privilege system for **Counter-Strike 1.6** built on **AMX Mod X** (AMXX), written in **Pawn**.  
Version: `5.0.0-rc4f1` | Author: ArKaNeMaN

> For deeper detail on each subsystem, see `docs/claude/`.

---

## What it does

Lets server admins define privilege tiers (VIPs) in JSON configs. Each privilege specifies:
- **Conditions** (`Access`) — which players qualify (Limits system)
- **Features** (`Modules`) — what those players get (Modules system)

Modules and limits are fully extensible: third-party plugins register new types at runtime.

---

## Plugin files

| File | Role |
|------|------|
| `VipModular.sma` | Core — init, config loading, player update lifecycle |
| `ItemsController.sma` | Standalone item-effects framework (give weapon/health/etc.) |
| `VipM-Misc.sma` | Reload helper — triggers `VipM_UserUpdate` on spawn/round events |
| `VipM-ModulesLimiter.sma` | Enables/disables modules per-map via `Modules.json` |
| `VipM-M-WeaponMenu.sma` | Module: weapon selection menu |
| `VipM-M-SpawnItems.sma` | Module: give items on spawn |
| `VipM-M-SpawnHealth.sma` | Module: set health/armor on spawn |
| `VipM-M-Vampire.sma` | Module: heal on kill |
| `VipM-M-VipInTab.sma` | Module: VIP label in scoreboard |

---

## Source layout

```
amxmodx/scripting/
├── include/VipModular.inc        — public API header (include this in plugins)
├── include/VipM/Modules.inc      — module system API
├── include/VipM/Limits.inc       — limits system API
├── include/ItemsController.inc   — items system API
├── VipM/
│   ├── Core/
│   │   ├── VipsManager.inc       — loads Vips.json, manages g_tUserModules[]
│   │   ├── Objects/Modules/Type.inc  — module type registry (ArrayMap)
│   │   ├── Objects/Modules/Unit.inc  — module instance (params per VIP entry)
│   │   ├── Objects/Limits/Type.inc   — limit type registry
│   │   ├── Objects/Limits/Unit.inc   — limit instance
│   │   ├── Objects/VipUnit.inc        — deserializes one privilege from JSON
│   │   └── API/{Main,Modules,Limits}.inc — native implementations
│   ├── DefaultObjects/
│   │   ├── Limit/*.inc           — 24 built-in limit types
│   │   └── Registrar.inc         — registers all defaults
│   ├── Forwards.inc              — thin macro wrapper over CreateMultiForward
│   ├── ArrayMap.inc              — string-keyed array map (used for type registries)
│   └── WeaponMenu/               — WeaponMenu module implementation
└── ItemsController/
    ├── Objects/Items/{Type,Instance}.inc
    ├── API/{ItemType,Item,Compat}.inc
    └── DefaultObjects/ItemType/  — 18 built-in item types
```

---

## Core data flow

```
plugin_precache
  Forwards_RegAndCall("VipM_Modules_OnInited")  ← modules register here
  Forwards_RegAndCall("VipM_Limits_OnInited")   ← limits register here
  IC_ItemType_OnInited()                         ← item types register here
  VipsManager_LoadFromFile("Vips.json")
  VipsManager_LoadFromFolder("Vips/")
  ModuleType_ActivateUsed()                      ← fires Module_OnActivated
  Forwards_RegAndCall("VipM_OnLoaded")

client_putinserver → RequestFrame → VipsManager_UserReload(playerIndex)
  for each VipUnit: check Access limits → grant matching Modules
  result stored in g_tUserModules[playerIndex]: Trie<moduleName → Trie<params>>
  fires VipM_OnUserUpdated(playerIndex)

client_disconnected → VipsManager_UserReset(playerIndex)  ← frees all Tries
```

---

## Key patterns

- **Type registries** use `ArrayMap` (string key → index cast to enum type, e.g. `T_ModuleType`, `T_LimitType`)
- **Params** stored in AMXX `Trie` (string key → value); parsed from JSON via ParamsController
- **Forwards** wrapped in `Forwards_Reg` / `Forwards_Call` macros (`VipM/Forwards.inc`)
- **Object handles** are array indices cast to enum type — `Invalid_*` sentinel = `-1`
- **Static limits** store a bitmask instead of calling a forward (faster, no params)

---

## External dependencies

| Library | Min version | Purpose |
|---------|-------------|---------|
| ParamsController | 1.3.2 | JSON parsing, param reading, path utils (`PCPath_*`, `PCJson_*`, `PCGet_*`) |
| CommandAliases | 1.0.1 | Server command aliasing |
| ReAPI | 5.24.0.300 | Game hooks: `RG_CBasePlayer_Spawn`, `RG_CSGameRules_RestartRound`, etc. |

---

## Config files

All under `amxmodx/configs/plugins/VipModular/`:

| File | Purpose |
|------|---------|
| `Vips.json` | Privilege definitions (or `Vips/` folder for multiple files) |
| `Modules.json` | Module enable/disable rules (read by `VipM-ModulesLimiter`) |
| `Items/*.json` | Reusable item definitions |

`"File:WeaponMenu/Premium"` in JSON → resolves to `configs/plugins/VipModular/WeaponMenu/Premium.json`

---

## Local-only docs

Some context is intentionally kept out of the repository (e.g. local tool paths).  
Check `.claude/` memory for a `local-env.md` file — if it exists, read it before working with the build system or dependency headers.

---

## Sub-docs

- [`docs/claude/module-system.md`](docs/claude/module-system.md) — registering & using modules
- [`docs/claude/limits-system.md`](docs/claude/limits-system.md) — registering & using limits
- [`docs/claude/items-system.md`](docs/claude/items-system.md) — ItemsController API
- [`docs/claude/config-format.md`](docs/claude/config-format.md) — Vips.json format reference

---

## Working rules for Claude

### Code style

Match the existing style exactly. Key conventions:

**Naming — identifiers**

| Kind | Convention | Example |
|------|-----------|---------|
| Local variables | camelCase | `playerIndex`, `menuIndex`, `params` |
| Global variables | PascalCase (capital first letter) | `UserAutoOpen`, `UserLeftItems`, `Vips` |
| Public functions / API | `Namespace_PascalCase` | `VipsManager_Init`, `ModuleType_Find` |
| Static (private) functions | `Namespace_PascalCase` + `static` keyword | `static LimitType_Get(...)` |
| Internal helpers | `_PascalCase` | `_Cmd_Menu` |
| Native/forward callbacks | `@FullNativeName` | `@RG_CBasePlayer_Spawn`, `@Module_OnActivated` |
| Enum handle types | `T_Name` | `T_ModuleType`, `T_VipUnit` |
| Enum struct layouts | `S_Name` | `S_ModuleType`, `S_WeaponMenu` |
| Enum (actual enumerations) | `E_Name` | `E_ModuleEvent`, `E_LimitEvent` |
| Enum fields | `StructName_FieldName` | `ModuleType_Name`, `VipUnit_Access` |
| Constants / macros | `SCREAMING_SNAKE_CASE` | `MODULE_NAME`, `TASK_OFFSET_AUTO_OPEN` |

**No Hungarian notation anywhere.** Don't add `i`, `s`, `f`, `b`, `g`, `g_` prefixes to variables — not on locals, not on globals. The existing codebase has legacy `g_aVips`, `gUserAutoOpen`, `iRet`, `sName` etc. — don't copy that pattern into new code.

**Formatting**
- 4-space indentation
- Braces on same line for control flow: `if (...) {`
- Single blank line between logical blocks; two blank lines between top-level function definitions
- Long argument lists: one argument per line, aligned to opening paren
- Conditions spanning multiple lines: each sub-condition on its own line, operator at start

**Other**
- Use `PCGet_*` / `PCSingle_*` helpers from ParamsController for reading params from Trie/JSON — don't read Tries directly where a helper exists
- Prefer `Invalid_*` sentinel checks over magic `-1` literals
- Don't add error handling for cases the framework already guards against

---

### Documentation maintenance

**After every code change, update the relevant `docs/claude/*.md` file** to reflect what changed:
- New module → update `docs/claude/module-system.md`
- New/changed limit type → update `docs/claude/limits-system.md`
- New/changed item type → update `docs/claude/items-system.md`
- Config format change → update `docs/claude/config-format.md`
- Structural change affecting multiple systems → update `CLAUDE.md` and the affected sub-docs

If a change doesn't fit any existing sub-doc, create a new one in `docs/claude/` and add it to the sub-docs list above.
