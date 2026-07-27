# VipModular — Complete Agent Reference

> Modular VIP/privilege system for **Counter-Strike 1.6** (AMX Mod X, Pawn).  
> Version: `5.0.0-rc4f2` | Author: ArKaNeMaN  
> Stack: AMXX 1.10, ReAPI 5.29, ParamsController 1.4.2, CommandAliases 1.0.1

---

## 1. Overview & Architecture

### What it does

Server admins define privilege tiers in JSON configs. Each privilege has:
- **`Access`** — conditions (Limits) that determine which players qualify
- **`Modules`** — features (Modules) that qualified players receive

The system has **3 extensible registries**:
| Registry | What it registers | Forward to use |
|----------|------------------|----------------|
| **Module types** | Named game features | `VipM_Modules_OnInited()` |
| **Limit types** | Boolean conditions | `VipM_Limits_OnInited()` |
| **Item types** | Effect objects (IC) | `IC_ItemType_OnInited()` |

### Core principle

Privileges are checked **top-to-bottom** in `Vips.json`. For each VIP unit:
- If the player **passes** any Access limit → all modules in that unit are merged into the player
- Modules of the **same name** from multiple VIPs get merged (via `Module_OnMergeParams`)
- First match wins for each module (if no merge handler)

---

## 2. Source Code Layout

```
amxmodx/scripting/
├── VipModular.sma                          ← Core: init, config loading, lifecycle
├── ItemsController.sma                     ← Standalone item-effects framework
├── VipM-Misc.sma                           ← Reload helper (spawn/round triggers)
├── VipM-ModulesLimiter.sma                 ← Map-based module enable/disable
│
├── VipM-M-WeaponMenu.sma                   ← Module: weapon selection menu
├── VipM-M-SpawnItems.sma                   ← Module: give items on spawn
├── VipM-M-SpawnHealth.sma                  ← Module: set health/armor on spawn
├── VipM-M-Vampire.sma                      ← Module: heal on kill
├── VipM-M-VipInTab.sma                     ← Module: VIP label in scoreboard
│
├── include/
│   ├── VipModular.inc                      ← Public API (main header, include this)
│   ├── VipM/
│   │   ├── Modules.inc                     ← Module system API (natives, forwards, events)
│   │   ├── Limits.inc                      ← Limits system API
│   │   └── Params.inc                      ← Deprecated param helpers (use PCGet_* instead)
│   │   └── M/
│   │       └── WeaponMenu.inc              ← WeaponMenu module native API
│   ├── ItemsController.inc                 ← Items system API
│
├── VipM/
│   ├── Core/
│   │   ├── VipsManager.inc                 ← Loads Vips.json, manages g_tUserModules[]
│   │   ├── SrvCmds.inc                     ← Server commands (vipm_info, vipm_modules, vipm_limits)
│   │   ├── Objects/
│   │   │   ├── VipUnit.inc                 ← Deserializes one privilege from JSON
│   │   │   ├── Modules/
│   │   │   │   ├── Type.inc                ← Module type registry (ArrayMap-based)
│   │   │   │   └── Unit.inc                ← Module instance (params per VIP entry)
│   │   │   ├── Limits/
│   │   │   │   ├── Type.inc                ← Limit type registry
│   │   │   │   └── Unit.inc                ← Limit instance
│   │   │   └── Param.inc                   ← Param parsing helpers
│   │   └── API/
│   │       ├── Main.inc                    ← Native: VipM_UserUpdate, VipM_Json_LogForFile
│   │       ├── Modules.inc                 ← Native implementations for module API
│   │       └── Limits.inc                  ← Native implementations for limit API
│   ├── DefaultObjects/
│   │   ├── Registrar.inc                   ← Registers all built-in limits/params/natives
│   │   ├── ParamType/                      ← Custom param type registrations
│   │   │   ├── Limit.inc
│   │   │   ├── Limits.inc
│   │   │   ├── LimitType.inc
│   │   │   ├── ModuleType.inc
│   │   │   └── CounterType.inc
│   │   ├── Limit/                          ← 22 built-in limit types
│   │   │   ├── Always.inc, Never.inc       ← Static, no params
│   │   │   ├── Alive.inc, Bot.inc, Steam.inc  ← Static, per-player
│   │   │   ├── Flags.inc, Map.inc, Time.inc → Dynamic with params
│   │   │   ├── Logic.inc                   ← AND/OR/NOT combinator
│   │   │   └── ... (22 total)
│   ├── Forwards.inc                        ← Thin macros over CreateMultiForward
│   ├── ArrayMap.inc                        ← String-keyed array map (used for type registries)
│   ├── ArrayTrieUtils.inc                  ← Iteration macros & safe array wrappers
│   ├── Utils.inc                           ← Bit ops, CallOnce, JSON helpers, lang macros
│   ├── DebugMode.inc                       ← Debug logging support
│   └── WeaponMenu/                         ← WeaponMenu subsystem
│       ├── Menus.inc                       ← Menu rendering logic
│       ├── Natives.inc                     ← Native implementations for WeaponMenu
│       ├── KeyValueCounter.inc             ← Counter for weapon menu limits
│       └── Objects/                        ← Weapon menu object definitions
│
└── ItemsController/
    ├── Objects/
    │   └── Items/
    │       ├── Type.inc                    ← Item type registry
    │       └── Instance.inc                ← Item instance management
    ├── API/
    │   ├── ItemType.inc                    ← ItemType registration natives
    │   ├── Item.inc                        ← Item read/give/free natives
    │   └── Compat.inc                      ← Deprecated compatibility layer
    └── DefaultObjects/
        └── ItemType/                       ← 16 built-in item types
            ├── Weapon.inc, Health.inc, Armor.inc
            ├── Money.inc, Speed.inc, DefuseKit.inc
            ├── Command.inc, Function.inc
            ├── If.inc, ItemsList.inc, Random.inc
            ├── DamageMult.inc, InstantReload.inc
            ├── InstantReloadAllWeapons.inc
            ├── RefillBpAmmo.inc, CustomWeapon.inc
```

