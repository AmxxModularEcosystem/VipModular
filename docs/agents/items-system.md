# Items System (ItemsController)

`ItemsController.sma` is a standalone framework for applying effects ("items") to players.  
It is independent from VipModular core but used by SpawnItems, Vampire, and WeaponMenu modules.

Public header: `include/ItemsController.inc`

---

## Concepts

- **Item Type** (`T_IC_ItemType`) — named effect class, registered by a plugin
- **Item Instance** (`T_IC_Item`) — one concrete item with parsed params, created from JSON
- Give an item: `IC_Item_Give(playerIndex, item)` → fires `ItemType_OnGive`

---

## Registration (in `IC_ItemType_OnInited`)

```pawn
public IC_ItemType_OnInited() {
    register_plugin(...);
    IC_Init();  // must call this in the plugin that registers types

    new T_IC_ItemType:t = IC_ItemType_Register("MyEffect");
    IC_ItemType_SetEventListener(t, ItemType_OnRead, "@OnRead");
    IC_ItemType_SetEventListener(t, ItemType_OnGive, "@OnGive");
    // IC_ItemType_SetEventListener(t, ItemType_OnFree, "@OnFree");  // optional
}
```

---

## Events

| Event | Signature | Return |
|-------|-----------|--------|
| `ItemType_OnRead` | `(JSON:jCfg, Trie:params)` | `IC_RET_READ_SUCCESS` or `IC_RET_READ_FAIL` |
| `ItemType_OnGive` | `(playerIndex, Trie:params)` | `IC_RET_GIVE_SUCCESS` or `IC_RET_GIVE_FAIL` |
| `ItemType_OnFree` | `(Trie:params)` | void — cleanup custom allocations |

---

## Built-in item types (18)

`ItemsController/DefaultObjects/ItemType/`:

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
| `If` | Conditional execution (check limit, then give item) |
| `ItemsList` | Ordered list of items |
| `Random` | Random item from a list |
| `CustomWeapon` | Spawn a custom weapon entity |

---

## Using ItemsController in a module

```pawn
// In VipM_Modules_OnInited:
VipM_Modules_AddParamsEx(MODULE_NAME,
    "Items", IC_PARAM_TYPE_ITEMS_NAME, true
);

// In game event handler:
new Trie:p = VipM_Modules_GetParams(MODULE_NAME, playerIndex);
PCGet_IcItemsGive(p, "Items", playerIndex);     // give list
// or:
PCGet_IcItemGive(p, "Item", playerIndex);       // give single
```

---

## Reading items from JSON manually

```pawn
// Single item
new T_IC_Item:item = IC_Item_ReadFromJson(jsonObject);
IC_Item_Give(playerIndex, item);
IC_Item_Free(item);

// List of items
new Array:items = IC_Items_ReadFromJson(jsonArray);
IC_Items_Give(playerIndex, items);
IC_Items_Free(items);
```

---

## Param type names (for `VipM_Modules_AddParamsEx`)

```pawn
IC_PARAM_TYPE_ITEM_NAME   // "IC-Item"   — single item
IC_PARAM_TYPE_ITEMS_NAME  // "IC-Items"  — array of items
```
