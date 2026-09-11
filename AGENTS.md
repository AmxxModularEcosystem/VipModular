# VipModular — Agent Reference

> Modular VIP/privilege system for **Counter-Strike 1.6** (AMX Mod X, Pawn). Author: ArKaNeMaN

Конкретные версии компилятора, AMXX и зависимостей — в [`amxbuild.yml`](amxbuild.yml) и [`README.md`](README.md).

---

## 1. Architecture Overview

Система построена вокруг JSON-конфигов привилегий. Каждая привилегия (VipUnit) состоит из:

- **`Access`** — массив условий (Limits), определяющих, подходит ли игрок
- **`Modules`** — массив фич (Modules), которые игрок получает

Три расширяемых реестра типов:

| Реестр | Что регистрирует | Форвард регистрации |
|--------|-----------------|---------------------|
| Module types | Именованные игровые фичи | `VipM_Modules_OnInited()` |
| Limit types | Булевы условия доступа | `VipM_Limits_OnInited()` |
| Item types | Эффекты-предметы (ItemsController) | `IC_ItemType_OnInited()` |

Привилегии проверяются **сверху вниз** в [`Vips.json`](amxmodx/configs/plugins/VipModular/Vips.json). Если игрок проходит Access — модули привилегии добавляются в его набор. Одноимённые модули мержатся через `Module_OnMergeParams`, только если модуль зарегистрирован с `Once = false`; по умолчанию первый совпавший выигрывает.

---

## 2. Source Code Layout

Корень исходников — `amxmodx/scripting/`.

- **`VipModular.sma`** — ядро: инициализация, загрузка конфигов, жизненный цикл
- **`ItemsController.sma`** — независимый фреймворк эффектов-предметов
- **`VipM-Misc.sma`** — релоад, спавн/раунд триггеры
- **`VipM-ModulesLimiter.sma`** — покартное включение/отключение модулей
- **`VipM-M-*.sma`** — встроенные модули (SpawnItems, SpawnHealth, Vampire, VipInTab, WeaponMenu)

Вложенные исходники (не отдают публичное API, подключаются через `#include`):

- `VipM/Core/` — загрузка конфигов, управление привилегиями, серверные команды
- `VipM/DefaultObjects/` — встроенные лимиты, типы параметров, регистратор
- `VipM/WeaponMenu/` — подсистема меню оружия
- `VipM/` прочее — `Forwards.inc`, `ArrayMap.inc`, `ArrayTrieUtils.inc`, `Utils.inc`, `DebugMode.inc`
- `ItemsController/` — реестр типов предметов, реализация нативов, встроенные типы