---

## 3. Config File Format

All configs are in `amxmodx/configs/plugins/VipModular/`.  
Paths prefixed with `File:` resolve relative to that folder.  
`/` at the start → resolves relative to `amxmodx/configs/`.

### Vips.json

```json
[
    {
        "Access": [
            { "Type": "Flags", "Flags": "t" }
        ],
        "Modules": [
            {
                "Type": "SpawnItems",
                "Items": [
                    { "Type": "Weapon", "Name": "weapon_awp" },
                    { "Type": "Health", "Value": 150 }
                ]
            },
            {
                "Type": "WeaponMenu",
                "File:Config": "WeaponMenu/Premium"
            }
        ]
    }
]
```

- **`Access`**: array of limit objects, evaluated with **OR** by default
- **`Modules`**: array of module config objects; `"Type"` matches the registered module name
- **`File:Config`**: loads external JSON and merges it via `PCJson_ParseFile`
- Privileges can also be individual files in `Vips/` folder (all `*.json` loaded)

### Modules.json (for VipM-ModulesLimiter)

```json
[
    {
        "Limits": [{ "Type": "Map", "Prefix": "cs_" }],
        "Disable": ["WeaponMenu"]
    },
    {
        "Limits": [{ "Type": "Always" }],
        "Enable": ["SpawnItems", "SpawnHealth"]
    }
]
```

Checked at `VipM_Modules_OnActivate` forward to block/enable modules per-map.

### Param type names (for `AddParamsEx`)

| Name | Type | Used for |
|------|------|----------|
| `"Integer"` | int | Whole numbers |
| `"Float"` | float | Decimal numbers |
| `"Bool"` | bool | true/false |
| `"String"` | string | Text values |
| `"VipM-Limit"` | `T_LimitUnit` | Single limit |
| `"VipM-Limits"` | `Array:T_LimitUnit` | List of limits |
| `"VipM-LimitType"` | `T_LimitType` | Limit type reference |
| `"VipM-ModuleType"` | `T_ModuleType` | Module type reference |
| `"IC-Item"` | `T_IC_Item` | Single item |
| `"IC-Items"` | `Array:T_IC_Item` | List of items |

For parameter definitions, use `PCParam()` macros:
```pawn
PCParam("Name", DEFAULT_PARAMS_STR_NAME)        // "String"
PCParam("Name", DEFAULT_PARAMS_INT_NAME)         // "Integer"
PCParam("Name", DEFAULT_PARAMS_FLOAT_NAME)       // "Float"
PCParam("Name", DEFAULT_PARAMS_BOOL_NAME)        // "Bool"
PCParam("Name", VIPM_PARAM_TYPE_LIMITS_NAME)     // "VipM-Limits"
PCParam("Name", IC_PARAM_TYPE_ITEMS_NAME)        // "IC-Items"
```
The 3rd argument is `required` (default: `false`):
```pawn
PCParam("Name", "Integer", true)                 // required param
```

### File reference syntax

- `"File:Path/Name"` → `configs/plugins/VipModular/Path/Name.json`
- `"File:/configs/Path/Name"` → `amxmodx/configs/Path/Name.json`

ParamsController resolves these transparently.

---

## 4. Core Data Flow (Lifecycle)

### Startup (`plugin_precache`)

```
VipModular.sma::plugin_precache()
  ├── register_plugin, register_library, PCCvar_Const
  ├── ParamsController_Init()
  ├── Forwards_Init()                        ← Init forward system
  ├── VipsManager_Init()                     ← Init Vips array
  ├── ModuleType_Init()                      ← Init module registry
  ├── SrvCmds_Init()                         ← Register server commands
  │
  ├── Forwards_RegAndCall("VipM_OnInitModules")  ← DEPRECATED
  │
  ├── VipsManager_LoadFromFile("Vips.json")  ← Parse and load VIP units
  ├── VipsManager_LoadFromFolder("Vips/")    ← Load VIP units from folder
  │
  ├── ModuleType_ActivateUsed()              ← Activate all used modules
  │   ├── Forwards_Call("VipM_Modules_OnActivate")  ← Each module → can be blocked
  │   └── each module's Module_OnActivated event     ← Register game hooks
  │
  └── Forwards_RegAndCall("VipM_OnLoaded")   ← System fully loaded
```

