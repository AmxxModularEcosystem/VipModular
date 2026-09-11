---
name: vipm-config-items
description: >-
  Author ItemsController item objects (the "Item" JSON objects) used by
  VipModular: exact shape {"Item": "<Type>", ...params}, the 16 built-in item
  types with their parameter names/types, nesting via ItemsList/Random/If,
  GiveType semantics and File: references. Use when configuring what a player
  actually receives in SpawnItems, WeaponMenu, Vampire or ItemsController
  module params; writing item JSON; or debugging why an item is dropped.
  Triggers: ItemsController, item object, "Item", SpawnItems Items, Weapon item,
  Money item, ItemsList, If item, Random item, GiveType, предмет, объект
  предмета, выдача предметов.
---

# ItemsController — Построение объектов предметов

## Когда применять / When to use

- Нужно описать, **что именно** получает игрок: оружие, здоровье/броню, деньги,
  гранаты, предмет «если-то», случайный предмет и т.д.
- Объекты предметов встречаются:
  - в параметрах модулей (`SpawnItems` → `Items`, `Vampire` и т.п.);
  - в пунктах меню WeaponMenu (`Menu.Items[].Items`);
  - в переиспользуемых файлах (`configs/plugins/VipModular/Items/*.json`),
    подключаемых через `"File:…"`;
  - вложенно — внутри `ItemsList`, `Random`, `If`.
- Поиск параметров типа в исходниках → скилл `vipm-config-type-lookup`.

## Структура объекта предмета

```jsonc
{
    "Item": "<ItemType>",          // имя зарегистрированного типа
    "<ParamKey>": "<ParamValue>"   // параметры конкретного типа
}
```

- `<ItemType>` — имя типа, зарегистрированного плагином-расширением (напр.
  `Weapon`, `Health`, `ItemsList`).
- Параметры и их типы задаёт сам тип. Неизвестные/отсутствующие обязательные
  параметры → предмет **отбрасывается** при чтении.
- Предмет можно вынести в файл: `configs/plugins/VipModular/Items/DefuseKit.json`
  → ссылка `"File:Items/DefuseKit"`.
- В массивах предметов элемент может быть либо объектом, либо строкой-ссылкой,
  либо ещё одним массивом (см. `IC-Items`).

## Встроенные типы предметов (16) и их параметры

| `"Item"` | Параметры (`Имя : Тип`, *обяз.*) | Прочее |
|---|---|---|
| `Weapon` | `Name` : String *, `BpAmmo` : Integer | `GiveType` (из raw-JSON) |
| `Health` | `Health` : Float *, `MaxHealth` : Float, `SetHealth` : Bool | |
| `Armor` | `Armor` : Integer *, `MaxArmor` : Integer, `SetArmor` : Bool, `Helmet` : Bool | |
| `DefuseKit` | — | |
| `Money` | `Amount` : Integer *, `TrackChange` : Bool | `GiveType` (raw). ⚠️ см. баг ниже |
| `Speed` | `Multiplier` : Float * | |
| `DamageMult` | `Given` : Float, `Taken` : Float | |
| `InstantReload` | — | |
| `InstantReloadAllWeapons` | — | |
| `RefillBpAmmo` | — | |
| `Command` | `Command` : String *, `ByServer` : Bool | |
| `Function` | `Plugin` : String *, `Function` : String *, `Value` : Integer, `Flags` : ShortString, `Days` : Integer | вызов функции плагина |
| `If` | `Limits` : `VipM-Limits` *, `Items` : `IC-Items`, `ElseItems` : `IC-Items` | ветвление |
| `ItemsList` | `Items` : `IC-Items` * | выдать всё по списку |
| `Random` | `Items` : `IC-Items` * | выдать один случайный |
| `CustomWeapon` | — | raw: `Name`, `GiveType`, `Controller` (`WSS`/`CWAPI`/`AUW`/`UW`) |

> Полный актуальный список для вашей сборки — командой `ic_item_types`; имена и
> параметры — через `vipm-config-type-lookup`.

