---
name: vipm-config-common
description: >-
  Author and edit the shared VipModular configuration tree for the AMXX
  VIP/privilege plugin: the Vips.json privilege array (Access limits, Modules),
  File: references, per-map module activation (Modules.json) and the config
  folder layout. Use when adding or editing a VIP privilege, granting modules to
  a group, writing Access conditions, splitting configs into files, or fixing
  Vips.json / Modules.json. Triggers: VipModular config, Vips.json, Modules.json,
  Access, Modules, File: reference, VIP privilege, privilege config, конфиг
  VipModular, привилегия, настройка VIP, права VIP.
---

# VipModular — Общие конфиги (shared configs)

## Когда применять / When to use

- Нужно добавить, изменить или удалить VIP-привилегию; выдать набор модулей
  группе; описать условия доступа (`Access`); разбить конфиг на файлы; поправить
  глобальную активацию модулей (`Modules.json`).
- **НЕ для этого скилла:** тело лимита → `vipm-config-limits`; тело модуля →
  `vipm-config-modules`; предмет → `vipm-config-items`; поиск имён типов/параметров в
  исходниках → `vipm-config-type-lookup`.

## Расположение и правила

```
amxmodx/configs/plugins/VipModular/
├── Vips.json              ← главный файл привилегий
├── Vips/                  ← (опционально) дополнительные *.json с привилегиями
├── Modules.json           ← правила активации модулей (плагин VipM-ModulesLimiter)
├── Limits/                ← переиспользуемые лимиты (напр. MinRound-2.json)
├── Items/                 ← переиспользуемые предметы (напр. DefuseKit.json)
├── WeaponMenu/ …          ← конфиги модулей (см. vipm-config-modules)
├── SpawnItems/ …
├── SpawnHealth/ …
├── Vampire/ …
└── VipInTab/ …
```

- Формат — **JSON с комментариями** (`// …` и `/* … */`). Висячие запятые
  запрещены.
- **Три дискриминатора объектов** (именно так, НЕ `"Type"`):
  | Сущность | Ключ типа | Пример |
  |---|---|---|
  | Лимит (проверка) | `"Limit"` | `{ "Limit": "Flags", "Flags": "t" }` |
  | Модуль | `"Module"` | `{ "Module": "VipInTab", "Enabled": true }` |
  | Предмет | `"Item"` | `{ "Item": "Weapon", "Name": "weapon_ak47" }` |

> ⚠️ Файл `docs/agents/config-format.md` в этом репозитории **недостоверен**:
> он использует несуществующий ключ `"Type"` и выдуманные параметры. Всегда
> сверяйтесь с этим скиллом, `readme/configs.md` и исходниками.

## Ссылки на файлы: `"File:<path>/<file>"`

- `<path>` — путь относительно `amxmodx/configs/plugins/VipModular/`;
  `<file>` — имя `.json` **без расширения**.
- `"File:/Items/DefuseKit"` (ведущий `/`) — путь относительно
  `amxmodx/configs/`.
- Повторная ссылка на тот же файл не читает файл заново — подставляется уже
  прочитанный объект (кэш).
- Ссылкой может быть как весь объект, так и **элемент массива**:
  ```jsonc
  "Modules": [ "File:WeaponMenu/Admin", { "Module": "VipInTab", "Enabled": true } ]
  ```
- Ссылаться можно на любой JSON-объект: модуль, лимит, предмет, меню.

## Главный файл `Vips.json`

Корень — **массив** объектов `{ "Access": …, "Modules": … }`:

```jsonc
[
    {
        "Access": [
            { "Limit": "Flags", "Flags": "l" }
        ],
        "Modules": [
            "File:WeaponMenu/Admin",
            "File:SpawnItems/Premium",
            "File:SpawnHealth/Default"
        ]
    },
    {
        "Access": [ { "Limit": "ForAll" } ],
        "Modules": [ "File:SpawnItems/Vip" ]
    }
]
```

- Поле `Access` — один или более лимитов. Если лимит один, допускается объект
  вместо массива. **По умолчанию список выполняется через OR** — достаточно
  любого совпадения. Чтобы потребовать все условия, оберните их в
  `{ "Limit": "Logic-AND", "Limits": [ … ] }` (см. `vipm-config-limits`).
- Поле `Modules` — один или более модулей (объект или `"File:…"`). Если модуль
  один, допускается объект вместо массива.
- `Access` и `Modules` целиком тоже могут быть ссылкой `"File:…"`.

### Порядок и приоритет

- Привилегии проверяются **сверху вниз**.
- Как только игрок проходит `Access`, модули привилегии добавляются к его
  набору.
- При конфликте одноимённых модулей **по умолчанию выигрывает первый**
  (`Once = true`). Модули, зарегистрированные с `Once = false`, объединяют
  параметры через хук `Module_OnMergeParams`.
- Поэтому порядок — от «сильной» привилегии к «слабой»:
  Админ → Premium → VIP → Steam → Для всех.

### Папка `Vips/`

Любые `*.json` в `amxmodx/configs/plugins/VipModular/Vips/` загружаются как
дополнительные привилегии с той же структурой. `Vips.json` читается первым;
порядок файлов внутри `Vips/` не гарантируется.

