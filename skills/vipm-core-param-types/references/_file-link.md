# Ссылки `"File:<path>"`

Общий блок для `VipM-Limit` и `VipM-Limits`: обе разворачивают ссылку через
`LimitUnit_Read` (`const JSON:valueJson`).

## Формат

Строка, начинающаяся с `File:` (без учёта регистра). Всё после префикса — путь.

## Разрешение пути

| Форма | Куда разрешается |
|---|---|
| `"File:Limits/MinRound-2"` | `amxmodx/configs/plugins/VipModular/Limits/MinRound-2.json` |
| `"File:/Limits/MinRound-2"` | `amxmodx/configs/Limits/MinRound-2.json` |

- Без ведущего слэша — относительно каталога конфигов VipModular
  (`amxmodx/configs/plugins/VipModular/`); это workdir, с которым ядро парсит
  конфиги (`Vips.json`, файлы `Vips/`).
- С ведущим `/` — относительно `amxmodx/configs/`.
- Расширение `.json` дописывается автоматически, если его нет.

```jsonc
"File:Limits/MinRound-2"        // → plugins/VipModular/Limits/MinRound-2.json
"File:/Limits/Global/MinRound"  // → configs/Limits/Global/MinRound.json
```

## Ошибки

- Файла нет — аварийное завершение `File '%s' not found.`.
- В файле невалидный JSON — аварийное завершение
  `File '%s' contains invalid JSON value.`.

## Кэш

Прочитанное значение кэшируется по строке ссылки: повторная ссылка на тот же
путь не читает файл заново и возвращает уже созданный объект.

## Ссылка ведёт на один объект

`VipM-Limit` и `VipM-Limits` разворачивают ссылку через `LimitUnit_Read`, то есть
как **один** лимит. Файл-массив по ссылке **не** превращается в список: попытка
прочитать из массива поле `Limit` завершается аварийно
(`Json value must be an object`). Для списка используйте `VipM-Limits` с массивом,
каждый элемент которого — объект лимита или отдельная `File:`-ссылка на объект.
