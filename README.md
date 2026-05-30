# Vip Modular

Модульная система привилегий.

## Требования

- [ParamsController](https://github.com/AmxxModularEcosystem/ParamsController) версии [1.4.2](https://github.com/AmxxModularEcosystem/ParamsController/releases/tag/1.4.2) или [выше](https://github.com/AmxxModularEcosystem/ParamsController/releases/latest).
- [CommandAliases](https://github.com/AmxxModularEcosystem/CommandAliases) версии [1.0.1](https://github.com/AmxxModularEcosystem/CommandAliases/releases/tag/1.0.1-fix1) или [выше](https://github.com/AmxxModularEcosystem/CommandAliases/releases/latest).
- [ReAPI](https://github.com/rehlds/ReAPI) версии [5.24.0.300](https://github.com/rehlds/ReAPI/releases/tag/5.24.0.300) или [выше](https://github.com/rehlds/ReAPI/releases/latest).

## Инструкции по настройке

- [Основные настройки](readme/configs.md)
- [Контроллер предметов](readme/extensions/items.md)
- [Стандартные расширения](readme/default-extensions.md)
- [Сторонние расширения](readme/thirdparty-extensions.md)

## Серверные команды

| Команда                           | Описание                                                    |
| :-------------------------------- | :---------------------------------------------------------- |
| `vipm_update_users`               | Обновляет привилегии у всех игроков                         |
| `vipm_info`                       | Выводит информацию о системе привилегий                     |
| `vipm_modules`                    | Выводит таблицу модулей и их статусов                       |
| `vipm_limits`                     | Выводит таблицу типов проверок и некоторую информацию о них |
| `ic_item_types`                   | Выводит таблицу типов предметов                             |

## Идеи

- _\[Configs\]_ Добавить глобальные настройки типа ключ-значение, на которые можно ссылаться из основных конфигов как на файлы, только с префиксом Var:
- _\[Modules\]_ Переработать обьедининение параметров модулей
- _\[IC\]_ Придумать как норм отвязать IC от ядра и, возможно, вынести его в отдельный репо (WIP)