### Initialization order is CRITICAL:

```
1. LimitUnit_Init()
   ├── LimitType_Init()              ← Creates ArrayMap for limit types
   └── Forwards_RegAndCall("VipM_Limits_OnInited")  ← Limits register here

2. ModuleUnit_Init()
   ├── ModuleType_Init()             ← Creates ArrayMap for module types
   └── Forwards_RegAndCall("VipM_Modules_OnInited") ← Modules register here

3. IC_ItemType_OnInited() forward    ← Item types register here (called from ItemsController)

4. VipsManager_LoadFromFile / VipsManager_LoadFromFolder
   └── VipUnit_ReadList              ← Parses JSON → creates VipUnit objects
       ├── PCSingle_ObjVipmLimits    ← Reads Access limits
       └── JsonObject_GetModuleUnits ← Reads Modules, marks types as "Used"

5. ModuleType_ActivateUsed()
   ├── Forwards_Call("VipM_Modules_OnActivate")  ← VipM-ModulesLimiter can block
   └── ExecuteForward(Module_OnActivated)         ← Module registers hooks
```

### Player connection

```
client_authorized(playerIndex)
  └── DefaultObjects_OnClientAuth()   ← Sets static limits (Steam, SteamId, IP, Bot)

client_putinserver(playerIndex)
  ├── DefaultObjects_OnClientPutInServer()  ← Sets per-player state (Alive, Counter)
  └── RequestFrame → VipsManager_UserReload(playerIndex)
       ├── TrieCreate() for g_tUserModules[playerIndex]
       ├── For each VipUnit:
       │   ├── VipUnit_CheckUserAccess()    ← Checks Access limits
       │   └── On pass: store module params in Trie
       │       ├── First occurrence: store as-is
       │       └── Duplicate module: ModuleUnit_Merge (calls Module_OnMergeParams)
       └── Forwards_CallP("VipM_OnUserUpdated", playerIndex)
            ← Module can cache params here (e.g., VipInTab)

client_disconnected(playerIndex)
  └── VipsManager_UserReset(playerIndex)
       └── Frees all Tries in g_tUserModules[playerIndex]
```

---

## 5. Module System

### What is a module?

A module is a named game feature. Register a type, the core activates it if referenced in any VIP config, then it hooks into game events.

### Module Plugin Template

```pawn
#include <amxmodx>
#include <reapi>
#include <VipModular>

public stock const PluginName[] = "[VipM-M] MyModule";
public stock const PluginVersion[] = _VIPM_VERSION;
public stock const PluginAuthor[] = "AuthorName";
public stock const PluginURL[] = _VIPM_PLUGIN_URL;
public stock const PluginDescription[] = "Description";

new const MODULE_NAME[] = "MyModule";

public VipM_Modules_OnInited() {
    register_plugin(PluginName, PluginVersion, PluginAuthor);

    VipM_Modules_Register(MODULE_NAME);
    VipM_Modules_AddParamsEx(MODULE_NAME,
        PCParam("SomeValue", DEFAULT_PARAMS_INT_NAME, true),
        PCParam("Limits", VIPM_PARAM_TYPE_LIMITS_NAME)
    );
    VipM_Modules_RegisterEvent(MODULE_NAME, Module_OnActivated, "@OnActivate");
}

@OnActivate() {
    RegisterHookChain(RG_CBasePlayer_Spawn, "@OnSpawn", true);
}

@OnSpawn(const playerIndex) {
    if (!VipM_Modules_HasModule(MODULE_NAME, playerIndex))
        return;

    new Trie:p = VipM_Modules_GetParams(MODULE_NAME, playerIndex);
    new val = PCGet_Int(p, "SomeValue", 100);
    // apply effect...
}
```

### VipM_Modules_Register(name, Once = true)

- `Once = true` (default): first VIP tier that grants this module wins, later ones are ignored
- `Once = false`: the system calls `Module_OnMergeParams` when the same module comes from multiple tiers

### Module Events

| Event | Signature | When | Notes |
|-------|-----------|------|-------|
| `Module_OnActivated` | `()` | After config load, `plugin_precache` | Register game hooks. Return `VIPM_STOP` to cancel. |
| `Module_OnRead` | `(JSON:jCfg, Trie:p)` | While parsing config, `plugin_precache` | Modify/validate params. Return `VIPM_STOP` to skip this module unit. |
| `Module_OnMergeParams` | `(Trie:p1, Trie:p2) → Trie` | When 2+ tiers grant same module | Return `p1`, `p2`, or new Trie. New Trie auto-freed. |

