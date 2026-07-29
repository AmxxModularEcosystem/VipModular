# VipModular — Agent Reference

> Modular VIP/privilege system for **Counter-Strike 1.6** (AMX Mod X, Pawn).  
> Version: `5.0.0-rc4f2` | Author: ArKaNeMaN  
> Stack: AMXX 1.10, ReAPI 5.29, ParamsController 1.4.2, CommandAliases 1.0.1

---

## 1. Architecture Overview

Система построена вокруг JSON-конфигов привилегий. Каждая привилегия (VipUnit) состоит из двух частей:

- **`Access`** — массив условий (Limits), определяющих, подходит ли игрок
- **`Modules`** — массив фич (Modules), которые игрок получает

Три расширяемых реестра типов:

| Реестр | Что регистрирует | Форвард для регистрации |
|--------|-----------------|------------------------|
| **Module types** | Именованные игровые фичи | `VipM_Modules_OnInited()` |
| **Limit types** | Булевы условия доступа | `VipM_Limits_OnInited()` |
| **Item types** | Эффекты-предметы (ItemsController) | `IC_ItemType_OnInited()` |

Привилегии проверяются **сверху вниз** в [`Vips.json`](amxmodx/configs/plugins/VipModular/Vips.json). Если игрок **проходит** любой лимит Access — все модули этой привилегии мержатся в набор игрока. Одноимённые модули из нескольких привилегий мержатся через хук `Module_OnMergeParams` (если модуль зарегистрирован с `Once = false`). По умолчанию `Once = true` — первый совпавший модуль выигрывает.

---

## 2. Source Code Layout

```
amxmodx/scripting/
├── VipModular.sma                         ← Ядро: инициализация, загрузка конфигов, жизненный цикл
├── ItemsController.sma                    ← Фреймворк эффектов-предметов (независимый от ядра)
├── VipM-Misc.sma                          ← Вспомогательный плагин (релоад, спавн/раунд триггеры)
├── VipM-ModulesLimiter.sma                ← Покартное включение/отключение модулей
│
├── VipM-M-WeaponMenu.sma                  ← Модуль: меню выбора оружия
├── VipM-M-SpawnItems.sma                  ← Модуль: выдача предметов при спавне
├── VipM-M-SpawnHealth.sma                 ← Модуль: установка здоровья/брони при спавне
├── VipM-M-Vampire.sma                     ← Модуль: вампиризм (хил за убийства)
├── VipM-M-VipInTab.sma                    ← Модуль: метка VIP в таблице счёта
│
├── include/
│   ├── VipModular.inc                     ← Публичное API (главный инклуд)
│   ├── VipM/
│   │   ├── Modules.inc                    ← API системы модулей (нативы, форварды, события)
│   │   ├── Limits.inc                     ← API системы лимитов
│   │   ├── Params.inc                     ← Устаревшие хелперы (используйте PCGet_*)
│   │   ├── L/
│   │   │   └── Counter.inc               ← API счётчиков (нативы, енум, хелперы)
│   │   ├── ItemsController.inc           ← Тонкая обёртка-редирект на <ItemsController>
│   │   └── M/
│   │       └── WeaponMenu.inc            ← Native API модуля WeaponMenu
│   ├── ItemsController.inc               ← API системы предметов
│
├── VipM/                                   ← Исходники ядра (не инклудятся, см. #include link)
│   ├── Core/                               ← Загрузка конфигов, управление привилегиями, серверные команды
│   ├── DefaultObjects/                     ← Встроенные лимиты, типы параметров, их регистратор
│   ├── Forwards.inc                        ← Тонкая обёртка над CreateMultiForward / ExecuteForward
│   ├── ArrayMap.inc                        ← Ассоциативный массив (Array + Trie)
│   ├── ArrayTrieUtils.inc                 ← Макросы итерации и безопасные врапперы
│   ├── Utils.inc                          ← Битовые операции, CallOnce, JSON-хелперы, языковые макросы
│   ├── DebugMode.inc                      ← Дебаг-логирование
│   └── WeaponMenu/                        ← Подсистема меню оружия (логика меню, нативы, счётчики)
│
└── ItemsController/                        ← Исходники ItemsController (не инклудятся)
    ├── Objects/Items/                      ← Реестр типов предметов и управление инстансами
    ├── API/                                ← Реализация нативов регистрации/чтения/выдачи
    └── DefaultObjects/ItemType/            ← 16 встроенных типов предметов
```

---

## 3. Config File Format

**Корневая директория конфигов:** `amxmodx/configs/plugins/VipModular/`

