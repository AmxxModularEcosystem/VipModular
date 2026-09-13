---
name: vipm-config-modules
description: >-
  Configure VipModular modules (the "Module" JSON objects): exact object shape,
  the 5 built-in modules (SpawnItems, SpawnHealth, Vampire, VipInTab,
  WeaponMenu) with all parameters, WeaponMenu nested menu/menu-item keys, and
  top-down precedence / Once merge semantics. Configuration only — no plugin
  development. Use when configuring what abilities a privilege grants, adding a
  module block to Vips.json, or setting up WeaponMenu menus. Triggers:
  VipModular module, "Module", SpawnItems, SpawnHealth, Vampire, VipInTab,
  WeaponMenu, module params, module merge, VIP module, модуль, настройка модуля,
  параметры модуля.
---

# VipModular (ядро) — Построение модулей

## Когда применять / When to use

- Нужно указать **что получает** игрок с привилегией: предметы, здоровье/броню,
  вампиризм, статус в TAB, доступ к меню оружия.
- Модули задаются: в `Vips.json` (`Modules`), в отдельных файлах
  (`configs/plugins/VipModular/<Module>/<Name>.json`), подключаемых через
  `"File:…"`, либо inline-объектом.
- **Скилл только про настройку** (конфигурацию модулей). Разработка не
  рассматривается.
- Тело лимитов → `vipm-config-limits`; предметов → `vipm-config-items`; поиск
  имён/параметров (в т.ч. в deps/repos) → `vipm-config-type-lookup`.

## Структура объекта модуля

```jsonc
{
    "Module": "<ModuleName>",
    "<ParamKey>": "<ParamValue>"
}
```

- Строка `"File:WeaponMenu/Vip"` — допустимая альтернатива объекту.
- `"Menus"` и `"Items"` внутри модулей принимают вложенные объекты и ссылки.

> Типы параметров в таблицах (`Integer`, `Boolean`, `String`, `VipM-Limits`, `VipM-L-CounterType`, `IC-Items`, …) описаны в скиллах `param-types`, `vipm-core-param-types`, `vipm-ic-param-types`.

### Приоритет и слияние (Once)

- Привилегии идут **сверху вниз**; первый модуль с данным именем выигрывает.
- `Once = true` (по умолчанию у всех встроенных): параметры **не объединяются**,
  берётся первый.
- Если модуль поддерживает слияние (`Once = false`), параметры двух модулей
  одного типа объединяются по правилам модуля; при отсутствии поддержки всегда
  берётся первый.

## Встроенные модули (5)

### SpawnItems — выдача предметов при спавне

| Параметр | Тип | Обяз. | Описание |
|---|---|:---:|---|
| `Items` | `IC-Items` | да | предметы для выдачи (см. `vipm-config-items`) |
| `Limits` | `VipM-Limits` | нет | условия выдачи (по умолчанию — всегда) |

```jsonc
{
    "Module": "SpawnItems",
    "Limits": [ "File:Limits/MinRound-2" ],
    "Items": [
        { "Item": "Weapon", "Name": "weapon_deagle", "GiveType": "Replace" },
        "File:Items/DefuseKit"
    ]
}
```

### SpawnHealth — здоровье и броня при спавне

| Параметр | Тип | Обяз. | Описание |
|---|---|:---:|---|
| `Health` | Integer | нет | сколько здоровья (default 0) |
| `SetHealth` | Boolean | нет | `true` — установить, `false` — добавить (default `true`) |
| `MaxHealth` | Integer | нет | предел здоровья при добавлении |
| `Armor` | Integer | нет | сколько брони (default 0) |
| `SetArmor` | Boolean | нет | `true` — установить, `false` — добавить (default `true`) |
| `MaxArmor` | Integer | нет | предел брони (default 100) |
| `Helmet` | Boolean | нет | выдать шлем (default `false`) |
| `Limits` | `VipM-Limits` | нет | условия |

```jsonc
{ "Module": "SpawnHealth", "Limits": ["File:Limits/MinRound-2"],
  "Armor": 100, "SetArmor": true, "Helmet": true }
```

### Vampire — здоровье за убийства

| Параметр | Тип | Обяз. | Описание |
|---|---|:---:|---|
| `ByKill` | Integer | нет | ХП за обычное убийство (default 0) |
| `ByHead` | Integer | нет | за убийство в голову (default = `ByKill`) |
| `ByKnife` | Integer | нет | за убийство ножом (default = `ByKill`) |
| `ByGrenade` | Integer | нет | за убийство гранатой (default = `ByKill`) |
| `MaxHealth` | Integer | нет | предел здоровья |
| `Limits` | `VipM-Limits` | нет | условия |

```jsonc
{ "Module": "Vampire", "ByKill": 5, "ByHead": 7, "ByKnife": 10, "MaxHealth": 100 }
```

### VipInTab — метка VIP в таблице счёта

