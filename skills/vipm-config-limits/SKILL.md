---
name: vipm-config-limits
description: >-
  Author VipModular limit (Access condition) objects: exact shape
  {"Limit": "<Type>", ...params}, all 31 built-in limit types with parameter
  names/types, player-bound vs player-free and static vs dynamic rules, and
  Logic-AND/OR/XOR/NOT composition. Use when writing Access filters, per-menu
  ActiveLimits, module Limits/AutoopenLimits, Modules.json activation rules, or
  debugging why a privilege does not trigger. Triggers: VipModular limit,
  "Limit", Access condition, Logic-AND, Logic-NOT, Flags, Steam, SteamId, Map,
  Round, Counter, OncePer, лимит, условие доступа, проверка доступа.
---

# VipModular (ядро) — Построение лимитов

## Когда применять / When to use

- Нужно задать **условие** доступа к привилегии или к конкретному функционалу
  (пункт меню, автооткрытие, выдача предметов).
- Лимиты встречаются в: `Access` (Vips.json), `Limits` и `AutoopenLimits`
  (модуль WeaponMenu), `ActiveLimits` (пункт меню), `Limits` (модули
  SpawnItems/SpawnHealth/Vampire), `Limits` (ItemsController `If`), `Limits`
  (Modules.json).
- **Скилл только про настройку** (конфигурацию лимитов). Разработка не
  рассматривается. Поиск имён/параметров → `vipm-config-type-lookup`.

## Структура объекта лимита

```jsonc
{
    "Limit": "<LimitType>",
    "<ParamKey>": "<ParamValue>"
}
```

- `<LimitType>` — имя зарегистрированного лимита (см. таблицу ниже).
- Если лимит один, в списках допускается объект вместо массива; список
  выполняется через **OR** (если не обёрнут в `Logic-*`).
- Допустима ссылка на файл: `"File:Limits/MinRound-2"` → `Limits/MinRound-2.json`.

## Две классификации лимитов

| Свойство | Значения | Что значит для конфига |
|---|---|---|
| Зависимость от игрока | `ForPlayer` да/нет | Лимоны «от игрока» нельзя использовать в `Modules.json` (проверка не на игроке) |
| Статический / динамический | static / dynamic | В JSON выглядят одинаково; статические просто не имеют параметров и работают быстрее |

## Встроенные типы лимитов (31 тип из 24 файлов)

| `"Limit"` | Зав. от игрока | Параметры (`Имя : Тип`, *обяз.*) |
|---|:---:|---|
| `ForAll`, `Always` | нет | — (всегда true) |
| `Never` | нет | — (всегда false) |
| `Alive` | да | — |
| `Bot` | да | — |
| `Steam` | да | — (Steam-аккаунт) |
| `WasAlive`, `WasKilled` | да | — |
| `SteamId` | да | `SteamId` : ShortString * |
| `Flags` | да | `Flags` : Flags *, `Strict` : Boolean |
| `Ip` | да | `Ip` : ShortString * |
| `Map` | да | `Map` : ShortString, `Regexp` : Regexp, `Real` : Boolean, `Prefix` : Boolean |
| `Name` | да | `Name` : ShortString * |
| `Counter` | да | `Type` : `VipM-L-CounterType` *, `Key` : ShortString *, `Max` : Integer, `Inc` : Integer |
| `Frags` | да | `Min` : Integer, `Max` : Integer |
| `HasPrimaryWeapon` | да | `HasNot` : Boolean |
| `InBuyZone` | да | `Reverse` : Boolean |
| `LifeTime` | да | `Min` : TimeInterval, `Max` : TimeInterval |
| `OncePerRound`, `OncePerMap`, `OncePerGame` | да | — |
| `Time` | нет | `Before` : Time, `After` : Time |
| `Round` | нет | `Min` : Integer, `Max` : Integer |
| `RoundTime` | нет | `Min` : TimeInterval, `Max` : TimeInterval |
| `GameTime` | нет | `Min` : TimeInterval, `Max` : TimeInterval |
| `WeekDay` | нет | `Day` : WeekDay * (один день) |
| `InFreezyTime` | нет | `Reverse` : Boolean |
| `Logic-OR`, `Logic-AND`, `Logic-XOR`, `Logic-NOT` | нет | `Limits` : `VipM-Limits` * |

