---
name: vipm-core-param-types
description: >-
  Типы параметров ядра VipModular, регистрируемые плагином поверх ParamsController:
  VipM-Limit, VipM-Limits, VipM-LimitType, VipM-ModuleType, VipM-L-CounterType.
  Допустимые JSON-значения, что попадает в Trie, условия ошибки, отсутствие тегов
  и функции чтения (PCGet_*/PCSingle_*). Использовать при настройке/проверке
  значений этих типов в конфигах VipModular и при чтении их кодом. Триггеры:
  VipM-Limit, VipM-Limits, VipM-LimitType, VipM-ModuleType, VipM-L-CounterType,
  VIPM_PARAM_TYPE_LIMIT_NAME, VIPM_L_COUNTER_PARAM_TYPE, типы параметров ядра.
  Use when configuring or reading core VipModular param types.
---

# Типы параметров ядра VipModular

Справочник по **кастомным типам параметров**, которые ядро VipModular
регистрирует поверх ParamsController: что писать в JSON и что из этого попадёт в
`Trie` (и каким геттером читается). Каждый тип — отдельный файл в `references/`;
открывай только нужные.

> **Границы скилла.** Встроенные типы ParamsController (`Integer`, `Boolean`,
> `Flags`, `Time`, `Model`, …) — в скилле `param-types`. Общие механизмы
> ParamsController (регистрация параметров, синтаксис тегов, чтение значений
> кодом, семантика ошибок чтения) — в скиллах `param-type-registration` и
> `params-usage`. Этот скилл описывает только специфику пяти типов ядра и **не**
> дублирует перечисленное.

## Типы

| Тип (имя в конфиге) | Что это | Файл |
|---|---|---|
| `VipM-Limit` | один лимит (условие доступа) → `T_LimitUnit` | `references/vipm-limit.md` |
| `VipM-Limits` | список лимитов → `Array` из `T_LimitUnit` | `references/vipm-limits.md` |
| `VipM-LimitType` | имя зарегистрированного типа лимита → `T_LimitType` | `references/vipm-limit-type.md` |
| `VipM-ModuleType` | имя зарегистрированного типа модуля → `T_ModuleType` | `references/vipm-module-type.md` |
| `VipM-L-CounterType` | тип счётчика → `VipM_L_Counter_Type` | `references/vipm-l-counter-type.md` |

Константа и путь регистрации каждого типа указаны в начале его файла.

## Общее для нескольких типов

- `references/_file-link.md` — ссылка `"File:<path>"`: разрешение пути, кэш и
  ограничение «ссылка ведёт на один объект». Общий блок для `VipM-Limit` и
  `VipM-Limits`.
