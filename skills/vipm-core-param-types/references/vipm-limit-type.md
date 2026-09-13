# VipM-LimitType

Имя зарегистрированного типа лимита (строкой).

- **Константа:** `VIPM_PARAM_TYPE_LIMIT_TYPE_NAME` (`"VipM-LimitType"`) —
  `amxmodx/scripting/include/VipM/Limits.inc`.
- **Регистрация:** `amxmodx/scripting/VipM/DefaultObjects/ParamType/LimitType.inc` —
  `ParamsController_RegSimpleType(VIPM_PARAM_TYPE_LIMIT_TYPE_NAME, "@DefaultObjects_ParamType_LimitType_OnRead")`.

## Допустимые значения в JSON

Строка — имя типа лимита, зарегистрированного через `VipM_Limits_RegisterType`
(например `"Round"`, `"Flags"`, `"Logic-AND"`).

```json
"Round"
```

## Теги

Тег не поддерживается: колбек `@DefaultObjects_ParamType_LimitType_OnRead`
объявлен с одним аргументом (`const JSON:valueJson`) и не получает и не
обрабатывает тег. Запись `VipM-LimitType:<tag>` синтаксически принимается
контроллером, но тег игнорируется.

## Что получится в Trie

Под ключом — ячейка с `T_LimitType` (`enum T_LimitType {Invalid_LimitType = -1}`).
Читается:

- `PCSingle_ObjVipmLimitType(objectJson, key, def = Invalid_LimitType, dotNot = false, orFail = false)`
  — одиночное значение из JSON-объекта (`VipM/Core/Objects/Limits/Type.inc`);
- из Trie — `PCGet_Cell` (специального `PCGet_*` геттера нет).

## Когда будет ошибка

- Имя пустое после `trim()` — WARNING «Limit type name not specified.»;
- тип не найден в реестре — WARNING «Limit type '%s' not found.»;
- нестроковое значение не даёт валидного имени: чтение через `ShortString`
  (`json_get_string`) не извлекает текст, и срабатывает одна из проверок выше.

На уровне параметра колбек возвращает `false`; что делать дальше (`orFail`,
обязательность) решает ParamsController — см. `params-usage`.

## Что преобразуется молча

- Ведущие и хвостовые пробелы обрезаются (`trim()`).
- Имя длиннее `VIPM_LIMITS_TYPE_NAME_MAX_LEN` (64 ячейки) обрезается при чтении.

## Побочные эффекты

- При первом чтении вызывается `LimitType_Init()` (идемпотентно).
- Поиск имени в реестре **регистрозависимый** (сопоставление по ключу Trie):
  `"round"` и `"Round"` — разные значения.

## Примеры

```jsonc
{ "Type": "Round" }
```

```pawn
new T_LimitType:type = PCSingle_ObjVipmLimitType(objectJson, "Type", .orFail = true);
new T_LimitType:fromTrie = T_LimitType:PCGet_Cell(params, "Type", Invalid_LimitType);
```