### Reading module params in handlers

```pawn
// Check if player has module
if (!VipM_Modules_HasModule(MODULE_NAME, playerIndex))
    return;

// Get params Trie
new Trie:p = VipM_Modules_GetParams(MODULE_NAME, playerIndex);

// Read values using PCGet_* helpers
new intVal = PCGet_Int(p, "SomeInt", 0);
new bool:boolVal = PCGet_Bool(p, "SomeBool", false);
new Float:floatVal = PCGet_Float(p, "SomeFloat", 0.0);
new strVal[32]; PCGet_Str(p, "SomeStr", strVal, charsmax(strVal));

// Execute embedded limits
if (!PCGet_VipmLimitsCheck(p, "Limits", playerIndex, Limit_Exec_AND))
    return;

// Give items
PCGet_IcItemsGive(p, "Items", playerIndex);
```

### Caching module params (VipInTab pattern)

```pawn
public VipM_OnUserUpdated(const playerIndex) {
    new Trie:Params = VipM_Modules_GetParams(MODULE_NAME, playerIndex);
    PlayerSettings[playerIndex][Param_Enabled] = PCGet_Bool(Params, "Enabled", false);
    PlayerSettings[playerIndex][Param_Override] = PCGet_Bool(Params, "Override", false);
}
```

### Module naming convention

- Plugin file: `VipM-M-ModuleName.sma`
- Module name constant: `new const MODULE_NAME[] = "ModuleName"`
- Plugin name: `[VipM-M] ModuleName`

---

## 6. Limits System

### What is a limit?

A boolean condition that gates access. Two kinds:

| Kind | Params | Callbacks | Use cases |
|------|--------|-----------|-----------|
| **Dynamic** | Has params | `Limit_OnRead`, `Limit_OnCheck` | Map, Flags, Time, Frags... |
| **Static** | No params | No callbacks | Fast per-player flags (Steam, Alive, Bot) |

Static limits store their result in a **bitmask** per player — no forward called on check, very fast.

### Limit Plugin Template (Dynamic)

```pawn
#include <amxmodx>
#include <ParamsController>
#include <VipModular>

DefaultObjects_Limit_MyLimit_Register() {
    VipM_Limits_RegisterType("MyLimit", true, false);
    VipM_Limits_AddParamsEx("MyLimit",
        PCParam("MinFrags", DEFAULT_PARAMS_INT_NAME, true)
    );
    VipM_Limits_RegisterTypeEvent("MyLimit", Limit_OnCheck, "@OnCheck");
}

@OnCheck(const Trie:p, const playerIndex) {
    new minFrags = PCGet_Int(p, "MinFrags", 0);
    return get_user_frags(playerIndex) >= minFrags;
}
```

### Limit Plugin Template (Static)

```pawn
#include <amxmodx>
#include <ParamsController>
#include <VipModular>

DefaultObjects_Limit_HasPremium_Register() {
    VipM_Limits_RegisterType("HasPremium", true, true);  // bForPlayer=true, bStatic=true
    VipM_Limits_SetStaticValue("HasPremium", false);     // default for all
}

// Somewhere else, set the value:
// VipM_Limits_SetStaticValue("HasPremium", true, playerIndex);
```

### Limit Events

| Event | Signature | Purpose |
|-------|-----------|---------|
| `Limit_OnRead` | `(JSON:jUnit, Trie:tParams)` | Post-parse hook — return `VIPM_STOP` to abort |
| `Limit_OnCheck` | `(Trie:tParams, playerIndex) → bool` | Evaluate condition. Not called for static limits. |

### Logical Execution

```pawn
VipM_Limits_Execute(T_LimitUnit:limit, UserId = 0);
VipM_Limits_ExecuteList(Array:limits, UserId = 0, E_LimitsExecType:type = Limit_Exec_OR);
```

`E_LimitsExecType`: `Limit_Exec_OR`, `Limit_Exec_AND`, `Limit_Exec_XOR`

### Built-in Limits (22 types)

| Type | File | Dynamic/Static | Description |
|------|------|---------------|-------------|
| `Flags` | Flags.inc | Dynamic | Admin flags bitmask |
| `Steam` | Steam.inc | Static | Steam validation status |
| `SteamId` | SteamId.inc | Dynamic | Specific Steam IDs |
| `Ip` | Ip.inc | Dynamic | IP address match |
| `Map` | Map.inc | Dynamic | Map name (prefix/exact/regex) |
| `Time` | Time.inc | Dynamic | Server clock range |
| `RoundTime` | RoundTime.inc | Dynamic | Time elapsed in round |
| `Round` | Round.inc | Dynamic | Round number range |
| `GameTime` | GameTime.inc | Dynamic | Total game time |
| `Alive` | Alive.inc | Static | Player alive state |
| `Frags` | Frags.inc | Dynamic | Kill count range |
| `WasKilled` | WasKilled.inc | Dynamic | Death state this round |
| `InBuyZone` | InBuyZone.inc | Dynamic | Buy zone presence |
| `InFreezyTime` | InFreezyTime.inc | Dynamic | Freeze time active |
| `HasPrimaryWeapon` | HasPrimaryWeapon.inc | Dynamic | Primary weapon check |
| `LifeTime` | LifeTime.inc | Dynamic | Time connected |
| `WeekDay` | WeekDay.inc | Dynamic | Day of week |
| `Bot` | Bot.inc | Static | Is bot |
| `Name` | Name.inc | Dynamic | Player name match |
| `Counter` | Counter.inc | Dynamic | Max uses counter |
| `OncePer` | OncePer.inc | Dynamic | One-time per period |
| `Logic` | Logic.inc | Dynamic | AND/OR/NOT combinator |
| `Always` | Always.inc | Static | Always true |
| `Never` | Never.inc | Static | Always false |

