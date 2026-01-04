#if defined _vipmodular_m_included
	#endinput
#endif
#define _vipmodular_m_included

/**
 * Vip Modular: Modules
*/

#define VIPM_MODULES_TYPE_NAME_MAX_LEN 64

stock const VIPM_MODULES_PARAMS_TEMP_MARK_KEY[] = "___temp_module_params___";

enum E_ModuleEvent{

    /*
     * Описание:    Вызывается, когда модуль был активирован.
     * Возв. тип:   VipM_FwdReturn
     * Параметры:   ()
     * 
     * Примечание: Вызывается в plugin_precache
     * Примечание: Если возвращено VIPM_STOP, моудль не будет активирован.
     */
    Module_OnActivated,

    /*
     * Описание:    Вызывается, когда читаются параметры модуля.
     * Возв. тип:   VipM_FwdReturn
     * Параметры:   (const JSON:moduleJson, Trie:p):
     *      moduleJson - JSON-обьект с параметрами.
     *      p - Хэш-карта с прочитанными параметрами. Может быть изменена.
     * 
     * Примечание: Вызывается в plugin_precache
     * Примечание: Если возвращено VIPM_STOP, элемент модуля будет пропущен.
     */
    Module_OnRead,

    /*
     * Описание:    Вызывается, когда надо обьединить два набора параметров модуля. Только для модулей с Once = false.
     * Возв. тип:   Trie
     * Параметры:   (const Trie:MainParams, const Trie:NewParams):
     *      MainParams - Основной набор параметров модуля.
     *      NewParams - Добавляемый набор параметров модуля.
     * 
     * Примечание: Результат обьединения должен быть возвращён обработчиком события.
     * 
     * УСТАРЕЛО! Используйте Module_OnMergeParams. 
     */
    Module_OnCompareParams,

    /*
     * Описание:    Вызывается при добавлении игроку второго и последующих модулей одного типа.
     * Возв. тип:   Trie
     * Параметры:   (const Trie:p1, const Trie:p2):
     *      p1 - Первый набор параметров.
     *      p2 - Второй набор параметров.
     * 
     * Примечание: Если обработчик для этого события не зарегистрирован,
     *             всегда будет выбираться p1 (модуль, добавленный первым) без изменений.
     * 
     * Примечание: Очищение/изменение переданных в параметры Trie недопустимо.
     * 
     * Примечание: Обработчик может вернуть один из полученных Trie или создать новый.
     *             При содании нового Trie ядро само позаботится о его очищении в будущем.
     */
    Module_OnMergeParams,
}

/**
 * Вызывается перед активацией модуля.
 *
 * @param moduleName Название модуля.
 *
 * @return VIPM_CONTINUE - модуль будет активирован, VIPM_STOP - активация модуля будет прервана.
 */
forward VipM_Modules_OnActivate(const moduleName[]);

/**
 * Регистрирует новый модуль.
 *
 * @param moduleName Название модуля.
 * @param Once       Обьединять ли параметры элементов модуля. true - Нет, false - Да. Подразумевает использование события OnCompareParams.
 *
 * @noreturn
 */
native VipM_Modules_Register(const moduleName[], const bool:Once = true);

/**
 * Регистрирует обработчик события для указанного модуля.
 *
 * @param moduleName Название модуля.
 * @param event      Событие.
 * @param func       Название функции-обработчика.
 *
 * @return  Вернёт true, если обработчик успешно зарегистрирован.
 */
native bool:VipM_Modules_RegisterEvent(const moduleName[], const E_ModuleEvent:event, const func[]);

/**
 * Добавляет набор параметров для указанного модуля.
 *
 * @param moduleName Название модуля.
 * @param any:...    Перечисление параметров.
 *
 * @note    Формат указания параметров: VipM_Modules_AddParams(..., const ParamName[], const E_ParamType:ParamType, const bool:ParamRequired, ...);
 * @note    ParamName - Название параметра / ParamType - Тип параметра / ParamRequired - Обязателен ли параметр.
 * @note    Пример: VipM_Modules_AddParams("ExampleModule", "Param1", ptInteger, true, "Param2", ptCustom, false);
 *
 * @noreturn
 */
native VipM_Modules_AddParams(const moduleName[], const any:...);

/**
 * Добавляет параметры для модуля.
 *
 * @param moduleName Название модуля.
 * @param any:...    Перечисление параметров.
 * 
 * @note            Формат перечисления параметров:
 *                      VipM_Modules_AddParamsEx(..., const paramName[], const paramType[], const bool:paramRequired, ...);
 * 
 *                  Где:
 *                  - paramName - Название параметра;
 *                  - paramType - Название типа параметра;
 *                  - paramRequired - Обязателен ли параметр.
 * 
 *                  Пример:
 *                      VipM_Modules_AddParamsEx(moduleName,
 *                          "Param1", "String", true,
 *                          "Param2", "Integer", false
 *                      );
 * 
 * @noreturn
 */
native VipM_Modules_AddParamsEx(const moduleName[], const any:...);

/**
 * Активен ли указанный модуль.
 *
 * @param Module    Название модуля.
 *
 * @return true, если модуль активен, иначе false.
 */
native bool:VipM_Modules_IsActive(const Module[]);

/**
 * Получение набора параметров модуля для указанного игрока.
 *
 * @param Module    Название модуля.
 * @param UserId    Индекс игрока.
 *
 * @return Trie с параметрами.
 */
native Trie:VipM_Modules_GetParams(const Module[], const UserId);

/**
 * Проверка наличия у игрока доступа к указанному модулю.
 *
 * @param sModuleName   Название модуля.
 * @param UserId        Индекс игрока.
 *
 * @return true, если у игрока имеется доступ к модулю, иначе false.
 */
stock bool:VipM_Modules_HasModule(const sModuleName[], const UserId) {
    return VipM_Modules_GetParams(sModuleName, UserId) != Invalid_Trie;
}
