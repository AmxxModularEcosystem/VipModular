#if defined _vipmodular_params_included
	#endinput
#endif
#define _vipmodular_params_included

#include <amxmodx>
#include <ParamsController>

#define VIPM_PARAM_NAME_MAX_LEN PARAM_KEY_MAX_LEN

/**
 * Vip Modular: Parameters
*/

#pragma deprecated Use params controller instead
enum E_ParamType {

    /*
     * Пользовательский тип параметра. Подразумевает использование события OnRead.
     * Обязательность таких параметров игнорируется.
     */
    #pragma deprecated Use params controller instead
    ptCustom = 0,
    
    /*
     * Целое число.
     */
    #pragma deprecated Use params controller instead
    ptInteger,
    
    /*
     * Дробное число.
     */
    #pragma deprecated Use params controller instead
    ptFloat,
    
    /*
     * true/false.
     */
    #pragma deprecated Use params controller instead
    ptBoolean,
    
    /*
     * Строка.
     */
    #pragma deprecated Use params controller instead
    ptString,
    
    /*
     * Цвет (Массив из трёх целых чисел).
     */
    #pragma deprecated Use params controller instead
    ptColor,
    
    /*
     * Двумерный вектор (Массив из двух дробных чисел).
     */
    #pragma deprecated Use params controller instead
    ptVector2,
    
    /*
     * Трёхмерный вектор (Массив из трёх дробных чисел).
     */
    #pragma deprecated Use params controller instead
    ptVector3,
    
    /*
     * Условный элемент.
     */
    #pragma deprecated Use params controller instead
    ptLimit,
    
    /*
     * Массив условных элементов.
     */
    #pragma deprecated Use params controller instead
    ptLimits,
}
stock const VIPM_PARAM_TYPE_NAMES[E_ParamType][] = {"Custom", "Integer", "Float", "Bool", "String", "Color", "Vector2", "Vector3", "Limit", "Limits"};

#pragma deprecated Use PCGet_*() stocks from ParamsController instead.
stock any:VipM_Params_GetCell(const Trie:tParams, const sKey[], const any:iDef = 0) {
    return PCGet_Cell(tParams, sKey, iDef);
}

#pragma deprecated Use PCGet_*() stocks from ParamsController instead.
stock VipM_Params_GetInt(const Trie:tParams, const sKey[], const iDef = 0) {
    return PCGet_Int(tParams, sKey, iDef);
}

#pragma deprecated Use PCGet_*() stocks from ParamsController instead.
stock bool:VipM_Params_GetBool(const Trie:tParams, const sKey[], const bool:bDef = false) {
    return PCGet_Bool(tParams, sKey, bDef) ? true : false;
}

#pragma deprecated Use PCGet_*() stocks from ParamsController instead.
stock Float:VipM_Params_GetFloat(const Trie:tParams, const sKey[], const Float:fDef = 0.0) {
    return PCGet_Float(tParams, sKey, fDef);
}

#pragma deprecated Use PCGet_*() stocks from ParamsController instead.
stock VipM_Params_GetStr(const Trie:tParams, const sKey[], sOut[], const iOutLen, const sDef[] = NULL_STRING) {
    return PCGet_Str(tParams, sKey, sOut, iOutLen, sDef);
}

#pragma deprecated Use PCGet_*() stocks from ParamsController instead.
stock VipM_Params_GetStri(const Trie:tParams, const sKey[], const sDef[] = NULL_STRING) {
    new str[PARAM_VALUE_MAX_LEN];
    PCGet_Str(p, key, str, charsmax(str), def);
    return str;
}

stock Array:VipM_Params_GetArr(const Trie:tParams, const sKey[]) {
    return Array:PCGet_Cell(tParams, sKey, Invalid_Array);
}

#pragma deprecated Use PCGet_VipmLimit() instead
stock T_LimitUnit:VipM_Params_GetLimit(const Trie:tParams, const sKey[], const T_LimitUnit:iDef = Invalid_LimitUnit) {
    return T_LimitUnit:PCGet_Cell(tParams, sKey, iDef);
}

#pragma deprecated Use PCGet_VipmLimitCheck() instead
stock bool:VipM_Params_ExecuteLimit(
    const Trie:tParams,
    const sKey[] = "Limit",
    const UserId = 0,
    const bool:bDef = true
) {
    new T_LimitUnit:iLimit = VipM_Params_GetLimit(tParams, sKey);
    if (iLimit == Invalid_LimitUnit) {
        return bDef;
    }

    return VipM_Limits_Execute(iLimit, UserId);
}

#pragma deprecated Use PCGet_VipmLimitsCheck() instead
stock bool:VipM_Params_ExecuteLimitsList(
    const Trie:tParams,
    const sKey[] = "Limits",
    const UserId = 0,
    const E_LimitsExecType:iType = Limit_Exec_OR,
    const bool:bDef = true
) {
    new Array:aLimits = VipM_Params_GetArr(tParams, sKey);
    if (aLimits == Invalid_Array) {
        return bDef;
    }
    
    return VipM_Limits_ExecuteList(aLimits, UserId, iType);
}
