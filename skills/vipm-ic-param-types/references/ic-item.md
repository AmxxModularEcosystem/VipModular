# IC-Item

Один экземпляр предмета ItemsController, прочитанный из JSON-объекта (`T_IC_Item`).

## Допустимые значения в JSON

Ожидается **объект** предмета:

```jsonc
{
    "Item": "<ItemType>",          // обязательно
    "<ParamKey>": "<ParamValue>"   // параметры конкретного типа предмета
}
```

- `<ItemType>` — имя зарегистрированного типа предмета (`Weapon`, `Health`,
  `DefuseKit`, `ItemsList`, …). Список и их параметры — в скилле
  `vipm-config-items` / командой `ic_item_types`.
- Легаси-поле `"Type"` принимается вместо `"Item"`, но при этом пишется WARNING:
  "Item object with field \`Type\` is deprecated, use \`Item\` field instead."
- Имя типа обрезается по `trim()`; пустое имя — ошибка.

Массив, число, `null` и голая строка предметом **не являются** (для массивов и
ссылок используйте `IC-Items`).

### Ссылки `"File:..."`

Ридер `IC_Item_ReadFromJson` сам ссылки не разворачивает: он сразу вызывает
`ItemInstance_ReadFromJsonObject`, который требует `json_is_object`. Это расхождение
с комментарием к нативу в `include/ItemsController.inc` («…содержащее предмет или
ссылку на него»), где поддержка ссылок заявлена.

При использовании как **типа параметра** контроллер перед вызовом колбека прогоняет
значение через `PCJson_HandleLinkedValue` (`ParamType_Read`), поэтому ссылка
разворачивается, если JSON-дерево обёрнуто (`PCJson_ParseFile`, как в конфигах
VipModular), — но развёрнутое значение всё равно обязано быть **объектом**. Ссылку на
файл-массив `IC-Item` не примет.

Практическое следствие: для ссылок на списки/массивы и для нескольких предметов
используйте `IC-Items`.

## Теги

Не поддерживаются. Регистрация — `ParamsController_RegSimpleType(IC_PARAM_TYPE_ITEM_NAME,
"@OnItemParamTypeRead")`, колбек `bool:@OnItemParamTypeRead(const JSON:valueJson)`
принимает только значение и игнорирует тег (четвёртый аргумент `ParamsController`).
`IC-Item:любойТег` обрабатывается как `IC-Item`.

## Что получится в Trie

- Под ключом — **cell**: хендлер экземпляра `T_IC_Item` (`Invalid_IC_Item = -1`).
- Чтение значения из `Trie`:
  ```pawn
  stock T_IC_Item:PCGet_IcItem(const Trie:p, const key[], const T_IC_Item:def = Invalid_IC_Item)
  ```
- Чтение одиночного JSON-значения (не из `Trie`):
  ```pawn
  stock T_IC_Item:PCSingle_ObjIcItem(const JSON:objectJson, const key[], const T_IC_Item:def = Invalid_IC_Item, const bool:dotNot = false, const bool:orFail = false)
  ```
- Выдача: `PCGet_IcItemGive(p, key, playerIndex)` — читает одиночный предмет из
  `Trie` и сразу выдаёт его игроку (для наборов — `PCGet_IcItemsGive`).

## Когда будет ошибка

Колбек возвращает `false` (значение не пишется в `Trie`), когда
`IC_Item_ReadFromJson` вернул `Invalid_IC_Item`:

- значение — не объект → `PCJson_ErrorForFile`: `Invalid item object (json value is not an object).`
- нет поля `Item` (и нет легаси `Type`) → "Invalid item object (must contains \`Item\` field)."
- `Item` пустой после `trim()` → `Item type is empty.`
- тип не зарегистрирован → `Item type '%s' not found.`
- обязательный параметр типа отсутствует или имеет неверное значение →
  `Param '%s' is required for item type '%s'.` / `Param '%s' has invalid value.`

Сообщения выводятся через `PCJson_ErrorForFile` (жёсткая ошибка, в отличие от
`PCJson_LogForFile`-WARNING).

## Побочные эффекты

- Создаётся экземпляр предмета (`ItemInstance_Construct`) и `Trie` его параметров;
  чтение выполняется при загрузке конфигов (обычно `plugin_precache`).
- Перед созданием вызывается событие `ItemType_OnRead` типа предмета; вернув
  `IC_RET_READ_FAIL`, тип может отклонить предмет.

## Ограничения

- На входе принимается **объект** предмета (ссылка `File:` разворачивается в
  объект); массив не подходит — для него `IC-Items`.
- Имя типа хранится в буфере `IC_ITEM_TYPE_NAME_MAX_LEN` (64).

## Примеры

```jsonc
// Простой предмет
{ "Item": "DefuseKit" }

// С параметрами
{ "Item": "Health", "Health": 50.0, "SetHealth": true }

// Несколько предметов — только через IC-Items, не IC-Item
```
