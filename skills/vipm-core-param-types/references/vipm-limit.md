# VipM-Limit

Один лимит (условие доступа): JSON-объект `{ "Limit": "<LimitType>", ...params }`
или ссылка на файл с таким объектом.

- **Константа:** `VIPM_PARAM_TYPE_LIMIT_NAME` (`"VipM-Limit"`) —
  `amxmodx/scripting/include/VipM/Limits.inc`.
- **Регистрация:** `amxmodx/scripting/VipM/DefaultObjects/ParamType/Limit.inc` —
  `ParamsController_RegSimpleType(VIPM_PARAM_TYPE_LIMIT_NAME, "@DefaultObjects_ParamType_Limit_OnRead")`.

> Ссылка `"File:<path>"` — общий блок `_file-link.md`.

## Допустимые значения в JSON

- **object** — объект лимита. Поле `"Limit"` обязательно и задаёт имя типа
  лимита; остальные поля — параметры этого типа.
- **string** — `"File:<path>"`: ссылка на JSON-файл с **одним** объектом лимита.
- Массив (`[...]`) не допускается — для списка используйте `VipM-Limits`.

```jsonc
{ "Limit": "Round", "Min": 2 }

{ "Limit": "Flags", "Flags": "t" }

"File:Limits/MinRound-2"
```

## Теги

Тег не поддерживается: колбек `@DefaultObjects_ParamType_Limit_OnRead` объявлен
с одним аргументом (`const JSON:valueJson`) и не получает и не обрабатывает
тег. Запись `VipM-Limit:<tag>` синтаксически принимается контроллером, но тег
игнорируется.

## Что получится в Trie

Под ключом — ячейка с `T_LimitUnit`
(`enum T_LimitUnit {Invalid_LimitUnit = -1}`). Читается:

- `PCGet_VipmLimit(p, key, def = Invalid_LimitUnit)` — из Trie;
- `PCGet_VipmLimitCheck(p, key, playerIndex = 0, def = true)` — сразу выполняет
  лимит; если лимита нет, возвращает `def`;
- `PCSingle_ObjVipmLimit(objectJson, key, def = Invalid_LimitUnit, dotNot = false, orFail = false)`
  — одиночное значение из JSON-объекта.

Все объявлены в `include/VipM/Limits.inc`.

## Когда будет ошибка

- Значение не объект и не строка `File:` (массив, число, `true`, простая
  строка) — аварийное завершение `Json value must be an object.`.
- В объекте нет поля `"Limit"` — аварийное завершение
  `Object must contain 'Limit' field.`.
- Поле `"Limit"` пустое или указывает на незарегистрированный тип — сначала
  WARNING `Limit type name not specified.` / `Limit type '%s' not found.`, затем
  аварийное завершение
  `Field 'Limit' has invalid value for 'VipM-LimitType' param type.`
  (чтение `"Limit"` идёт с `orFail = true`).
- У динамического типа отсутствует обязательный параметр или неверно значение
  параметра — аварийное завершение `Param '%s' required for '%s' limit.` /
  `Param '%s' has invalid value.`.

## Что преобразуется молча

- Для **статического** типа лимита параметры из JSON не читаются и молча
  игнорируются (у статических типов параметров нет).

## Побочные эффекты

- При первом чтении вызывается `LimitUnit_Init()` (идемпотентно) — инициализирует
  реестр типов лимитов.
- Для `"File:<path>"` прочитанный `T_LimitUnit` кэшируется по пути ссылки:
  повторная ссылка на тот же файл возвращает тот же юнит.

## Ограничения

- Ссылка `File:` должна вести на **один объект**; файл-массив не
  разворачивается в список (см. `_file-link.md`).

## Примеры

```jsonc
// Динамический лимит с параметрами
{ "Limit": "Round", "Min": 4 }

// Статический лимит (без параметров)
{ "Limit": "Steam" }

// Переиспользование через ссылку
"File:Limits/MinRound-2"
```

```pawn
if (PCGet_VipmLimitCheck(p, "Access", playerIndex)) {
    // условие выполнено
}
```
