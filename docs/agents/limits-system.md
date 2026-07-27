# Limits System

Limits are boolean conditions that gate access to privileges, modules, or items.  
They are read from JSON configs and evaluated at runtime.

---

## Two kinds of limits

| Kind | How it works | Use case |
|------|-------------|---------|
| **Dynamic** | Has params + `Limit_OnRead` / `Limit_OnCheck` callbacks | Most conditions (Map, Flags, Time, Frags…) |
| **Static** | No params, value is set programmatically via `VipM_Limits_SetStaticValue` | Fast per-player flags (e.g. Steam validation status, alive check) |

Static limit types store their result in a bitmask per player; no forward is called on check.

---

## Registration (in `VipM_Limits_OnInited`)

```pawn
public VipM_Limits_OnInited() {
    register_plugin(...);

    // Dynamic limit
    VipM_Limits_RegisterType("MyLimit",
        .bForPlayer = true,   // depends on a specific player
        .bStatic    = false   // dynamic
    );
    VipM_Limits_AddParamsEx("MyLimit",
        "MinFrags", "Integer", true
    );
    VipM_Limits_RegisterTypeEvent("MyLimit", Limit_OnCheck, "@OnCheck");

    // Static limit (no params, no callback)
    VipM_Limits_RegisterType("HasPremium",
        .bForPlayer = true,
        .bStatic    = true
    );
    VipM_Limits_SetStaticValue("HasPremium", false); // default for all
}
```

---

## Events

| Event | Signature | Purpose |
|-------|-----------|---------|
| `Limit_OnRead` | `(JSON:jUnit, Trie:tParams)` | Post-parse hook — modify params or abort (`VIPM_STOP`) |
| `Limit_OnCheck` | `(Trie:tParams, playerIndex) → bool` | Evaluate the condition for a player |

`Limit_OnCheck` is **not called** for static limits — the core reads the bitmask directly.

---

## Built-in limit types (24)

`VipM/DefaultObjects/Limit/`:

| Type | File | Notes |
|------|------|-------|
| `Flags` | Flags.inc | Admin flags bitmask |
| `Steam` | Steam.inc | Steam validation status (static) |
| `SteamId` | SteamId.inc | Specific Steam IDs list |
| `Ip` | Ip.inc | IP address match |
| `Map` | Map.inc | Map name (prefix / exact / regex) |
| `Time` | Time.inc | Server clock range |
| `RoundTime` | RoundTime.inc | Time elapsed in current round |
| `Round` | Round.inc | Round number |
| `GameTime` | GameTime.inc | Total game time |
| `Alive` | Alive.inc | Player alive state (static) |
| `Frags` | Frags.inc | Kill count range |
| `WasKilled` | WasKilled.inc | Death state this round |
| `InBuyZone` | InBuyZone.inc | Buy zone presence |
| `InFreezyTime` | InFreezyTime.inc | Freeze time active |
| `HasPrimaryWeapon` | HasPrimaryWeapon.inc | Primary weapon in inventory |
| `LifeTime` | LifeTime.inc | Time connected |
| `WeekDay` | WeekDay.inc | Day of week |
| `Bot` | Bot.inc | Is bot |
| `Name` | Name.inc | Player name match |
| `Counter` | Counter.inc | Counter limit (max uses) |
| `OncePer` | OncePer.inc | One-time per period |
| `Logic` | Logic.inc | AND / OR / NOT over nested limits |
| `Always` | Always.inc | Always true (static) |
| `Never` | Never.inc | Always false (static) |

---

## Executing limits

```pawn
// Execute a single limit unit
bool:VipM_Limits_Execute(T_LimitUnit:iLimit, UserId = 0)

// Execute a list with a logical operator (OR / AND / XOR)
bool:VipM_Limits_ExecuteList(Array:aLimits, UserId = 0, E_LimitsExecType:iType = Limit_Exec_OR)

// Convenience: read a limit from a params Trie and check it
PCGet_VipmLimitCheck(Trie:p, "LimitKey", playerIndex, bool:def = true)
PCGet_VipmLimitsCheck(Trie:p, "LimitsKey", playerIndex, Limit_Exec_AND, bool:def = true)
```

`E_LimitsExecType`: `Limit_Exec_OR`, `Limit_Exec_AND`, `Limit_Exec_XOR`

---

## Limit in JSON config

```json
// Single limit object
{ "Type": "Flags", "Flags": "a" }

// As access condition in Vips.json
"Access": [
    { "Type": "Flags", "Flags": "t" },
    { "Type": "Map",   "Prefix": "de_" }
]
// default exec type is OR: either condition passes

// Logic limit for AND
{ "Type": "Logic", "Mode": "AND", "Limits": [
    { "Type": "Alive" },
    { "Type": "InBuyZone" }
]}
```

---

## Setting static values

Static limits must have their value pushed externally (e.g. on spawn or auth):

```pawn
// Set for all players
VipM_Limits_SetStaticValue("MyStaticLimit", true);

// Set for specific player
VipM_Limits_SetStaticValue("Steam", is_user_steam(playerIndex), playerIndex);
```
