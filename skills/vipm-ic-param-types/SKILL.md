---
name: vipm-ic-param-types
description: >-
  Типы параметров ItemsController, регистрируемые фреймворком поверх
  ParamsController: IC-Item и IC-Items. Допустимые JSON-значения, что попадает в
  Trie, условия ошибки, отсутствие тегов, функции чтения (PCGet_IcItem(s),
  PCSingle_ObjIcItem(s)). Использовать при настройке/проверке значений этих типов
  в конфигах VipModular и при чтении их кодом. Триггеры: IC-Item, IC-Items,
  IC_PARAM_TYPE_ITEM_NAME, IC_PARAM_TYPE_ITEMS_NAME, типы параметров предметов.
  Use when configuring or reading ItemsController param types.
---

# Типы параметров ItemsController

Типы параметров, которые фреймворк ItemsController регистрирует в ParamsController
(`ItemsController/DefaultObjects/ParamType/`), — что писать в JSON и что из этого
попадёт в `Trie`.

- Встроенные типы контроллера (`Boolean`, `Integer`, `Model`, …) — в скилле `param-types`.
- Общие механизмы ParamsController (регистрация, синтаксис тегов, чтение значений,
  семантика ошибок) — в скиллах `param-type-registration` и `params-usage`.
- Структура объекта предмета (`"Item"` + параметры, вложенные `ItemsList`/`Random`/`If`,
  ссылки `File:`) — в скилле `vipm-config-items`; здесь она не дублируется.

## Типы

Файл каждого типа — в `references/`. Открывай только тот, что нужен для задачи.

| Тип (имя в конфиге) | Что это | Файл |
|---|---|---|
| `IC-Item` | один объект предмета → хендлер `T_IC_Item` | `references/ic-item.md` |
| `IC-Items` | массив / один объект / `File:`-ссылка на предметы → `Array` хендлеров `T_IC_Item` | `references/ic-items.md` |

Оба типа **не поддерживают теги** (`IC-Item:тег`, `IC-Items:тег`): их колбеки чтения
объявлены с одним аргументом (`const JSON:valueJson`) и игнорируют тег, который
ParamsController передаёт четвёртым параметром. Подробности — в файле типа.