## `Modules.json` — активация модулей (VipM-ModulesLimiter)

```jsonc
[
    {
        "Modules": [ "SpawnItems", "WeaponMenu", "SpawnHealth", "Vampire" ],
        "Limits": {
            "Limit": "Logic-NOT",
            "Limits": { "Limit": "Map", "Regexp": "^(\\$|aim_|awp_|fy_)", "Real": true }
        }
    }
]
```

- Корень — массив объектов `{ "Modules": [ … ], "Limits": … }`.
- `Modules` — имена модулей, к которым применяется правило.
- `Limits` — один лимит **или** массив. Достаточно выполнения **хотя бы
  одного** (OR).
- Модуль, **не упомянутый** в файле, активен всегда. Активируются только
  модули, реально используемые в привилегиях.
- Проверка выполняется при старте карты. Здесь **нельзя** использовать
  зависящие от игрока лимиты (например `Flags`, `Alive`).
- Отключите плагин `VipM-ModulesLimiter`, чтобы модули всегда были активны.

## Типы параметров (ParamsController)

Начиная с VipModular **5.0.0-rc.\*** система использует **ParamsController**, где
список типов параметров **расширяемый**: тип — это строковое имя, которое
регистрирует библиотека или плагин-расширение. Старый enum `E_ParamType`
(`ptInteger`, `ptBoolean`, `ptLimit`, `ptCustom`, …) **устарел** — не используйте
его в новых конфигах/плагинах.

Встроенные имена типов (ParamsController):

| Имя | Значение |
|---|---|
| `Integer` | целое число |
| `Float` | дробное число |
| `Boolean` | `true`/`false` |
| `String` | строка |
| `ShortString` / `LongString` | короткая / длинная строка |
| `RGB` | цвет |
| `Model` / `PlayerModel` | модель |
| `Sound` / `Resource` | звук / ресурс |
| `File` / `Dir` | файл / путь |
| `ChatMessage` | сообщение в чат |
| `Time` / `TimeInterval` | время / интервал |
| `WeekDay` | день недели |
| `Flags` | флаги |
| `Regexp` | регулярное выражение |

Дополнительные типы, регистрируемые VipModular / ItemsController:

| Имя | Значение |
|---|---|
| `VipM-Limit` / `VipM-Limits` | один лимит / список лимитов |
| `VipM-LimitType` / `VipM-ModuleType` | ссылка на тип лимита / модуля |
| `VipM-L-CounterType` | тип сброса счётчика (`PerLife`/`PerRound`/…) |
| `IC-Item` / `IC-Items` | один предмет / список предметов |

Плагины-расширения могут добавлять собственные типы — список не закрытый.
Актуальные имена всегда можно найти в исходниках (`vipm-config-type-lookup`).

## Рабочий процесс / Workflow

1. Определите цель: какая группа игроков и что должна получать.
2. Выберите место: новая привилегия в `Vips.json` (или файл в `Vips/`),
   переиспользуемые тела — в `Limits/`, `Items/`, папках модулей.
3. Для каждого модуля уточните имя и параметры через скилл `vipm-config-type-lookup`
   и `vipm-config-modules`.
4. Для условий доступа уточните имя и параметры лимита через `vipm-config-limits`.
5. Соберите JSON по схемам выше; используйте `"File:…"` для переиспользования.
6. Проверьте: JSON валиден (нет висячих запятых), имена типов/параметров
   совпадают с зарегистрированными, порядок привилегий корректен.
7. На сервере примените изменения: команда `vipm_update_users` (обновить
   привилегии игроков) или релоад конфигов через `VipM-Misc`.
8. Диагностика: `vipm_info`, `vipm_modules`, `vipm_limits`, `ic_item_types`.

## Частые ошибки / Gotchas

- Ключ `"Type"` вместо `"Limit"` / `"Module"` / `"Item"` — частая ошибка из
  старых доков. Валиден только профильный ключ для каждой сущности.
- `File:` указывается **без** `.json` и без ведущего слэша (если путь не от
  `configs/`).
- Забыли, что `Access` — это OR. Нужен AND → `Logic-AND`.
- Модуль указан не в порядке приоритета — незаметно перекрывается «сильным».
- `Modules.json` использует `Modules` + `Limits` (нет `Enable`/`Disable`).
- Висячие запятые и неэкранированные `\` в regex (в JSON строках двойной
  обратный слэш: `"^fy_"` → `"^(\\$|fy_)"`).
- Не заявляйте тип/параметр, которого нет в исходниках — сначала проверьте.

## Чек-лист перед завершением

- [ ] JSON парсится (комментарии допустимы, висячих запятых нет).
- [ ] Использованы правильные дискриминаторы (`Limit`/`Module`/`Item`).
- [ ] Все имена типов и параметры подтверждены исходниками (`vipm-config-type-lookup`).
- [ ] Порядок привилегий корректен, конфликтующие модули не перекрываются.
- [ ] `Access` использует OR по умолчанию; AND обёрнут в `Logic-AND`.
- [ ] `File:`-ссылки без расширения и ведут на существующие файлы.
- [ ] Указан способ применения (релоад / `vipm_update_users`).
