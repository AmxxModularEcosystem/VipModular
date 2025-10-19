#if defined __vipm_limits_included
	#endinput
#endif
#define __vipm_limits_included

#include <json>
#include <ParamsController>

/**
 * Vip Modular: Limits
*/

#define VIPM_LIMITS_TYPE_NAME_MAX_LEN 64

stock const VIPM_PARAM_TYPE_LIMIT_NAME[] = "VipM-Limit";
stock const VIPM_PARAM_TYPE_LIMITS_NAME[] = "VipM-Limits";

enum T_LimitUnit {Invalid_LimitUnit = -1}

enum E_LimitEvent{
    /*
     * Описание:    Вызывается после прочтения параметров лимита.
     * Возв. тип:   VipM_FwdReturn
     * Параметры:   (const JSON:jUnit, Trie:tParams):
     *      jUnit - JSON-обьект с параметрами.
     *      tParams - Хэш-карта с прочитанными параметрами. Может быть изменена.
     * 
     * Примечание: Вызывается в plugin_precache
     * Примечание: Если возвращено VIPM_STOP, лимит будет пропущен.
     */
    Limit_OnRead,

    /*
     * Описание:    Вызывается при проверке выполнения условия.
     * Возв. тип:   bool
     * Параметры:   (const Trie:tParams, const UserId):
     *      tParams - Хэш-карта с параметрами. Для извлечения отдельных значений можно использовать стоковые функции VipM_Params_*.
     *      UserId - Индекс игрока. 0, если лимит не зависит от игрока.
     * 
     * Примечание: Если возвращено true, условие выполняется.
     * Примечание: Не вызывается для статических лимитов.
     */
    Limit_OnCheck,
}

enum E_LimitsExecType {
    /* ИЛИ */
    Limit_Exec_OR,

    /* И */
    Limit_Exec_AND,

    /* Исключающее ИЛИ */
    Limit_Exec_XOR,
}

/**
 * Вызывается в момент, когда нужно регистрировать типы лимитов.
 * Типы лимитов должны регистрироваться строго в рамках этого форварда.
 *
 * @note    Вызывается в рамках plugin_precache.
 * 
 * @deprecated Use VipM_Limits_OnInited instead.
 * 
 * @noreturn
 */
#pragma deprecated Use VipM_Limits_OnInited instead.
forward VipM_OnInitLimits();

/**
 * Вызывается в момент, когда нужно регистрировать типы лимитов.
 * Типы лимитов должны регистрироваться строго в рамках этого форварда.
 *
 * @note    Вызывается в рамках plugin_precache.
 *
 * @noreturn
 */
forward VipM_Limits_OnInited();

/**
 * Регистрирует в системе тип лимита.
 *
 * @param jUnit     Название регистрируемого типа.
 * @param tParams   Относится ли регистрируемый тип к игрокам.
 * @param tParams   Является ли регистрируемый тип статическим.
 *
 * @note            Статические типы не имеют параметров и колбэков, значения для них надо передавать через натив VipM_Limits_SetStaticValue.
 *                  Работают быстрее обычного. Могут относиться к игрокам.
 *                  Примеры статических типов есть в VipM-L-Default.sma
 *
 * @noreturn
 */
native VipM_Limits_RegisterType(const sName[], const bool:bForPlayer = true, const bool:bStatic = false);

/**
 * Регистрирует обработчик события для указанного типа лимита.
 *
 * @param sName     Название типа.
 * @param iEvent    Событие.
 * @param sFunc     Название функции-обработчика.
 *
 * @return  Вернёт true, если обработчик успешно зарегистрирован.
 */
native VipM_Limits_RegisterTypeEvent(const sName[], const E_LimitEvent:iEvent, const sFunc[]);

/**
 * Добавляет параметр(ы) для типа лимита.
 *
 * @param sName     Название типа.
 * @param any:...   Перечисление параметров.
 *
 * @note            Формат указания параметров: VipM_Limits_AddTypeParams(..., const ParamName[], const E_ParamType:ParamType, const bool:ParamRequired, ...);
 * @note            ParamName - Название параметра / ParamType - Тип параметра / ParamRequired - Обязателен ли параметр.
 * @note            Пример: VipM_Limits_AddTypeParams("ExampleModule", "Param1", ptInteger, true, "Param2", ptCustom, false);
 *
 * @noreturn
 */
native VipM_Limits_AddTypeParams(const sName[], any:...);

/**
 * Добавляет параметры для лимита.
 *
 * @param moduleName Название лимита.
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
 *                      VipM_Limits_AddParamsEx(limitName,
 *                          "Param1", "String", true,
 *                          "Param2", "Integer", false
 *                      );
 * 
 * @noreturn
 */
native VipM_Limits_AddParamsEx(const limitName[], const any:...);

