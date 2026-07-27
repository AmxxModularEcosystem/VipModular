# Config Format Reference

All configs are JSON, located in `amxmodx/configs/plugins/VipModular/`.  
Paths inside JSON are relative to that folder unless prefixed with `/` (then relative to `amxmodx/configs/`).

---

## Vips.json

Array of privilege objects, checked top-to-bottom. First match wins for each module.

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

- `Access` — array of Limit objects, evaluated with **OR** by default
- `Modules` — array of module config objects; `"Type"` is the module name
- `"File:Config": "Path/Name"` — loads `configs/plugins/VipModular/Path/Name.json` and merges it

Alternatively, put individual privilege files in `Vips/` folder — all `.json` files there are loaded automatically.

---

## Limit object

```json
{ "Type": "LimitTypeName", ...params }
```

Examples:
```json
{ "Type": "Flags",   "Flags": "ab" }
{ "Type": "Map",     "Prefix": "de_" }
{ "Type": "Map",     "Exact": "de_dust2" }
{ "Type": "Map",     "Regex": "^de_.*2$" }
{ "Type": "Time",    "From": "18:00", "To": "23:59" }
{ "Type": "Steam" }
{ "Type": "SteamId", "List": ["STEAM_0:1:12345"] }
{ "Type": "Round",   "Min": 1, "Max": 5 }
{ "Type": "Frags",   "Min": 10 }
{ "Type": "WeekDay", "Days": [1, 2, 3, 4, 5] }
{ "Type": "Always" }
{ "Type": "Never" }

// Logical combinator
{ "Type": "Logic", "Mode": "AND", "Limits": [
    { "Type": "Alive" },
    { "Type": "InBuyZone" }
]}
```

---

## Item object (ItemsController)

```json
{ "Type": "ItemTypeName", ...params }
```

Examples:
```json
{ "Type": "Weapon",  "Name": "weapon_ak47" }
{ "Type": "Health",  "Value": 100, "Add": true }
{ "Type": "Armor",   "Value": 100 }
{ "Type": "Money",   "Value": 1000, "Add": true }
{ "Type": "Speed",   "Value": 1.2 }
{ "Type": "DefuseKit" }
{ "Type": "RefillBpAmmo" }
{ "Type": "Command", "Value": "say_team VIP spawned" }

// Conditional: check limit, then give item
{ "Type": "If", "Limit": { "Type": "Alive" }, "Item": { "Type": "Health", "Value": 50 } }

// Random pick
{ "Type": "Random", "Items": [
    { "Type": "Weapon", "Name": "weapon_awp" },
    { "Type": "Weapon", "Name": "weapon_ak47" }
]}

// List
{ "Type": "ItemsList", "Items": [
    { "Type": "Weapon", "Name": "weapon_deagle" },
    { "Type": "Armor",  "Value": 100 }
]}
```

---

## Modules.json (VipM-ModulesLimiter)

Array of rules; each rule specifies conditions and which modules to enable/disable.  
Checked by `VipM-ModulesLimiter.sma` in `VipM_Modules_OnActivate` forward.

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

---

## File reference syntax

In any JSON field value:
- `"File:Path/Name"` → loads `configs/plugins/VipModular/Path/Name.json`
- `"File:/configs/Path/Name"` → loads `amxmodx/configs/Path/Name.json`

ParamsController resolves these transparently when calling `PCJson_ParseFile`.

---

## Param type names (for `AddParamsEx`)

| Name | Type | Usage |
|------|------|-------|
| `"Integer"` | `int` | |
| `"Float"` | `float` | |
| `"Bool"` | `bool` | |
| `"String"` | `string` | |
| `"VipM-Limit"` | `T_LimitUnit` | Single limit |
| `"VipM-Limits"` | `Array:T_LimitUnit` | List of limits |
| `"VipM-LimitType"` | `T_LimitType` | Limit type reference |
| `"VipM-ModuleType"` | `T_ModuleType` | Module type reference |
| `"IC-Item"` | `T_IC_Item` | Single item |
| `"IC-Items"` | `Array:T_IC_Item` | List of items |
