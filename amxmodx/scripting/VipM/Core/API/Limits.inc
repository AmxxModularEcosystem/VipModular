#include <amxmodx>
#include <VipM/Limits>
#include <ParamsController>

#include "VipM/Core/Objects/Limits/Type"
#include "VipM/Core/Objects/Limits/Unit"
#include "VipM/Core/Objects/Param"

API_Limits_Init() {
    register_native("VipM_Limits_RegisterType", "@_Limits_RegisterType");
    register_native("VipM_Limits_AddTypeParams", "@_Limits_AddTypeParams");
    register_native("VipM_Limits_AddParamsEx", "@VipM_Limits_AddParamsEx");
    register_native("VipM_Limits_RegisterTypeEvent", "@_Limits_RegisterTypeEvent");

    register_native("VipM_Limits_ReadFromJson", "@_Limits_ReadFromJson");
    register_native("VipM_Limits_ReadListFromJson", "@_Limits_ReadListFromJson");

    register_native("VipM_Limits_SetStaticValue", "@_Limits_SetStaticValue");

    register_native("VipM_Limits_Execute", "@_Limits_Execute");
    register_native("VipM_Limits_ExecuteList", "@_Limits_ExecuteList");
}

static T_LimitType:API_Limits_GetTypeParam(const param) {
    new limitName[VIPM_LIMITS_TYPE_NAME_MAX_LEN];
    get_string(param, limitName, charsmax(limitName));

    new T_LimitType:type = LimitType_Find(limitName);
    if (type == Invalid_LimitType) {
        abort(AMX_ERR_PARAMS, "Limit type '%s' not found.", limitName);
    }

    return type;
}

// native VipM_Limits_RegisterType(const limitName[], const bool:isForPlayer = true, const bool:isStatic = false);
T_LimitType:@_Limits_RegisterType() {
    enum {Arg_LimitName = 1, Arg_IsForPlayer, Arg_IsStatic}

    new limitName[VIPM_LIMITS_TYPE_NAME_MAX_LEN];
    get_string(Arg_LimitName, limitName, charsmax(limitName));

    new bool:isForPlayer = bool:get_param(Arg_IsForPlayer);
    new bool:isStatic = bool:get_param(Arg_IsStatic);
    
    new T_LimitType:type = LimitType_Construct(limitName, isForPlayer, isStatic);
    if (type == Invalid_LimitType) {
        log_error(0, "Can't create limit type '%s'.", limitName);
        return Invalid_LimitType;
    }

    return type;
}

// native VipM_Limits_AddTypeParams(const limitName[], any:...);
@_Limits_AddTypeParams(const pluginIndex, const paramsCount) {
    enum {Arg_LimitName = 1, Arg_Params}

    new T_LimitType:type = API_Limits_GetTypeParam(Arg_LimitName);
    new Array:params = CfgParam_GetFromNative(Arg_Params, paramsCount);

    if (params == Invalid_Array) {
        return;
    }

    LimitType_AddParams(type, params);
}

// native VipM_Limits_AddParamsEx(const limitName[], any:...);
@VipM_Limits_AddParamsEx(const pluginIndex, const paramsCount) {
    enum {Arg_LimitName = 1, Arg_Params}

    new T_LimitType:type = API_Limits_GetTypeParam(Arg_LimitName);
    new Array:params = ParamsController_Param_ListFromNativeParams(Arg_Params, paramsCount);

    LimitType_AddParams(type, params);
    ArrayDestroy(params);
}

// native VipM_Limits_RegisterTypeEvent(const limitName[], const E_LimitEvent:event, const func[]);
@_Limits_RegisterTypeEvent(const pluginIndex) {
    enum {Arg_LimitName = 1, Arg_Event, Arg_Func}

    new T_LimitType:type = API_Limits_GetTypeParam(Arg_LimitName);
    new E_LimitEvent:event = E_LimitEvent:get_param(Arg_Event);

    new func[64];
    get_string(Arg_Func, func, charsmax(func));
    
    LimitType_SetEventListener(type, event, pluginIndex, func);
}

// native VipM_Limits_Execute(const limitName[], const bool:value, const playerIndex = 0);
@_Limits_SetStaticValue() {
    enum {Arg_LimitName = 1, Arg_Value, Arg_PlayerIndex}

    new T_LimitType:type = API_Limits_GetTypeParam(Arg_LimitName);
    new bool:value = bool:get_param(Arg_Value);
    new playerIndex = get_param(Arg_PlayerIndex);
    
    LimitType_SetStaticValue(type, value, playerIndex);
}

// native T_LimitUnit:VipM_Limits_ReadFromJson(const JSON:limitJson);
T_LimitUnit:@_Limits_ReadFromJson() {
    enum {Arg_JsonValue = 1}

    new JSON:limitJson = JSON:get_param(Arg_JsonValue);
    
    return LimitUnit_Read(limitJson);
}

// native Array:VipM_Limits_ReadListFromJson(const JSON:limitsJson, Array:limits = Invalid_Array);
Array:@_Limits_ReadListFromJson() {
    enum {Arg_JsonValue = 1, Arg_Array}

    new JSON:limitsJson = JSON:get_param(Arg_JsonValue);
    new Array:limits = Array:get_param(Arg_Array);
    
    return LimitUnit_ReadList(limitsJson, limits);
}

// native bool:VipM_Limits_Execute(const T_LimitUnit:limit, const playerIndex = 0);
bool:@_Limits_Execute() {
    enum {Arg_LimitUnit = 1, Arg_PlayerIndex}

    new T_LimitUnit:limit = T_LimitUnit:get_param(Arg_LimitUnit);
    new playerIndex = get_param(Arg_PlayerIndex);
    
    return LimitUnit_Execute(limit, playerIndex);
}

// native bool:VipM_Limits_ExecuteList(const Array:limits, const playerIndex = 0, const E_LimitsExecType:type = Limit_Exec_OR);
bool:@_Limits_ExecuteList() {
    enum {Arg_LimitUnitList = 1, Arg_PlayerIndex, Arg_Type}

    new Array:limits = Array:get_param(Arg_LimitUnitList);
    new playerIndex = get_param(Arg_PlayerIndex);
    new E_LimitsExecType:type = E_LimitsExecType:get_param(Arg_Type);
    
    return LimitUnit_ExecuteList(limits, playerIndex, type);
}