## Семантика `GiveType`

- `Weapon.GiveType`: `Add` | `Replace` | `Drop` / `DropAndReplace`
  (по умолчанию — drop-and-replace).
- `Money.GiveType`: `Set` | `Add` (по умолчанию — add).
- `CustomWeapon.GiveType` и `Controller` — собственные значения типа.
- `GiveType` у `Weapon`/`Money`/`CustomWeapon` не входит в список параметров
  типа, но в конфиге это обычное валидное поле.

## Примеры из репозитория

Оружие + боезапас, режим «заменить»:

```jsonc
{ "Item": "Weapon", "Name": "weapon_deagle", "GiveType": "Replace" }
{ "Item": "Weapon", "Name": "weapon_flashbang", "BpAmmo": 2, "GiveType": "Add" }
```

Простейший предмет (файл `Items/DefuseKit.json`):

```json
{ "Item": "DefuseKit" }
```

Список предметов (`Items/AllGrenades.json`), подключается как `"File:Items/AllGrenades"`:

```jsonc
{
    "Item": "ItemsList",
    "Items": [
        { "Item": "Weapon", "Name": "weapon_hegrenade", "GiveType": "Add" },
        { "Item": "Weapon", "Name": "weapon_smokegrenade", "GiveType": "Add" },
        { "Item": "Weapon", "Name": "weapon_flashbang", "BpAmmo": 2, "GiveType": "Add" }
    ]
}
```

Условная выдача:

```jsonc
{
    "Item": "If",
    "Limits": [ { "Limit": "Alive" } ],
    "Items": [ { "Item": "Money", "Amount": 500 } ],
    "ElseItems": [ { "Item": "Health", "Health": 50, "SetHealth": true } ]
}
```

Случайный предмет из набора:

```jsonc
{
    "Item": "Random",
    "Items": [
        { "Item": "Weapon", "Name": "weapon_ak47" },
        { "Item": "Weapon", "Name": "weapon_m4a1" }
    ]
}
```

## Композиция (вложенность)

- `ItemsList` — выдаёт все предметы массива `Items` последовательно.
- `Random` — выбирает и выдаёт один случайный элемент `Items`.
- `If` — проверяет `Limits` (AND, зависит от игрока) и выдаёт `Items` либо
  `ElseItems`. Внутри `If.Limits` доступны любые лимиты, включая зависимые от
  игрока (см. `vipm-config-limits`).
- Любой элемент `Items`/`ElseItems` может быть объектом, строкой `"File:…"` или
  вложенным массивом.

## Частые ошибки / Gotchas

- **`Money`**: тип объявляет параметр `TrackChange`, но обработчик читает
  `TrackChanges` — объявленный параметр фактически не работает. Документируйте
  `TrackChange` (как в регистрации), но знайте о несоответствии.
- `GiveType` у `Weapon`/`Money`/`CustomWeapon` не указан в списке параметров, но
  в конфиге он валиден — задавайте его как обычное поле.
- Опечатка в `"Item"` или отсутствие обязательного параметра → предмет молча
  отбрасывается при чтении.
- `If.Limits` использует **ключ `Limits`** (мн.ч.), а не `Limit`.
- Типы без параметров (`DefuseKit`, `InstantReload`, …) принимают только сам
  объект с `"Item"`.
- `readme/extensions/items.md` устаревший: он утверждает, что «у предметов нет
  списка параметров» — в текущей версии параметры у предметов есть (см. таблицу
  выше).

## Чек-лист перед завершением

- [ ] Имя `"Item"` совпадает с зарегистрированным типом (`ic_item_types` / исходники).
- [ ] Все обязательные параметры указаны, значения соответствуют типам.
- [ ] `GiveType` задан там, где важно поведение выдачи.
- [ ] Вложенные `Items`/`ElseItems` корректны и не пусты.
- [ ] `File:`-ссылки ведут на существующие `Items/*.json` (без расширения).
- [ ] JSON валиден, комментарии допустимы, висячих запятых нет.