| Параметр | Тип | Обяз. | Описание |
|---|---|:---:|---|
| `Enabled` | Boolean | да | показывать ли VIP-статус |
| `Override` | Boolean | нет | перекрывать ли другие статусы (Bomb/Dead…), default `false` |

```jsonc
{ "Module": "VipInTab", "Enabled": true, "Override": false }
```

### WeaponMenu — меню выбора оружия

| Параметр | Тип | Обяз. | Описание |
|---|---|:---:|---|
| `Menus` | массив объектов | **да** | список меню (читается из raw-JSON; без него конфиг отбрасывается) |
| `MainMenuTitle` | String | нет | заголовок главного меню (default из ланг-файла) |
| `Count` | Integer | нет | лимит предметов за раунд (default — неограничено) |
| `CounterType` | `VipM-L-CounterType` | нет | тип сброса счётчика |
| `CounterKey` | ShortString | нет | ключ счётчика |
| `ResetCountOnSpawn` | Boolean | нет | **deprecated**; сброс счётчика при спавне |
| `Limits` | `VipM-Limits` | нет | условия доступности меню |
| `AutoopenLimits` | `VipM-Limits` | нет | условия авто-открытия |
| `AutoopenDelay` | Float | нет | задержка авто-открытия, сек (default 0.0) |
| `AutoopenCloseDelay` | Float | нет | задержка авто-закрытия, сек |
| `AutoopenMenuNum` | Integer | нет | номер меню из `Menus` для авто-открытия |
| `StayOpen` | Boolean | нет | не закрывать меню после выбора (default `false`) |
| `StayOpen_CheckCounter` | Boolean | нет | проверять остаток при повторном открытии (default `false`) |
| `StayOpen_WhenRestricted` | Boolean | нет | оставаться открытым при ограничении |

> Если в `Menus` указано **одно** меню — оно открывается сразу, минуя главное.

#### Поля объекта меню (`Menus[]`)

| Поле | Тип | Описание |
|---|---|---|
| `Name` | String | текст пункта в главном меню |
| `Title` | String | заголовок меню |
| `Limits` | `VipM-Limits` | условия открытия меню |
| `Items` | массив пунктов | пункты меню |
| `Fake` | Boolean | `true` — меню недоступно, показывается неактивным |
| `FakeMessage` | String | сообщение при попытке открыть `Fake`-меню |
| `BackOnExit` | Boolean | вместо выхода — пункт возврата в главное меню |
| `PerPage` | Integer | пунктов на странице (default 7) |
| `ShowPage` | Boolean | показывать счётчик страниц |

#### Поля объекта пункта меню (`Menus[].Items[]`)

| Поле | Тип | Описание |
|---|---|---|
| `Title` | String | текст пункта (без `Items` — некликабельный текст; без `Title` — пропуск строки) |
| `Items` | `IC-Items` | предметы, выдаваемые при выборе |
| `UseCounter` | Boolean | учитывать ли счётчик предметов (default `true`) |
| `Limits` | `VipM-Limits` | условия выдачи предметов |
| `ActiveLimits` | `VipM-Limits` | условия активности пункта (default — всегда активен) |
| `ShowLimits` | `VipM-Limits` | условия отображения пункта (default — всегда) |
| `FakeInactive` | Boolean | `true` — неактивный пункт можно выбрать, но предметы не выдадутся |

Полный пример:

```jsonc
{
    "Module": "WeaponMenu",
    "Count": 2,
    "Limits": [ "File:Limits/MinRound-2" ],
    "AutoopenLimits": "File:Limits/Menus/AutoOpen",
    "Menus": [
        "File:WeaponMenu/Menus/Vip",
        {
            "Name": "Premium-меню [недоступно]",
            "Fake": true,
            "FakeMessage": "Приобретите услугу."
        }
    ]
}
```

## Частые ошибки / Gotchas

- `WeaponMenu` **требует** `Menus`; без него конфиг модуля отбрасывается с
  предупреждением (WARNING).
- Названия в `Vips.json` должны быть в порядке от «сильной» к «слабой» —
  иначе модуль молча перекрывается.
- `Limits` внутри модуля по умолчанию выполняется через **AND** — в отличие от
  `Access` (OR).
- `Items`/`Menus` принимают ссылки `"File:…"` на любом уровне вложенности.
- `ResetCountOnSpawn` — устаревший параметр; используйте `CounterType`.

## Чек-лист перед завершением

- [ ] Имя `"Module"` подтверждено исходниками (`vipm_modules` / `vipm-config-type-lookup`).
- [ ] Все обязательные параметры указаны; значения соответствуют типам.
- [ ] Для модулей с предметами/меню вложенные `Items`/`Menus` валидны.
- [ ] Учтён порядок привилегий и семантика `Once`.
- [ ] `Limits`/`AutoopenLimits` составлены по `vipm-config-limits`.
