# VipM-ModuleType

Имя зарегистрированного типа модуля (строкой).

- **Константа:** `VIPM_PARAM_TYPE_MODULE_TYPE_NAME` (`"VipM-ModuleType"`) —
  `amxmodx/scripting/include/VipM/Modules.inc`.
- **Регистрация:** `amxmodx/scripting/VipM/DefaultObjects/ParamType/ModuleType.inc` —
  `ParamsController_RegSimpleType(VIPM_PARAM_TYPE_MODULE_TYPE_NAME, "@DefaultObjects_ParamType_ModuleType_OnRead")`.

## Допустимые значения в JSON

Строка — имя типа модуля, зарегистрированного через `VipM_Modules_Register`
(например `"VipInTab"`, `"WeaponMenu"`, `"SpawnItems"`).

```json
"VipInTab"
```

## Теги

Тег не поддерживается: колбек `@DefaultObjects_ParamType_ModuleType_OnRead`
объявлен с одним аргументом (`const JSON:valueJson`) и не получает и не
обрабатывает тег. Запись `VipM-ModuleType:<tag>` синтаксически принимается
контроллером, но тег игнорируется.

## Что получится в Trie

Под ключом — ячейка с `T_ModuleType`
(`enum T_ModuleType {Invalid_ModuleType = -1}`). Читается:

- `PCSingle_ObjVipmModuleType(objectJson, key, def = Invalid_ModuleType, dotNot = false, orFail = false)`
  — одиночное значение из JSON-объекта (`VipM/Core/Objects/Modules/Type.inc`);
- из Trie — `PCGet_Cell` (специального `PCGet_*` геттера нет).

## Когда будет ошибка

- Имя пустое после `trim()` — WARNING «Module type name not specified.»;
- тип не найден в реестре — WARNING «Module type '%s' not found.»;
- нестроковое значение не даёт валидного имени: чтение через `ShortString`
  (`json_get_string`) не извлекает текст, и срабатывает одна из проверок выше.

На уровне параметра колбек возвращает `false`; что делать дальше (`orFail`,
обязательность) решает ParamsController — см. `params-usage`.

## Что преобразуется молча

- Ведущие и хвостовые пробелы обрезаются (`trim()`).
- Имя длиннее `VIPM_MODULES_TYPE_NAME_MAX_LEN` (64 ячейки) обрезается при чтении.

## Побочные эффекты

- При первом чтении вызывается `ModuleType_Init()` (идемпотентно).
- Поиск имени в реестре **регистрозависимый** (сопоставление по ключу Trie):
  `"vipintab"` и `"VipInTab"` — разные значения.

## Примеры

```jsonc
{ "Module": "VipInTab" }
```

```pawn
new T_ModuleType:type = PCSingle_ObjVipmModuleType(objectJson, "Module", .orFail = true);
new T_ModuleType:fromTrie = T_ModuleType:PCGet_Cell(params, "Module", Invalid_ModuleType);
```