Публичное API — только в `include/` (см. #6).

---

## 3. Config File Format

Корневая директория: `amxmodx/configs/plugins/VipModular/`.

- `File:` разрешается относительно этой папки, `/` — относительно `amxmodx/configs/`
- [`Vips.json`](amxmodx/configs/plugins/VipModular/Vips.json) — массив привилегий (`Access` + `Modules`); доп. файлы — из `Vips/*.json`
- [`Modules.json`](amxmodx/configs/plugins/VipModular/Modules.json) — покартное вкл/выкл модулей
- `WeaponMenu/*.json` — конфиги меню оружия

Параметры модулей/лимитов описываются через `PCParam()` из ParamsController: примитивы (`Integer`, `Float`, `Bool`, `String`) и ссылки (`VipM-Limit(s)`, `VipM-LimitType`, `VipM-ModuleType`, `IC-Item(s)`, `VipM-L-CounterType`). Актуальный набор — в `.inc` и в конфигах.

---

## 4. Lifecycle

### Инициализация (`plugin_precache`)

1. `register_plugin` / `register_library`, инициализация ParamsController
2. Создание реестров (ArrayMap) и подсистемы форвардов
3. Форварды инициализации — `VipM_Limits_OnInited()`, `VipM_Modules_OnInited()`, `IC_ItemType_OnInited()`
4. `VipM/Core/VipsManager.inc` парсит `Vips.json` и `Vips/`, создаёт VipUnit
5. `ModuleType_ActivateUsed()` активирует используемые модули (здесь `VipM-ModulesLimiter` может заблокировать модуль для карты)

### Подключение игрока

- `client_authorized` — статические лимиты (Steam, SteamId, IP, Bot)
- `client_putinserver` — Alive/Counter; через `RequestFrame` запускается `VipsManager_UserReload`
- **`VipsManager_UserReload`** перебирает VipUnit, проверяет Access, сохраняет/мержит параметры в `g_tUserModules[playerIndex]`, затем вызывает `VipM_OnUserUpdated()`
- `client_disconnected` — `VipsManager_UserReset()` очищает `g_tUserModules[playerIndex]`

Детали реализации — в `VipM/Core/`.

---

## 5. Extensible Registries

### Modules

Модуль — именованная игровая фича. Регистрируется в `VipM_Modules_OnInited()` и активируется автоматически при упоминании в любом VIP-конфиге. В игре модуль проверяет `VipM_Modules_HasModule(MODULE_NAME, playerIndex)` и читает параметры через `VipM_Modules_GetParams()`.

Нейминг: файл `VipM-M-<Name>.sma`, константа `MODULE_NAME`, имя плагина `[VipM-M] <Name>`.

API — [`include/VipM/Modules.inc`](amxmodx/scripting/include/VipM/Modules.inc); пример — [`VipM-M-VipInTab.sma`](amxmodx/scripting/VipM-M-VipInTab.sma).

### Limits

Лимит — булево условие доступа. **Dynamic** имеет параметры и колбэки `Limit_OnRead` / `Limit_OnCheck`; **Static** без параметров и колбэков, хранит результат в битмаске на игрока.

`VipM_Limits_ExecuteList(limits, playerIndex, E_LimitsExecType)` поддерживает `OR` / `AND` / `XOR`; Access по умолчанию — OR. Счётчики (Counter limit) ограничивают число использований — API в [`include/VipM/L/Counter.inc`](amxmodx/scripting/include/VipM/L/Counter.inc).

Встроенные типы и API — в `VipM/DefaultObjects/Limit/` и [`include/VipM/Limits.inc`](amxmodx/scripting/include/VipM/Limits.inc).

### ItemsController

Независимый фреймворк эффектов («предметов»); используется модулями SpawnItems, Vampire, WeaponMenu.

- **Item Type** (`T_IC_ItemType`) — класс эффекта, регистрируется плагином
- **Item Instance** (`T_IC_Item`) — конкретный предмет с распарсенными параметрами

Жизненный цикл: JSON → `IC_Item_ReadFromJson()` → `IC_Item_Give()` → `ItemType_OnGive` → `IC_Item_Free()`.

В модуле: параметр `PCParam("Items", IC_PARAM_TYPE_ITEMS_NAME)`, чтение через `PCGet_IcItemsGive(...)`; перед регистрацией таких параметров вызвать `IC_Init()`.

Встроенные типы и API — в `ItemsController/DefaultObjects/ItemType/` и [`include/ItemsController.inc`](amxmodx/scripting/include/ItemsController.inc).

### Forward System

Тонкая обёртка над `CreateMultiForward` / `ExecuteForward` — [`VipM/Forwards.inc`](amxmodx/scripting/VipM/Forwards.inc):

- **Registered** — живёт постоянно (например, `VipM_OnUserUpdated`)
- **RegAndCall** — создаётся, вызывается и уничтожается (одноразовые `*_OnInited`, `VipM_OnLoaded`)

Полный список — в [`VipM/Core/SrvCmds.inc`](amxmodx/scripting/VipM/Core/SrvCmds.inc) (`vipm_info`).

---

## 6. Public API

Всё публичное API объявлено в `include/`. **Не копируйте сигнатуры сюда** — источники истины это `.inc`-файлы:

`VipModular.inc`, `VipM/Modules.inc`, `VipM/Limits.inc`, `VipM/L/Counter.inc`, `VipM/M/WeaponMenu.inc`, `ItemsController.inc`. `VipM/Params.inc` — deprecated, используйте `PCGet_*`.

---

## 7. Code Conventions

### Нейминг

| Вид | Соглашение | Пример |
|-----|-----------|--------|
| Локальные переменные | `camelCase` | `playerIndex` |
| Глобальные переменные | `PascalCase` | `UserAutoOpen` |
| Публичные функции | `Namespace_PascalCase` | `VipM_Limits_RegisterType` |
| Приватные функции | `DefaultObjects_*` + `static` | `DefaultObjects_Limit_Map_GetCurrentName` |
| Внутренние хелперы | `_PascalCase` / `@PascalCase` | `_Cmd_Menu`, `@OnModuleActivate` |
| Handle types | `T_Name` | `T_ModuleType` |
| Struct layouts | `S_Name` | `S_ModuleType` |
| Enum поля | `StructName_FieldName` | `VipUnit_Access` |
| Enum (перечисления) | `E_Name` | `E_ModuleEvent` |
| Константы и макросы | `SCREAMING_SNAKE_CASE` | `MODULE_NAME` |

Без венгерской нотации (`i`, `s`, `b`, `g_` и т.п.).

### Форматирование

- Отступ — 4 пробела; открывающая скобка на той же строке: `if (x) {`
- Одна пустая строка между логическими блоками, **две** между функциями верхнего уровня
- Длинные аргументы — по одному на строку, выравнивание по открывающей скобке
- Многострочные условия — оператор в начале строки

### Соглашения

- Использовать `PCGet_*` / `PCSingle_*`, не читать Trie напрямую
- Предпочитать сентинелы `Invalid_*` вместо `-1`
- `CallOnce()` для инициализации; вся инициализация — в `plugin_precache`
- Регистрация модулей/лимитов — только в соответствующих `OnInited` форвардах

### Файловая структура

- Модули: `VipM-M-<Name>.sma`; лимиты: `VipM/DefaultObjects/Limit/<Name>.inc`
- Типы предметов: `ItemsController/DefaultObjects/ItemType/<Name>.inc`
- Регистрация встроенных лимитов/параметров: `VipM/DefaultObjects/Registrar.inc`

---

## 8. Creating a New Extension

- **Модуль** — новый `VipM-M-<Name>.sma`, регистрация в `VipM_Modules_OnInited()`; активируется автоматически при упоминании в VIP.
- **Лимит** — новый `VipM/DefaultObjects/Limit/<Name>.inc`, `#include` и вызов в `Registrar.inc`.
- **Тип предмета** — новый `ItemsController/DefaultObjects/ItemType/<Name>.inc`, регистрация в `ItemsController.sma`.

Точные функции регистрации — в соответствующих `.inc` (#6) и в примерах встроенных реализаций.

---

## 9. Internal Data Structures

- **ArrayMap** (Array + Trie) — [`VipM/ArrayMap.inc`](amxmodx/scripting/VipM/ArrayMap.inc); реестры типов модулей и лимитов.
- **`g_tUserModules[playerIndex]`** — пер-игроковый `Trie<module → Trie<params>>`; создаётся в `VipsManager_UserReload()`, очищается в `VipsManager_UserReset()`.
- **Handle types** (`T_VipUnit`, `T_ModuleType`, `T_LimitType`, `T_IC_Item`, …) — индексы массива, кастованные к enum-типу; сентинел `Invalid_*` = `-1`.

---

## 10. Server Commands

| Команда | Описание |
|---------|----------|
| `vipm_update_users` | Обновить привилегии всех игроков |
| `vipm_info` | Информация о системе |
| `vipm_modules` | Таблица модулей и их статусов |
| `vipm_limits` | Таблица типов лимитов |
| `ic_item_types` | Таблица типов предметов |

Исходники — [`VipM/Core/SrvCmds.inc`](amxmodx/scripting/VipM/Core/SrvCmds.inc).

---

## 11. Debugging

В [`VipM/DebugMode.inc`](amxmodx/scripting/VipM/DebugMode.inc) — `Dbg_Log(...)` (условное логирование) и `Dbg_PrintServer(...)`; включается дефайном при компиляции.

---

## 12. Build System

Определён в [`amxbuild.yml`](amxbuild.yml) в корне проекта; оттуда же читаются версии компилятора и зависимостей. Сборка — через `amxx-builder` или GitHub Actions CI.

---

## 13. Documentation Maintenance

При изменениях кода обновляйте соответствующий раздел: новый модуль/лимит/тип предмета — #5 и #8, формат конфигов — #3, структура исходников — #2. Сигнатуры нативов **не** копируйте сюда — читайте `.inc`.
