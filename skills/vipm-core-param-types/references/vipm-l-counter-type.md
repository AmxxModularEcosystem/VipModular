# VipM-L-CounterType

Тип счётчика лимита `Counter` (строкой).

- **Константа:** `VIPM_L_COUNTER_PARAM_TYPE` (`"VipM-L-CounterType"`) —
  `amxmodx/scripting/include/VipM/L/Counter.inc`.
- **Регистрация:** `amxmodx/scripting/VipM/DefaultObjects/ParamType/CounterType.inc` —
  `ParamsController_RegSimpleType(VIPM_L_COUNTER_PARAM_TYPE, "@DefaultObjects_ParamType_CounterType_OnRead")`.

## Допустимые значения в JSON

Строка; регистр не важен. Принимаются полная форма `Per<X>` и короткая `<X>`:

| JSON-строка | Результат |
|---|---|
| `"PerRound"` / `"Round"` | `VipM_L_Counter_PerRound` |
| `"PerLife"` / `"Life"` | `VipM_L_Counter_PerLife` |
| `"PerSession"` / `"Session"` | `VipM_L_Counter_PerSession` |
| `"PerGame"` / `"Game"` | `VipM_L_Counter_PerGame` |
| `"PerMap"` / `"Map"` | `VipM_L_Counter_PerMap` |

```jsonc
{ "Type": "PerRound" }

{ "Type": "Round" }
```

## Теги

Тег не поддерживается: колбек `@DefaultObjects_ParamType_CounterType_OnRead`
объявлен с одним аргументом (`const JSON:valueJson`) и не получает и не
обрабатывает тег. Запись `VipM-L-CounterType:<tag>` синтаксически принимается
контроллером, но тег игнорируется.

## Что получится в Trie

Под ключом — ячейка с `VipM_L_Counter_Type`:

| Константа | Значение |
|---|---|
| `VipM_L_Counter_PerLife` | 0 |
| `VipM_L_Counter_PerRound` | 1 |
| `VipM_L_Counter_PerSession` | 2 |
| `VipM_L_Counter_PerGame` | 3 |
| `VipM_L_Counter_PerMap` | 4 |

Читается:

- `PCGet_VipmCounterType(p, key, def = VipM_L_Counter_PerLife)` — из Trie;
- `PCSingle_VipmCounterType(valueJson, def = VipM_L_Counter_PerLife, orFailKey = "")`
  — одиночное значение (не объект);
- `PCSingle_ObjVipmCounterType(objectJson, key, def = VipM_L_Counter_PerLife, dotNot = false, orFail = false)`
  — одиночное значение из JSON-объекта.

Все объявлены в `include/VipM/L/Counter.inc`.

## Когда будет ошибка

- Значение не строка — WARNING «Counter type must be a string (PerRound,
  PerLife, PerSession, PerGame, PerMap).»; параметр не читается.
- Строка не совпала ни с одной формой — WARNING «Counter type `%s` is not
  defined. Available types: PerRound, PerLife, PerSession, PerGame, PerMap.»;
  параметр не читается.

## Что преобразуется молча

- Сравнение без учёта регистра (`equali`): `"perround"` == `"PerRound"`.
- Короткие имена без префикса `Per` принимаются наравне с полными.
- Пробелы **не** обрезаются: `" Round "` не совпадёт и даст ошибку.

## Ограничения

- Строка читается в буфер `VIPM_L_COUNTER_KEY_MAX_LEN` (64 ячейки).

## Примеры

```jsonc
// Counter в параметрах лимита
{ "Limit": "Counter", "Type": "PerMap", "Key": "free_nade", "Max": 1 }

// Короткая форма
{ "Limit": "Counter", "Type": "Round", "Key": "buy_awp", "Max": 2 }
```

```pawn
new VipM_L_Counter_Type:type = PCGet_VipmCounterType(p, "Type");
```
