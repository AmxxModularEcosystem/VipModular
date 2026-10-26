# IC-Items

Список предметов ItemsController — массив, один объект или `File:`-ссылка, —
прочитанный в динамический массив `Array` хендлеров `T_IC_Item`.

## Допустимые значения в JSON

- **массив** предметов; элемент массива — объект предмета, строка-ссылка или
  вложенный массив (рекурсия):
  ```jsonc
  [
      { "Item": "Weapon", "Name": "weapon_deagle", "GiveType": "Replace" },
      "File:Items/DefuseKit",
      [ { "Item": "Money", "Amount": 500 } ]
  ]
  ```
- **один объект** предмета:
  ```jsonc
  { "Item": "DefuseKit" }
  ```
- **строка-ссылка** `"File:<path>"` (без расширения, относительно конфигов
  VipModular). Ссылки разворачиваются: `ItemInstance_ReadArrayFromJsonValue` вызывает
  `PCJson_HandleLinkedValue`; развёрнутое значение может быть объектом или массивом
  (в т.ч. снова ссылкой — рекурсивно).

Объект предмета — той же формы, что и у `IC-Item` (`"Item"` + параметры; легаси
`"Type"` с WARNING). Полное описание полей — в `references/ic-item.md`.

## Теги

Не поддерживаются. Регистрация — `ParamsController_RegSimpleType(IC_PARAM_TYPE_ITEMS_NAME,
"@OnItemsParamTypeRead")`, колбек `@OnItemsParamTypeRead(const JSON:valueJson)`
принимает только значение и игнорирует тег (четвёртый аргумент `ParamsController`).
`IC-Items:любойТег` обрабатывается как `IC-Items`.

## Что получится в Trie

- Под ключом — **cell**: хендлер `Array` с хендлерами `T_IC_Item` (`Invalid_Array` при
  отсутствии).
- Чтение массива:
  ```pawn
  stock Array:PCGet_IcItems(const Trie:p, const key[], const Array:def = Invalid_Array)
  ```
- Выдача игроку (единственный рабочий готовый helper выдачи):
  ```pawn
  stock bool:PCGet_IcItemsGive(const Trie:p, const key[], const playerIndex)
  // вызывает IC_Item_GiveArray(playerIndex, PCGet_IcItems(p, key))
  ```
- Чтение одиночного JSON-значения (не из `Trie`):
  ```pawn
  stock Array:PCSingle_ObjIcItems(const JSON:objectJson, const key[], const Array:def = Invalid_Array, const bool:dotNot = false, const bool:orFail = false)
  ```

Устаревшее: `Json_Object_IC_GetItems` (обёртка над `PCSingle_ObjIcItems`) помечено
`#pragma deprecated`.

## Когда будет ошибка

Колбек возвращает `false` (значение не пишется в `Trie`):

- значение (после разворачивания ссылки) — не массив и не объект →
  `PCJson_ErrorForFile`: `Json value must be an array or an object.`
  (жёсткая ошибка; массив при этом остаётся пустым);
- значение — **объект**, но ни одного валидного предмета не вышло;
- значение — **пустой массив**.

В двух последних случаях пишется WARNING `All items are invalid.` и возвращается
`false`.

## Что преобразуется молча

- Вложенные массивы могут не дать ни одного предмета (например, пустые) — тогда они
  просто ничего не добавляют, без ошибки.
- Непустой массив, в котором не оказалось ни одного предмета, не подпадает под
  проверку выше (`json_array_get_count(valueJson) != 0`), поэтому колбек вернёт `true`
  с пустым `Array` — предметы молча теряются.
- Вложенные массивы разворачиваются; итоговый `Array` — плоский список предметов
  (без сохранения вложенности).

## Побочные эффекты

- Создаётся общий `Array` (`ArrayCreate(1, 1)`), в который дописываются все
  прочитанные экземпляры; при рекурсии он передаётся по ссылке.
- Каждый объектный элемент создаёт экземпляр предмета (`ItemInstance_Construct`)
  и `Trie` его параметров; освобождение — `IC_Item_Free` / `IC_Item_Free` для каждого.

## Ограничения

- Начальная ёмкость массива — 1 ячейка (`ArrayCreate(1, 1)`), массив растёт
  автоматически.
- Глубина рекурсии вложенных массивов/ссылок не ограничена явно.

## Примеры

```jsonc
// Один предмет
{ "Item": "DefuseKit" }

// Ссылка на файл с одним предметом — configs/plugins/VipModular/Items/DefuseKit.json
"File:Items/DefuseKit"

// Массив с ссылкой и вложенным массивом
[
    { "Item": "Weapon", "Name": "weapon_hegrenade" },
    "File:Items/DefuseKit",
    [ { "Item": "Money", "Amount": 500 } ]
]
```