/**
 * Читает лимит из JSON-обьекта.
 *
 * @param jLimit    JSON-обьект.
 *
 * @note            После вызова натива, JSON-обьект не очищается.
 *
 * @return          Индекс прочтённого лимита. Invalid_LimitUnit, если что-то пошло не так.
 */
native T_LimitUnit:VipM_Limits_ReadFromJson(const JSON:jLimit);

/**
 * Читает список лимитов из JSON-значения.
 *
 * @param jLimits   JSON-обьект.
 * @param aLimits   Динамический массив, в который надо добавить прочтённые элементы, либо Invalid_Array, если надо создать новый.
 *
 * @note            После вызова натива, JSON-обьект не очищается.
 * @note            При передаче параметра aLimits, функция вернёт этот же массив с новыми элементами.
 *
 * @return          Динамический массив с хендлерами прочтённых лимитов. Invalid_Array, если не было прочтено ни одного элемента.
 */
native Array:VipM_Limits_ReadListFromJson(const JSON:jLimits, Array:aLimits = Invalid_Array);

/**
 * Устанавливает значение для статического лимита.
 *
 * @param sName     Название типа.
 * @param bNewValue Новое значение.
 * @param UserId    Индекс игрока, если тип относится к игрокам.
 *
 * @note            Желательно инициализировать статическое значение после регистрации типа.
 *
 * @noreturn
 */
native VipM_Limits_SetStaticValue(const sName[], const bool:bNewValue, const UserId = 0);

/**
 * Выполняет проверку лимита.
 *
 * @param iLimit    Индекс лимита.
 * @param UserId    Индекс игрока.
 *
 * @note            Если индекс игрока не передан, попытка использовать относящийся к игроку лимит вызовет ошибку.
 *                  Но это уже ошибка конфигурации.
 *
 * @return          true, если условие выполняется, иначе false.
 */
native bool:VipM_Limits_Execute(const T_LimitUnit:iLimit, const UserId = 0);

/**
 * Выполняет проверку списка лимитов.
 *
 * @param aLimits   Динамический массив хендлеров лимитов.
 * @param UserId    Индекс игрока.
 * @param iType     Логический оператор. См. перечисление E_LimitsExecType.
 *
 * @note            Если индекс игрока не передан, попытка использовать относящийся к игроку лимит вызовет ошибку.
 *                  Но это уже ошибка конфигурации.
 *
 * @return          true, если условие выполняется, иначе false.
 */
native bool:VipM_Limits_ExecuteList(const Array:aLimits, const UserId = 0, const E_LimitsExecType:iType = Limit_Exec_OR);

stock Array:Json_Object_GetLimits(const JSON:jObj, const sKey[], const bool:bDotNot = false) {
    if (!json_object_has_value(jObj, sKey, .dot_not = bDotNot)) {
        return Invalid_Array;
    }

    new JSON:jLimits = json_object_get_value(jObj, sKey, bDotNot);
    new Array:aLimits = VipM_Limits_ReadListFromJson(jLimits);
    json_free(jLimits);

    return aLimits;
}

stock T_LimitUnit:PCGet_VipmLimit(const Trie:p, const key[], const T_LimitUnit:def = Invalid_LimitUnit) {
    return PCGet_Cell(p, key, def);
}

stock Array:PCGet_VipmLimits(const Trie:p, const key[], const Array:def = Invalid_Array) {
    return PCGet_Cell(p, key, def);
}

stock bool:PCGet_VipmLimitCheck(const Trie:p, const key[], const playerIndex = 0, const bool:def = true) {
    new T_LimitUnit:limit = PCGet_VipmLimit(p, key);
    if (limit == Invalid_LimitUnit) {
        return def;
    }

    return VipM_Limits_Execute(limit, playerIndex);
}

stock bool:PCGet_VipmLimitsCheck(const Trie:p, const key[], const playerIndex = 0, const E_LimitsExecType:type = Limit_Exec_OR, const bool:def = true) {
    new Array:limits = PCGet_VipmLimits(p, key);
    if (limits == Invalid_Array || ArraySize(limits) < 1) {
        return def;
    }

    return VipM_Limits_ExecuteList(limits, playerIndex, type);
}

stock T_LimitUnit:PCSingle_ObjVipmLimit(const JSON:objectJson, const key[], const T_LimitUnit:def = Invalid_LimitUnit, const bool:dotNot = false, const bool:orFail = false) {
    return PCSingle_ObjCell(objectJson, key, VIPM_PARAM_TYPE_LIMIT_NAME, def, dotNot, orFail);
}

stock Array:PCSingle_ObjVipmLimits(const JSON:objectJson, const key[], const Array:def = Invalid_Array, const bool:dotNot = false, const bool:orFail = false) {
    return PCSingle_ObjCell(objectJson, key, VIPM_PARAM_TYPE_LIMITS_NAME, def, dotNot, orFail);
}