(Note: the source lists ~22 but 24 entries including some noted. Always + Never are separate.)

### Setting static limit values in client callbacks

```pawn
// In DefaultObjects_OnClientAuth:
VipM_Limits_SetStaticValue("Steam", is_user_steam(playerIndex), playerIndex);

// In DefaultObjects_OnClientPutInServer:
VipM_Limits_SetStaticValue("Alive", true, playerIndex);
```

---

## 7. ItemsController System

### What is ItemsController?

Standalone framework for applying effects ("items") to players. Independent from VipModular core but tightly integrated. Used by SpawnItems, Vampire, WeaponMenu modules.

### Concepts

- **Item Type** (`T_IC_ItemType`) — named effect class, registered by a plugin
- **Item Instance** (`T_IC_Item`) — one concrete item with parsed params, created from JSON
- Give an item: `IC_Item_Give(playerIndex, item)` → fires `ItemType_OnGive`

### Item Type Registration Template

```pawn
#include <amxmodx>
#include <ItemsController>
#include <ParamsController>

DefaultObjects_ItemType_MyEffect_Register() {
    new T_IC_ItemType:type = IC_ItemType_SimpleRegister(
        .name = "MyEffect",
        .onGive = "@OnMyEffectGive"
    );
    IC_ItemType_AddParams(type,
        PCParam("Value", DEFAULT_PARAMS_INT_NAME, true),
        PCParam("SomeBool", DEFAULT_PARAMS_BOOL_NAME)
    );
}

@OnMyEffectGive(const playerIndex, const Trie:p) {
    new val = PCGet_Int(p, "Value", 0);
    // apply effect to player...
    return IC_RET_GIVE_SUCCESS;  // or IC_RET_GIVE_FAIL
}
```

### Item Type Events

| Event | Signature | Return |
|-------|-----------|--------|
| `ItemType_OnRead` | `(JSON:jCfg, Trie:params)` | `IC_RET_READ_SUCCESS` or `IC_RET_READ_FAIL` |
| `ItemType_OnGive` | `(playerIndex, Trie:params)` | `IC_RET_GIVE_SUCCESS` or `IC_RET_GIVE_FAIL` |
| `ItemType_OnFree` | `(Trie:params)` | void — cleanup custom allocations |

### API: Reading & giving items

```pawn
// From JSON
new T_IC_Item:item = IC_Item_ReadFromJson(jsonObject);
IC_Item_Give(playerIndex, item);
IC_Item_Free(item);

// From array
new Array:items = IC_Items_ReadFromJson(jsonArray);
IC_Items_Give(playerIndex, items);
IC_Items_Free(items);

// Convenience from module params
PCGet_IcItemGive(p, "Item", playerIndex);      // single
PCGet_IcItemsGive(p, "Items", playerIndex);    // list

// From JSON via ParamsController
new Array:items = PCSingle_ObjIcItems(jsonObj, "Items");
```

### Built-in Item Types (16 types)

| Type | Effect |
|------|--------|
| `Weapon` | Give weapon (strips old if needed) |
| `Health` | Set / add health |
| `Armor` | Set / add armor |
| `DefuseKit` | Give defuse kit |
| `Money` | Give / set money |
| `Speed` | Modify movement speed |
| `DamageMult` | Damage multiplier |
| `InstantReload` | Reload current weapon instantly |
| `InstantReloadAllWeapons` | Reload all weapons |
| `RefillBpAmmo` | Refill backpack ammo |
| `Command` | Execute server command |
| `Function` | Call a plugin function |
| `If` | Conditional (check limit → give item) |
| `ItemsList` | Ordered list of items |
| `Random` | Random item from a list |
| `CustomWeapon` | Spawn a custom weapon entity |

### Using ItemsController in a module

```pawn
// Register params:
VipM_Modules_AddParamsEx(MODULE_NAME,
    PCParam("Items", IC_PARAM_TYPE_ITEMS_NAME, true),
    PCParam("Limits", VIPM_PARAM_TYPE_LIMITS_NAME)
);

// In game event handler:
new Trie:p = VipM_Modules_GetParams(MODULE_NAME, playerIndex);
if (!PCGet_VipmLimitsCheck(p, "Limits", playerIndex, Limit_Exec_AND))
    return;
PCGet_IcItemsGive(p, "Items", playerIndex);
```

