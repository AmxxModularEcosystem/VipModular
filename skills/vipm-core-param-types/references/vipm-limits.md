# VipM-Limits

Список лимитов (условий доступа): массив объектов/ссылок, один объект или
ссылка на файл с одним объектом.

- **Константа:** `VIPM_PARAM_TYPE_LIMITS_NAME` (`"VipM-Limits"`) —
  `amxmodx/scripting/include/VipM/Limits.inc`.
- **Регистрация:** `amxmodx/scripting/VipM/DefaultObjects/ParamType/Limits.inc` —
  `ParamsController_RegSimpleType(VIPM_PARAM_TYPE_LIMITS_NAME, "@DefaultObjects_ParamType_Limits_OnRead")`.

> Ссылка `"File:<path>"` — общий блок `_file-link.md`.

## Допустимые значения в JSON

- **array** — массив элементов. Каждый элемент — объект лимита
  (`{ "Limit": ... }`) или строка `"File:<path>"` на один объект.
- **object** — один лимит; трактуется как список из одного элемента (без ошибки).
- **string** — `"File:<path>"`, указывающий на **один** объект лимита.

```jsonc
// array (элементы — объекты или File-ссылки)
[
    { "Limit": "Round", "Min": 4 },
    "File:Limits/NotOnAwp"
]
```

```jsonc
// один объект
{ "Limit": "Steam" }
```

```jsonc
// File-ссылка на один объект
"File:Limits/Menus/AwpAccess"
```

## Теги

Тег не поддерживается: колбек `@DefaultObjects_ParamType_Limits_OnRead`
объявлен с одним аргументом (`const JSON:valueJson`) и игнорирует тег.

## Что получится в Trie

Под ключом — ячейка с хендлером `Array` (динамический массив `T_LimitUnit`).
Читается:

- `PCGet_VipmLimits(p, key, def = Invalid_Array)` — из Trie;
- `PCGet_VipmLimitsCheck(p, key, playerIndex = 0, type = Limit_Exec_OR, def = true)`
  — сразу выполняет список; `type` — логический оператор `E_LimitsExecType`
  (`Limit_Exec_OR` / `Limit_Exec_AND` / `Limit_Exec_XOR`); если списка нет,
  возвращает `def`;
- `PCSingle_ObjVipmLimits(objectJson, key, def = Invalid_Array, dotNot = false, orFail = false)`
  — одиночное значение из JSON-объекта.

Все объявлены в `include/VipM/Limits.inc`.

## Когда будет ошибка

- Массив пуст (`[]`) — WARNING «Limits array is empty.»; параметр не читается.
- Не прочитано ни одного валидного лимита — WARNING «All limits are invalid.»;
  параметр не читается.
- Значение не массив, не объект и не `File:`-строка (число, `true` и т.п.) —
  аварийное завершение `Json value must be an object.`.
- `File:` указывает на **массив**, а не на один объект — попытка прочитать его
  как один лимит завершается аварийно (`Json value must be an object`);
  файл-массив по ссылке **не** разворачивается в список.
- Ошибка в элементе массива не пропускается: неизвестный тип лимита, отсутствие
  обязательного параметра или неверное значение параметра приводят к аварийному
  завершению.

## Что преобразуется молча

- Один объект вместо массива молча читается как список из одного элемента.

## Побочные эффекты

- `LimitUnit_Init()` при первом чтении (идемпотентно).
- `File:`-ссылки на отдельные лимиты кэшируются (см. `_file-link.md`).

## Ограничения

- Логический оператор по умолчанию при выполнении — OR; другой оператор
  передаётся явно в `PCGet_VipmLimitsCheck`.

## Примеры

```jsonc
// Массив: нужны оба условия (оператор AND задаётся при выполнении)
[
    { "Limit": "Round", "Min": 4 },
    { "Limit": "HasPrimaryWeapon", "HasNot": true }
]
```

```jsonc
// Одиночный лимит вместо массива (список из одного элемента)
{ "Limit": "Steam" }
```

```pawn
new bool:ok = PCGet_VipmLimitsCheck(p, "Limits", playerIndex, Limit_Exec_AND);
```