- Пути с префиксом `File:` разрешаются относительно этой папки
- Пути, начинающиеся с `/`, разрешаются относительно `amxmodx/configs/`

**Основные конфиги:**
- [`Vips.json`](amxmodx/configs/plugins/VipModular/Vips.json) — массив привилегий. Каждая содержит `Access` (лимиты) и `Modules` (модули). Дополнительные файлы загружаются из `Vips/` (все `*.json`).
- [`Modules.json`](amxmodx/configs/plugins/VipModular/Modules.json) — конфиг `VipM-ModulesLimiter` для покартного включения/отключения модулей.
- `configs/plugins/VipModular/WeaponMenu/*.json` — конфиги меню оружия.

**Параметры модулей/лимитов** определяются через `PCParam()` макросы из библиотеки ParamsController. Типы параметров:
- `Integer`, `Float`, `Bool`, `String` — примитивные
- `VipM-Limit` / `VipM-Limits` — ссылка на один или список лимитов
- `VipM-LimitType` / `VipM-ModuleType` — ссылка на тип лимита/модуля
- `IC-Item` / `IC-Items` — один предмет или список предметов
- `VipM-L-CounterType` — тип сброса счётчика (PerLife/PerRound/PerSession/PerGame/PerMap)

---

## 4. Core Data Flow (Lifecycle)

### Инициализация (`plugin_precache`)

1. **Регистрация** — `register_plugin`, `register_library`, инициализация `ParamsController`
2. **Инициализация реестров** — создаются ArrayMap для типов лимитов, модулей, подсистема форвардов
3. **Форварды инициализации** — `VipM_Limits_OnInited()`, `VipM_Modules_OnInited()`, `IC_ItemType_OnInited()` — плагины регистрируют свои типы
4. **Загрузка конфигов** — `VipM/Core/VipsManager.inc` парсит `Vips.json` и папку `Vips/`, создаёт объекты привилегий (VipUnit), отмечает используемые типы модулей
5. **Активация модулей** — `ModuleType_ActivateUsed()` вызывает `VipM_Modules_OnActivate()` (тут `VipM-ModulesLimiter` может заблокировать модуль для карты) и `Module_OnActivated` для каждого модуля

### Подключение игрока

- `client_authorized` — устанавливаются статические лимиты (Steam, SteamId, IP, Bot)
- `client_putinserver` — устанавливаются состояния Alive, Counter; через `RequestFrame` запускается `VipsManager_UserReload`
- **VipsManager_UserReload**: перебирает все VipUnit, проверяет Access лимиты через `VipUnit_CheckUserAccess`, при прохождении сохраняет параметры модуля в `g_tUserModules[playerIndex]`. Если модуль уже был — мержит (`Module_OnMergeParams`). В конце вызывает `VipM_OnUserUpdated()`, где модули могут кешировать параметры
- `client_disconnected` — `VipsManager_UserReset()` очищает `g_tUserModules[playerIndex]`

Детали реализации каждого шага — в исходниках `VipM/Core/` и `VipM/Core/Objects/`.

---

## 5. Module System

Модуль — это именованная игровая фича. Плагин регистрирует тип модуля, ядро активирует его, если он упомянут в любом VIP-конфиге, после чего модуль вешает хуки на игровые события.

**Жизненный цикл модуля:**
1. Регистрация в `VipM_Modules_OnInited()` через `VipM_Modules_Register()`, указание параметров через `VipM_Modules_AddParamsEx()`, подписка на события (`Module_OnActivated`, `Module_OnRead`, `Module_OnMergeParams`)
2. Активация — в `Module_OnActivated` модуль вешает хуки
3. Во время игры — проверяет `VipM_Modules_HasModule(MODULE_NAME, playerIndex)`, получает параметры через `VipM_Modules_GetParams()` и применяет эффект

**Параметр `Once`:** если `true` (по умолчанию) — первый VIP, давший модуль, выигрывает. Если `false` — при совпадении вызывается `Module_OnMergeParams`, который может объединить параметры.

**Нейминг:**
- Файл плагина: `VipM-M-ModuleName.sma`
- Константа имени: `new const MODULE_NAME[] = "ModuleName"`
- Имя плагина: `[VipM-M] ModuleName`

Детали API — в [`include/VipM/Modules.inc`](amxmodx/scripting/include/VipM/Modules.inc).  
Пример реализации — [`VipM-M-VipInTab.sma`](amxmodx/scripting/VipM-M-VipInTab.sma).

---

## 6. Limits System

