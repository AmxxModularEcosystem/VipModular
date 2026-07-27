# Module System

Modules are named, pluggable game features. A module plugin registers a type during init, the core activates it if referenced in configs, and the module then hooks into game events.

---

## Registration (in `VipM_Modules_OnInited`)

```pawn
public VipM_Modules_OnInited() {
    register_plugin(...);

    VipM_Modules_Register("SpawnItems");

    // paramName, paramTypeName, required
    VipM_Modules_AddParamsEx("SpawnItems",
        "Items",  IC_PARAM_TYPE_ITEMS_NAME,      true,
        "Limits", VIPM_PARAM_TYPE_LIMITS_NAME,   false
    );

    VipM_Modules_RegisterEvent("SpawnItems", Module_OnActivated, "@OnModuleActivate");
    // Optional: Module_OnRead, Module_OnMergeParams
}
```

`VipM_Modules_Register(name, Once = true)` — `Once = false` means the system will call `Module_OnMergeParams` when the same module appears from multiple VIP tiers for one player.

---

## Events

| Event | Signature | When called | Notes |
|-------|-----------|-------------|-------|
| `Module_OnActivated` | `()` | After config load, in `plugin_precache` | Register game hooks here. Return `VIPM_STOP` to cancel activation. |
| `Module_OnRead` | `(JSON:jCfg, Trie:p)` | While reading config, `plugin_precache` | Extend/override parsed params. Return `VIPM_STOP` to skip this module unit. |
| `Module_OnMergeParams` | `(Trie:p1, Trie:p2) → Trie` | When same module appears from 2+ VIP levels | Return `p1`, `p2`, or a new Trie. New Trie is auto-freed by core. Do not modify `p1`/`p2`. |

---

## Activation flow

```
VipsManager_LoadFromFile → reads all VipUnits
  each VipUnit's module entries mark their type as "Used"
ModuleType_ActivateUsed()
  for each used type:
    fires "VipM_Modules_OnActivate" forward (any plugin can return VIPM_STOP to block)
    if not blocked: Module_OnActivated event → module registers its ReAPI hooks, etc.
```

`VipM-ModulesLimiter` uses `VipM_Modules_OnActivate` to block modules based on map/conditions from `Modules.json`.

---

## Checking module access in-game

```pawn
// Simple check
if (!VipM_Modules_HasModule("SpawnItems", playerIndex))
    return;

// Get params
new Trie:p = VipM_Modules_GetParams("SpawnItems", playerIndex);

// Use ParamsController getters
PCGet_IcItemsGive(p, "Items", playerIndex);
PCGet_VipmLimitsCheck(p, "Limits", playerIndex, Limit_Exec_AND);
```

`VipM_Modules_GetParams` returns `Invalid_Trie` if player has no access → `HasModule` is just a null check.

---

## Player module state

`g_tUserModules[playerIndex]` in `VipM/Core/VipsManager.inc`:
- Created fresh on each `VipsManager_UserReload()`
- Key: module name string → Value: `Trie:params`
- Destroyed (with inner Tries) on `VipsManager_UserReset()`
- Updated via `VipM_UserUpdate(playerIndex)` native

---

## Minimal module skeleton

```pawn
#include <amxmodx>
#include <reapi>
#include <VipModular>

new const MODULE_NAME[] = "MyModule";

public VipM_Modules_OnInited() {
    register_plugin("VipM-M-MyModule", "1.0.0", "Author");

    VipM_Modules_Register(MODULE_NAME);
    VipM_Modules_AddParamsEx(MODULE_NAME,
        "SomeValue", "Integer", true
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