**Important**: Call `IC_Init()` in the plugin that uses items (before registering item params):
```pawn
public VipM_Modules_OnInited() {
    register_plugin(...);
    IC_Init();
    // ... register module with IC params
}
```

---

## 8. WeaponMenu System

### How it works

The WeaponMenu module (`VipM-M-WeaponMenu`) provides a weapon selection menu. It uses:
- Config-based weapon lists from JSON files
- Access limits per weapon
- Optional auto-open on spawn
- Expire status display via `VipM_WeaponMenu_SetExpireStatus()`

### Config structure

WeaponMenu configs are in `amxmodx/configs/plugins/VipModular/WeaponMenu/`.  
Referenced as `"File:WeaponMenu/Name"` in Vips.json.

### Native API

```pawn
// Set expire status text shown in weapon menu
native VipM_WeaponMenu_SetExpireStatus(const UserId, const sNewStatus[]);

// Menu commands
register_clcmd(VIPM_M_WEAPONMENU_CMD_MENU, "@CmdMenu");
register_clcmd(VIPM_M_WEAPONMENU_CMD_MENU_SILENT, "@CmdMenuSilent");
register_clcmd(VIPM_M_WEAPONMENU_CMD_AUTOOPEN_TOGGLE, "@CmdAutoOpenToggle");
```

### Keys (for Counter limits)

```pawn
VIPM_M_WEAPONMENU_PLAYER_COUNTER_KEY[]       // "VipM-M-WeaponMenu-Player"
VIPM_M_WEAPONMENU_MENU_COUNTER_KEY_PREFIX[]  // "VipM-M-WeaponMenu-Menu"
```

---

## 9. Forward System

The project uses a custom thin wrapper over AMXX `CreateMultiForward` / `ExecuteForward`:

```pawn
// In VipM/Forwards.inc:

Forwards_Init()                                    ← Must call first
Forwards_Reg(name, stopType)                       ← Register a multi-forward
Forwards_RegAndCall(name, stopType)                ← Register + execute immediately (destroy after)
Forwards_Call(name)                                ← Execute forward (no params)
Forwards_CallP(name, ...params)                    ← Execute forward with params
Forwards_DefaultReturn(val)                        ← Set default return value
Forwards_GetReturn()                               ← Get last call's return value
```

Key pattern: **Registered forwards** (persist across calls) vs **RegAndCall** (used for initialization hooks that fire once).

### All Forwards in the system

| Forward | When | Registered at init | Purpose |
|---------|------|-------------------|---------|
| `VipM_OnInitModules` (deprecated) | `plugin_precache` | RegAndCall | Init modules/limits (deprecated) |
| `VipM_Modules_OnInited` | `plugin_precache` | RegAndCall | Modules register here |
| `VipM_Limits_OnInited` | `plugin_precache` | RegAndCall | Limits register here |
| `VipM_Modules_OnActivate` | `plugin_precache` | Registered | Can block module activation |
| `VipM_OnLoaded` | `plugin_precache` | RegAndCall | System fully loaded |
| `VipM_OnUserUpdated` | `VipsManager_UserReload` | Registered | Player modules loaded/updated |
| `IC_ItemType_OnInited` | ItemsController init | RegAndCall | Item types register here |
| `IC_Item_OnInited` | ItemsController init | RegAndCall | Items initialized |
| `ParamsController_OnRegisterTypes` | PC init | PC internal | Custom param types register |

---

## 10. Public API Summary

### Main natives (`VipModular.inc`)

```pawn
native VipM_UserUpdate(const UserId);
forward VipM_OnLoaded();
forward VipM_OnUserUpdated(const UserId);
```

### Module API natives (`VipM/Modules.inc`)

```pawn
native VipM_Modules_Register(const moduleName[], const bool:Once = true);
native VipM_Modules_AddParamsEx(const moduleName[], any:...);
native VipM_Modules_RegisterEvent(const moduleName[], const E_ModuleEvent:event, const func[]);
native bool:VipM_Modules_IsActive(const moduleName[]);
native Trie:VipM_Modules_GetParams(const moduleName[], const playerIndex);
stock bool:VipM_Modules_HasModule(const moduleName[], const playerIndex);
forward VipM_Modules_OnInited();
forward VipM_Modules_OnActivate(const moduleName[]);
```

### Limits API natives (`VipM/Limits.inc`)

