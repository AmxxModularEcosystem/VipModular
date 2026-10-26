#include <amxmodx>
#include <ParamsController>

#include "VipM/DebugMode"
#include "VipM/Core/Objects/Param"

API_Modules_Init(){
    register_native("VipM_Modules_Register", "@_Modules_Register");
    register_native("VipM_Modules_AddParams", "@_Modules_AddParams");
    register_native("VipM_Modules_AddParamsEx", "@VipM_Modules_AddParamsEx");
    register_native("VipM_Modules_RegisterEvent", "@_Modules_RegisterEvent");

    register_native("VipM_Modules_IsActive", "@_Modules_IsActive");
    register_native("VipM_Modules_GetParams", "@_Modules_GetParams");

}

T_ModuleType:@_Modules_Register(const pluginIndex) {
    enum {Arg_TypeName = 1}

    new typeName[VIPM_MODULES_TYPE_NAME_MAX_LEN];
    get_string(Arg_TypeName, typeName, charsmax(typeName));

    new T_ModuleType:type = ModuleType_Construct(typeName);
    if (type == Invalid_ModuleType) {
        log_error(0, "Can't create module type '%s'.", typeName);
        return Invalid_ModuleType;
    }

    return type;
}

@_Modules_AddParams(const pluginIndex, const iParamsNum) {
    enum {Arg_TypeName = 1, Arg_Params}

    new typeName[VIPM_MODULES_TYPE_NAME_MAX_LEN];
    get_string(Arg_TypeName, typeName, charsmax(typeName));

    new T_ModuleType:type = ModuleType_Find(typeName);
    if (type == Invalid_ModuleType) {
        log_error(0, "Module type '%s' not found.", typeName);
        return;
    }

    new Array:params = CfgParam_GetFromNative(Arg_Params, iParamsNum);
    if (params == Invalid_Array) {
        return;
    }

    ModuleType_AddParams(type, params);
    ArrayDestroy(params);
}

@VipM_Modules_AddParamsEx(const pluginIndex, const paramsCount) {
    enum {Arg_TypeName = 1, Arg_Params}

    new typeName[VIPM_MODULES_TYPE_NAME_MAX_LEN];
    get_string(Arg_TypeName, typeName, charsmax(typeName));

    new T_ModuleType:type = ModuleType_Find(typeName);
    if (type == Invalid_ModuleType) {
        log_error(0, "Module type '%s' not found.", typeName);
        return;
    }

    new Array:params = ParamsController_Param_ListFromNativeParams(Arg_Params, paramsCount);
    ModuleType_AddParams(type, params);
    ArrayDestroy(params);
}

bool:@_Modules_RegisterEvent(const pluginIndex) {
    enum {Arg_TypeName = 1, Arg_iEvent, Arg_sFuncName}

    new typeName[VIPM_MODULES_TYPE_NAME_MAX_LEN];
    get_string(Arg_TypeName, typeName, charsmax(typeName));

    new T_ModuleType:type = ModuleType_Find(typeName);
    if (type == Invalid_ModuleType) {
        log_error(0, "Module type '%s' not found.", typeName);
        return false;
    }

    new E_ModuleEvent:event = E_ModuleEvent:get_param(Arg_iEvent);

    new funcName[64];
    get_string(Arg_sFuncName, funcName, charsmax(funcName));
    
    ModuleType_SetEventListener(type, event, pluginIndex, funcName);
    return true;
}

bool:@_Modules_IsActive() {
    enum {Arg_TypeName = 1}

    new typeName[VIPM_MODULES_TYPE_NAME_MAX_LEN];
    get_string(Arg_TypeName, typeName, charsmax(typeName));

    new T_ModuleType:type = ModuleType_Find(typeName);
    if (type == Invalid_ModuleType) {
        log_error(0, "Module type '%s' not found.", typeName);
        return false;
    }

    return ModuleType_IsActive(type);
}

Trie:@_Modules_GetParams() {
    enum {Arg_TypeName = 1, Arg_PlayerIndex}

    new typeName[VIPM_MODULES_TYPE_NAME_MAX_LEN];
    get_string(Arg_TypeName, typeName, charsmax(typeName));

    // TODO: Потом переделать сразу на хендлеры
    new T_ModuleType:type = ModuleType_Find(typeName);
    if (type == Invalid_ModuleType) {
        log_error(AMX_ERR_PARAMS, "Module type '%s' not found.", typeName);
        return Invalid_Trie;
    }

    new playerIndex = get_param(Arg_PlayerIndex);
    
    return VipsManager_GetUserParams(playerIndex, type);
}