Лимит — булево условие, определяющее, имеет ли игрок доступ к привилегии.

**Два вида:**

| Вид | Параметры | Колбэки | Применение |
|-----|-----------|---------|------------|
| **Dynamic** | Есть | `Limit_OnRead`, `Limit_OnCheck` | Flags, Map, Time, Frags... |
| **Static** | Нет | Нет | Быстрые флаги на игрока (Steam, Alive, Bot) |

Статические лимиты хранят результат в битмаске на игрока — никакой форвард при проверке не вызывается.

**API регистрации:** в `VipM_Limits_OnInited()` вызвать `VipM_Limits_RegisterType()`, добавить параметры через `VipM_Limits_AddParamsEx()`, подписаться на `Limit_OnCheck`.

**Выполнение:** `VipM_Limits_ExecuteList(limits, playerIndex, E_LimitsExecType)` — поддерживает `OR`, `AND`, `XOR`. По умолчанию лимиты в Access выполняются через OR.

**Система счётчиков** (Counter limit): позволяет ограничить количество использований. API в [`include/VipM/L/Counter.inc`](amxmodx/scripting/include/VipM/L/Counter.inc) — `VipM_L_Counter_Get/Set/Inc` с типами сброса PerLife/PerRound/PerSession/PerGame/PerMap.

**Встроенные лимиты (24 типа):** Always, Never, Alive, Bot, Steam, Flags, SteamId, Ip, Map, Time, RoundTime, Round, GameTime, Frags, WasKilled, InBuyZone, InFreezyTime, HasPrimaryWeapon, LifeTime, WeekDay, Name, Counter, OncePer, Logic. Реализации — в `VipM/DefaultObjects/Limit/`.

Детали API — в [`include/VipM/Limits.inc`](amxmodx/scripting/include/VipM/Limits.inc).

---

## 7. ItemsController System

**ItemsController** — независимый фреймворк для применения эффектов ("предметов") к игрокам. Используется модулями SpawnItems, Vampire, WeaponMenu.

**Основные понятия:**
- **Item Type** (`T_IC_ItemType`) — класс эффекта, регистрируется плагином
- **Item Instance** (`T_IC_Item`) — конкретный предмет с распарсенными параметрами

**Жизненный цикл:** JSON → `IC_Item_ReadFromJson()` → `IC_Item_Give(playerIndex, item)` → `ItemType_OnGive` → `IC_Item_Free()`

**Регистрация типа предмета:** через `IC_ItemType_SimpleRegister()` или `IC_ItemType_Register()` + `IC_ItemType_SetEventListener()`. Параметры добавляются через `IC_ItemType_AddParams()`.

**Использование в модуле:** в параметрах модуля указать `PCParam("Items", IC_PARAM_TYPE_ITEMS_NAME)`, в хендлере читать через `PCGet_IcItemsGive(p, "Items", playerIndex)`. **Важно:** перед регистрацией таких параметров вызвать `IC_Init()`.

**Встроенные типы (16):** Weapon, Health, Armor, DefuseKit, Money, Speed, DamageMult, InstantReload, InstantReloadAllWeapons, RefillBpAmmo, Command, Function, If, ItemsList, Random, CustomWeapon.

Детали API — в [`include/ItemsController.inc`](amxmodx/scripting/include/ItemsController.inc).

---

## 8. WeaponMenu System

Модуль [`VipM-M-WeaponMenu.sma`](amxmodx/scripting/VipM-M-WeaponMenu.sma) предоставляет меню выбора оружия.

**Фичи:**
- Конфиги списков оружия в JSON (директория `configs/plugins/VipModular/WeaponMenu/`)
- Лимиты доступа к конкретному оружию
- Авто-открытие при спавне
- Отображение статуса истечения через `VipM_WeaponMenu_SetExpireStatus()`
- Команды: `say /weapons`, `say /ws`, `say /w` и авто-открытие тоггл

Детали API — в [`include/VipM/M/WeaponMenu.inc`](amxmodx/scripting/include/VipM/M/WeaponMenu.inc).  
Исходники подсистемы — в [`VipM/WeaponMenu/`](amxmodx/scripting/VipM/WeaponMenu/).

---

## 9. Forward System

Проект использует тонкую обёртку над AMXX `CreateMultiForward` / `ExecuteForward`, определённую в [`VipM/Forwards.inc`](amxmodx/scripting/VipM/Forwards.inc).