```pawn
native VipM_Limits_RegisterType(const sName[], const bool:bForPlayer = true, const bool:bStatic = false);
native VipM_Limits_AddParamsEx(const limitName[], any:...);
native VipM_Limits_RegisterTypeEvent(const sName[], const E_LimitEvent:iEvent, const sFunc[]);
native VipM_Limits_SetStaticValue(const sName[], const bool:bNewValue, const UserId = 0);
native T_LimitUnit:VipM_Limits_ReadFromJson(const JSON:jLimit);
native Array:VipM_Limits_ReadListFromJson(const JSON:jLimits, Array:aLimits = Invalid_Array);
native bool:VipM_Limits_Execute(const T_LimitUnit:iLimit, const UserId = 0);
native bool:VipM_Limits_ExecuteList(const Array:aLimits, const UserId = 0, const E_LimitsExecType:iType = Limit_Exec_OR);
forward VipM_Limits_OnInited();
```

### Items Controller API (`ItemsController.inc`)

```pawn
native IC_Init();
native T_IC_ItemType:IC_ItemType_Register(const name[]);
native IC_ItemType_SetEventListener(const T_IC_ItemType:type, const E_ItemTypeEvent:event, const functionName[]);
native IC_ItemType_AddParams(const T_IC_ItemType:type, any:...);
stock T_IC_ItemType:IC_ItemType_SimpleRegister(const name[], const onRead[] = "", const onGive[] = "");
native T_IC_Item:IC_Item_ReadFromJson(const JSON:instanceJson);
native Array:IC_Item_ReadArrayFromJson(const JSON:instancesJson, &Array:array = Invalid_Array);
native bool:IC_Item_Give(const playerIndex, const T_IC_Item:item);
native T_IC_Item:IC_Item_Free(&T_IC_Item:item);
stock bool:IC_Item_GiveArray(const playerIndex, const Array:array);
forward IC_ItemType_OnInited();
forward IC_Item_OnInited();
```

### WeaponMenu API (`VipM/M/WeaponMenu.inc`)

```pawn
native VipM_WeaponMenu_SetExpireStatus(const UserId, const sNewStatus[]);
```

---

## 11. Code Conventions

### Naming

| Kind | Convention | Example |
|------|-----------|---------|
| Local variables | `camelCase` | `playerIndex`, `menuIndex`, `params` |
| Global variables | `PascalCase` | `UserAutoOpen`, `UserLeftItems`, `Vips` |
| Public functions / API | `Namespace_PascalCase` | `VipM_Limits_RegisterType`, `ModuleType_Find` |
| Static (private) functions | `DefaultObjects_*` (prefix from file) + `static` | `static DefaultObjects_Limit_Map_GetCurrentName(...)` |
| Internal helpers | `_PascalCase` | `_Cmd_Menu`, `@OnModuleActivate` |
| Native/forward callbacks | `@FullName` (public is `@FunctionName`) | `@RG_CBasePlayer_Spawn`, `@Event_ModuleActivate` |
| Enum handle types | `T_Name` | `T_ModuleType`, `T_VipUnit`, `T_IC_ItemType` |
| Enum struct layouts | `S_Name` | `S_ModuleType`, `S_WeaponMenu` |
| Enum fields | `StructName_FieldName` | `ModuleType_Name`, `VipUnit_Access` |
| Constants / macros | `SCREAMING_SNAKE_CASE` | `MODULE_NAME`, `VIPM_MODULES_TYPE_NAME_MAX_LEN` |
| Enum (actual enumerations) | `E_Name` | `E_ModuleEvent`, `E_LimitEvent`, `E_LimitsExecType` |

**No Hungarian notation** — no `i`, `s`, `f`, `b`, `g`, `g_` prefixes on any variables. Legacy code has some (`g_aVips`, `gUserAutoOpen`, `iRet`, `sName`) — don't copy that into new code.

### Formatting

- 4-space indentation (no tabs)
- Braces on same line: `if (...) {`
- Single blank line between logical blocks; **two** blank lines between top-level functions
- Long argument lists: one per line, aligned to opening paren
- Multi-line conditions: each sub-condition on its own line, **operator at start**

### Conventions