> Типы параметров в таблице (`Flags`, `Time`, `TimeInterval`, `WeekDay`,
> `Regexp`, …) описаны в скилле `param-types`; тип `VipM-L-CounterType` —
> в скилле `vipm-core-param-types`.

## Композиция условий: `Logic-*`

Все `Logic-*` принимают вложенный список `Limits` и комбинируют его:

```jsonc
{
    "Limit": "Logic-AND",
    "Limits": [
        { "Limit": "Round", "Min": 4 },
        { "Limit": "HasPrimaryWeapon", "HasNot": true }
    ]
}
```

- `Logic-AND` — все истинны.
- `Logic-OR` — хотя бы одна.
- `Logic-XOR` — ровно одна.
- `Logic-NOT` — инверсия всего списка (внутри — AND).

Вложенность не ограничена — можно собирать деревья условий.

## Примеры из репозитория

Файл `Limits/MinRound-2.json` (переиспользуется через `"File:Limits/MinRound-2"`):

```json
{ "Limit": "Round", "Min": 2 }
```

`Limits/Menus/AwpAccess.json`:

```jsonc
{
    "Limit": "Logic-AND",
    "Limits": [ { "Limit": "Round", "Min": 4 } ]
}
```

`Limits/Menus/AutoOpen.json`:

```jsonc
{
    "Limit": "Logic-AND",
    "Limits": [ { "Limit": "HasPrimaryWeapon", "HasNot": true } ]
}
```

`Access` в `Vips.json` (по умолчанию OR — достаточно флага `t` ИЛИ Steam):

```jsonc
"Access": [
    { "Limit": "Flags", "Flags": "t" },
    { "Limit": "Steam" }
]
```

`Modules.json` (свободные от игрока лимиты; `Real` = «настоящее» имя карты):

```jsonc
"Limits": {
    "Limit": "Logic-NOT",
    "Limits": { "Limit": "Map", "Regexp": "^(\\$|aim_|awp_|fy_)", "Real": true }
}
```

## Правила применения

- В `Access` — OR по умолчанию. Нужно «и» — оберните в `Logic-AND`.
- В `Modules.json` нельзя использовать зависимые от игрока лимиты.
- `Map` без `Prefix: true` — точное совпадение; `Prefix: true` — сравнение по
  префиксу; `Regexp` — регулярка; `Real: true` — реальное имя карты вместо
  переопределённого (`amx_mapname`).
- `Counter` ограничивает число срабатываний: `Max` — предел, `Inc` — шаг
  (по умолчанию 1). `Key` — произвольный ключ счётчика.
- `Time`: `After`/`Before` — секунды от начала суток; учитывайте переход через
  полночь (см. исходник `Time.inc`).

## Частые ошибки / Gotchas

- Ключ `"Limit"` — не `"Type"`, не `"Module"`.
- `HasPrimaryWeapon` использует `HasNot` (не `Reverse`); `InBuyZone`/`InFreezyTime`
  — `Reverse`.
- `WeekDay.Day` — **один** день (число), не массив.
- `SteamId.SteamId` — **строка**, не список.
- `Map` — параметры `Map`/`Regexp`/`Real`/`Prefix` (не `Exact`).
- ⚠️ В исходнике `Ip.inc` обработчик читает ключ `"SteamId"` вместо `"Ip"` —
  регистрация авторитетна (`Ip`), но лимит может не срабатывать как ожидается.
- В `Modules.json` зависящий от игрока лимит (напр. `Flags`) недопустим.
- В JSON регекспы экранируются: `"^(\\$|fy_)"`, а не `"^(\$|fy_)"`.

## Чек-лист перед завершением

- [ ] Ключ `"Limit"` и имя типа подтверждены (`vipm_limits` / `vipm-config-type-lookup`).
- [ ] Все параметры указаны с корректными типами; обязательные — присутствуют.
- [ ] Для «все условия» использован `Logic-AND` (помните про OR по умолчанию).
- [ ] В `Modules.json` нет зависимых от игрока лимитов.
- [ ] Regex корректно экранирован; `File:`-ссылки валидны.
- [ ] Проверено, что условие действительно выполняется в нужный момент
      (раунд/время/карта/игрок).