**Два режима:**
- **Registered** — форвард живёт постоянно, используется для событий, которые могут сработать много раз (например, `VipM_OnUserUpdated`)
- **RegAndCall** — форвард создаётся, выполняется и сразу уничтожается; используется для однократных хуков инициализации (`VipM_Modules_OnInited`, `VipM_Limits_OnInited`, `VipM_OnLoaded`, `IC_ItemType_OnInited`)

**Полный список форвардов** — в [`VipM/Core/SrvCmds.inc`](amxmodx/scripting/VipM/Core/SrvCmds.inc) (команда `vipm_info`).

---

## 10. Public API Summary

Все нативы и форварды объявлены в .inc файлах директории `include/`. **Не копируйте сигнатуры в документацию** — актуальная информация всегда в исходниках:

| Файл | Содержит |
|------|----------|
| [`include/VipModular.inc`](amxmodx/scripting/include/VipModular.inc) | `VipM_UserUpdate()`, `VipM_OnLoaded()`, `VipM_OnUserUpdated()` |
| [`include/VipM/Modules.inc`](amxmodx/scripting/include/VipM/Modules.inc) | API модулей: регистрация, параметры, события, проверка наличия |
| [`include/VipM/Limits.inc`](amxmodx/scripting/include/VipM/Limits.inc) | API лимитов: регистрация типов, параметры, выполнение |
| [`include/VipM/L/Counter.inc`](amxmodx/scripting/include/VipM/L/Counter.inc) | API счётчиков: Get/Set/Inc |
| [`include/ItemsController.inc`](amxmodx/scripting/include/ItemsController.inc) | API предметов: регистрация, чтение, выдача |
| [`include/VipM/M/WeaponMenu.inc`](amxmodx/scripting/include/VipM/M/WeaponMenu.inc) | API WeaponMenu: SetExpireStatus |
| [`include/VipM/Params.inc`](amxmodx/scripting/include/VipM/Params.inc) | (Deprecated) Устаревшие хелперы, используйте `PCGet_*` |

---

## 11. Code Conventions

### Нейминг

| Вид | Соглашение | Пример |
|-----|-----------|--------|
| Локальные переменные | `camelCase` | `playerIndex`, `menuIndex` |
| Глобальные переменные | `PascalCase` | `UserAutoOpen`, `Vips` |
| Публичные функции | `Namespace_PascalCase` | `VipM_Limits_RegisterType` |
| Приватные функции | `DefaultObjects_*` + `static` | `static DefaultObjects_Limit_Map_GetCurrentName(...)` |
| Внутренние хелперы | `_PascalCase` или `@PascalCase` | `_Cmd_Menu`, `@OnModuleActivate` |
| Enum-handle types | `T_Name` | `T_ModuleType`, `T_IC_Item` |
| Enum struct layouts | `S_Name` | `S_ModuleType`, `S_WeaponMenu` |
| Enum поля | `StructName_FieldName` | `ModuleType_Name`, `VipUnit_Access` |
| Константы и макросы | `SCREAMING_SNAKE_CASE` | `MODULE_NAME`, `VIPM_MODULES_TYPE_NAME_MAX_LEN` |
| Enum (перечисления) | `E_Name` | `E_ModuleEvent`, `E_LimitsExecType` |

**Без венгерской нотации:** никаких `i`, `s`, `f`, `b`, `g`, `g_` префиксов.

### Форматирование

- Отступ — 4 пробела (без табуляции)
- Открывающая скобка на той же строке: `if (x) {`
- Одна пустая строка между логическими блоками; **две** между функциями верхнего уровня
- Длинные аргументы — один на строку с выравниванием по открывающей скобке
- Многострочные условия — каждый под-уcловие на своей строке, оператор **в начале**

### Соглашения

- Использовать `PCGet_*` / `PCSingle_*` хелперы из ParamsController, не читать Trie напрямую
- Предпочитать `Invalid_*` сентинелы вместо `-1`
- `CallOnce()` макрос для функций инициализации
- Вся инициализация — в `plugin_precache`
- Регистрация модулей/лимитов — только в соответствующих `OnInited` форвардах

### Файловая структура

- Модули: `VipM-M-ModuleName.sma`
- Лимиты: `VipM/DefaultObjects/Limit/LimitName.inc`
- Типы предметов: `ItemsController/DefaultObjects/ItemType/TypeName.inc`
- Регистрация встроенных лимитов/параметров: `VipM/DefaultObjects/Registrar.inc`

---

## 12. Creating a New Extension

### A. Новый модуль