- Use `PCGet_*` / `PCSingle_*` helpers from ParamsController — don't read Tries directly
- Prefer `Invalid_*` sentinel checks over magic `-1`
- Don't add error handling for cases the framework already guards against
- Use `CallOnce()` macro for init functions that must run once
- Use `plugin_precache` for all initialization (it's the earliest AMXX forward that runs)
- Module/Limit registration **must** happen in their respective `OnInited` forwards

### File structure conventions

- **Module plugins**: `VipM-M-ModuleName.sma`
- **Limit implementations**: `VipM/DefaultObjects/Limit/LimitName.inc`
- **Item Type implementations**: `ItemsController/DefaultObjects/ItemType/TypeName.inc`
- Built-in registrations in `VipM/DefaultObjects/Registrar.inc` (limits + param types)
- Built-in item types registered in `ItemsController.sma` main file

---

## 12. Creating a New Extension

### A. New Module

1. Create `amxmodx/scripting/VipM-M-YourModule.sma`
2. Use the template from Section 5
3. Register in `VipM_Modules_OnInited()`
4. The module is automatically activated when referenced in any VIP's `Modules` array
5. Update `docs/agents/module-system.md`

### B. New Limit Type

1. Create `amxmodx/scripting/VipM/DefaultObjects/Limit/YourLimit.inc`
2. Define a `DefaultObjects_Limit_YourLimit_Register()` function
3. Use `VipM_Limits_RegisterType()`, add params with `VipM_Limits_AddParamsEx()`, register event with `VipM_Limits_RegisterTypeEvent()`
4. Add `#include` and call in `VipM/DefaultObjects/Registrar.inc`
5. Update `docs/agents/limits-system.md`

### C. New Item Type (ItemsController)

1. Create `amxmodx/scripting/ItemsController/DefaultObjects/ItemType/YourType.inc`
2. Define a `DefaultObjects_ItemType_YourType_Register()` function
3. Use `IC_ItemType_SimpleRegister()` or `IC_ItemType_Register()` + `IC_ItemType_SetEventListener()`
4. Add params with `IC_ItemType_AddParams()`
5. Register in the `ItemsController.sma` init chain (see how other types are registered)
6. Update `docs/agents/items-system.md`

### D. New Config format

If you add new JSON keys or change how configs are read/merged, update `docs/agents/config-format.md`.

---

## 13. Build System

Defined in [`amxbuild.yml`](amxbuild.yml) (project root). Dependencies: ParamsController 1.4.2, CommandAliases 1.0.1, ReAPI 5.29.0.358. Build via GitHub Actions CI or the `amxx-builder` toolchain.

---

## 14. Internal Data Structures

### ArrayMap (custom)

String-keyed array map combining `Array` (sequential storage) + `Trie` (key→index lookup):

```pawn
enum ArrayMap { Array:AM_Arr, Trie:AM_Map }
#define ArrayMap(%1) %1[ArrayMap]

ArrayMapCreate(ArrayMap(am), cellSize, reserved);
ArrayMapPushCell/String/Array(am, value, key);
ArrayMapGetCell/String/Array(am, index);
ArrayMapGetCellByKey/StringByKey/ArrayByKey(am, key);
ArrayMapForeachArray(ArrayMap:idx => arr[S_Type]) { ... }
```

Used for: module type registry, limit type registry.

### g_tUserModules[playerIndex]

```pawn
static Trie:g_tUserModules[MAX_PLAYERS + 1] = {Invalid_Trie, ...};
// Structure: Trie<moduleName → Trie<params>>
// Created per-player on VipsManager_UserReload()
// Destroyed on VipsManager_UserReset()
```

### Handle types with Invalid sentinels

```pawn
enum T_VipUnit     { Invalid_VipUnit = -1 }
enum T_ModuleType  { Invalid_ModuleType = -1 }
enum T_ModuleUnit  { Invalid_ModuleUnit = -1 }
enum T_LimitType   { Invalid_LimitType = -1 }
enum T_LimitUnit   { Invalid_LimitUnit = -1 }
enum T_IC_ItemType { Invalid_IC_ItemType = -1 }
enum T_IC_Item     { Invalid_IC_Item = -1 }
```

All handles are array indices cast to enum types. `Invalid_*` sentinel is always `-1`.

---

## 15. Server Commands

| Command | Description |
|---------|-------------|
| `vipm_update_users` | Refresh privileges for all players (native: `VipM_UserUpdate`) |
| `vipm_info` | System info — modules, limits, vips count, versions |
| `vipm_modules` | Table of all registered modules and their active status |
| `vipm_limits` | Table of all registered limit types with properties |
| `ic_item_types` | Table of registered item types |

(Defined in `VipM/Core/SrvCmds.inc`)

---

## 16. Debugging

Project has a `VipM/DebugMode.inc` with `Dbg_*` macros:
- `Dbg_Log(...)` — conditional logging when debug is enabled
- `Dbg_PrintServer(...)` — server_print in debug mode

Enable by defining debug constant before compilation.

---

## 17. Documentation Maintenance Rules

After EVERY code change:
- New module → update `docs/agents/module-system.md`
- New/changed limit type → update `docs/agents/limits-system.md`
- New/changed item type → update `docs/agents/items-system.md`
- Config format change → update `docs/agents/config-format.md`
- Structural change → update affected sub-docs
- If change doesn't fit any sub-doc → create new one in `docs/agents/`

---

## 18. Dependency Versions

| Library | Min | Current | Docs |
|---------|-----|---------|------|
| ParamsController | 1.3.2 | 1.4.2 | [GitHub](https://github.com/AmxxModularEcosystem/ParamsController) |
| CommandAliases | 1.0.1 | 1.0.1 | [GitHub](https://github.com/AmxxModularEcosystem/CommandAliases) |
| ReAPI | 5.24.0.300 | 5.29.0.358 | [GitHub](https://github.com/rehlds/ReAPI) |
| AMXX | 1.10 | 1.10.5428 | — |

For ParamsController API details (param types, `PCGet_*` helpers, `PCSingle_*` helpers, `PCJson_*` functions), fetch its `README.md` or wiki from the repo above when needed.