1. Создать `amxmodx/scripting/VipM-M-YourModule.sma`
2. Зарегистрировать в `VipM_Modules_OnInited()` — `VipM_Modules_Register()`, `VipM_Modules_AddParamsEx()`, `VipM_Modules_RegisterEvent()`
3. Модуль активируется автоматически при упоминании в любом VIP
4. API модулей — в [`include/VipM/Modules.inc`](amxmodx/scripting/include/VipM/Modules.inc)

### B. Новый тип лимита

1. Создать `amxmodx/scripting/VipM/DefaultObjects/Limit/YourLimit.inc`
2. Определить `DefaultObjects_Limit_YourLimit_Register()`, использовать `VipM_Limits_RegisterType()`, `VipM_Limits_AddParamsEx()`, `VipM_Limits_RegisterTypeEvent()`
3. Добавить `#include` и вызов в [`VipM/DefaultObjects/Registrar.inc`](amxmodx/scripting/VipM/DefaultObjects/Registrar.inc)
4. API лимитов — в [`include/VipM/Limits.inc`](amxmodx/scripting/include/VipM/Limits.inc)

### C. Новый тип предмета (ItemsController)

1. Создать `amxmodx/scripting/ItemsController/DefaultObjects/ItemType/YourType.inc`
2. Определить `DefaultObjects_ItemType_YourType_Register()`, использовать `IC_ItemType_SimpleRegister()` или `IC_ItemType_Register()` + `IC_ItemType_SetEventListener()`
3. Зарегистрировать в `ItemsController.sma` (смотреть как регистрируются другие типы)
4. API предметов — в [`include/ItemsController.inc`](amxmodx/scripting/include/ItemsController.inc)

### D. Новый формат конфига

Если добавляются новые JSON-ключи или меняется мерж конфигов — обновить описание формата.

---

## 13. Build System

Определён в [`amxbuild.yml`](amxbuild.yml) в корне проекта. Сборка через GitHub Actions CI или `amxx-builder`. Зависимости: ParamsController 1.4.2, CommandAliases 1.0.1, ReAPI 5.29.0.358.

---

## 14. Internal Data Structures

Детали реализации — в исходниках:

- **ArrayMap** (ассоциативный массив Array+Trie) — [`VipM/ArrayMap.inc`](amxmodx/scripting/VipM/ArrayMap.inc). Используется для реестров типов модулей и лимитов.
- **g_tUserModules[playerIndex]** — пер-игроковый `Trie<moduleName → Trie<params>>`, создаётся в `VipsManager_UserReload()`, очищается в `VipsManager_UserReset()`.
- **Handle types** (`T_VipUnit`, `T_ModuleType`, `T_LimitType`, `T_IC_Item`, etc.) — все являются индексами массива, кастованными к enum-типу. Сентинел `Invalid_*` всегда `-1`.

---

## 15. Server Commands

| Команда | Описание |
|---------|----------|
| `vipm_update_users` | Обновить привилегии всех игроков |
| `vipm_info` | Информация о системе (модули, лимиты, версии) |
| `vipm_modules` | Таблица зарегистрированных модулей и их статусов |
| `vipm_limits` | Таблица зарегистрированных типов лимитов |
| `ic_item_types` | Таблица зарегистрированных типов предметов |

Исходники — [`VipM/Core/SrvCmds.inc`](amxmodx/scripting/VipM/Core/SrvCmds.inc).

---

## 16. Debugging

В [`VipM/DebugMode.inc`](amxmodx/scripting/VipM/DebugMode.inc) определён макрос `Dbg_Log(...)` (условное логирование) и `Dbg_PrintServer(...)`. Включается дефайном при компиляции.

---

## 17. Documentation Maintenance Rules

После каждого изменения кода:

- Новый модуль → обновить этот файл (секция 5 или 12)
- Новый/изменённый тип лимита → обновить секцию 6
- Новый/изменённый тип предмета → обновить секцию 7
- Изменение формата конфигов → обновить секцию 3
- Структурные изменения → обновить секцию 2
- **Не копируйте сигнатуры нативов в AGENTS.md** — читайте .inc файлы напрямую

---

## 18. Dependency Versions

| Library | Min | Current | Docs |
|---------|-----|---------|------|
| ParamsController | 1.3.2 | 1.4.2 | [GitHub](https://github.com/AmxxModularEcosystem/ParamsController) |
| CommandAliases | 1.0.1 | 1.0.1 | [GitHub](https://github.com/AmxxModularEcosystem/CommandAliases) |
| ReAPI | 5.24.0.300 | 5.29.0.358 | [GitHub](https://github.com/rehlds/ReAPI) |
| AMXX | 1.10 | 1.10.5428 | — |
