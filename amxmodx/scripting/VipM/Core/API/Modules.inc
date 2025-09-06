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

T_ModuleType:@_Modules_Register(const PluginId) {
    enum {Arg_sModuleTypeName = 1}

    new sModuleTypeName[VIPM_MODULES_TYPE_NAME_MAX_LEN];
    get_string(Arg_sModuleTypeName, sModuleTypeName, charsmax(sModuleTypeName));

    new T_ModuleType:iModuleType = ModuleType_Construct(sModuleTypeName);
    if (iModuleType == Invalid_ModuleType) {
        log_error(0, "Can't create module type '%s'.", sModuleTypeName);
        return Invalid_ModuleType;
    }

    return iModuleType;
}

@_Modules_AddParams(const PluginId, const iParamsNum) {
    enum {Arg_sModuleTypeName = 1, Arg_Params}

    new sModuleTypeName[VIPM_MODULES_TYPE_NAME_MAX_LEN];
    get_string(Arg_sModuleTypeName, sModuleTypeName, charsmax(sModuleTypeName));

    new T_ModuleType:iModuleType = ModuleType_Find(sModuleTypeName);
    if (iModuleType == Invalid_ModuleType) {
        log_error(0, "Module type '%s' not found.", sModuleTypeName);
        return;
    }

    new Array:aParams = CfgParam_GetFromNative(Arg_Params, iParamsNum);
    if (aParams == Invalid_Array) {
        return;
    }

    ModuleType_AddParams(iModuleType, aParams);
}

@VipM_Modules_AddParamsEx(const pluginIndex, const paramsCount) {
    enum {Arg_ModuleName = 1, Arg_Params}

    new modelName[VIPM_MODULES_TYPE_NAME_MAX_LEN];
    get_string(Arg_ModuleName, modelName, charsmax(modelName));

    new T_ModuleType:module = ModuleType_Find(modelName);
    if (module == Invalid_ModuleType) {
        log_error(0, "Module type '%s' not found.", modelName);
        return;
    }

    new Array:params = ParamsController_Param_ListFromNativeParams(Arg_Params, paramsCount);
    ModuleType_AddParams(module, params);
    ArrayDestroy(params);
}

bool:@_Modules_RegisterEvent(const PluginId) {
    enum {Arg_sModuleTypeName = 1, Arg_iEvent, Arg_sFuncName}

    new sModuleTypeName[VIPM_MODULES_TYPE_NAME_MAX_LEN];
    get_string(Arg_sModuleTypeName, sModuleTypeName, charsmax(sModuleTypeName));

    new T_ModuleType:iModuleType = ModuleType_Find(sModuleTypeName);
    if (iModuleType == Invalid_ModuleType) {
        log_error(0, "Module type '%s' not found.", sModuleTypeName);
        return false;
    }

    new E_ModuleEvent:iEvent = E_ModuleEvent:get_param(Arg_iEvent);

    new sFuncName[64];
    get_string(Arg_sFuncName, sFuncName, charsmax(sFuncName));
    
    ModuleType_SetEventListener(iModuleType, iEvent, PluginId, sFuncName);
    return true;
}

bool:@_Modules_IsActive() {
    enum {Arg_sModuleTypeName = 1}

    new sModuleTypeName[VIPM_MODULES_TYPE_NAME_MAX_LEN];
    get_string(Arg_sModuleTypeName, sModuleTypeName, charsmax(sModuleTypeName));

    new T_ModuleType:iModuleType = ModuleType_Find(sModuleTypeName);
    if (iModuleType == Invalid_ModuleType) {
        log_error(0, "Module type '%s' not found.", sModuleTypeName);
        return false;
    }

    return ModuleType_IsActive(iModuleType);
}

Trie:@_Modules_GetParams() {
    enum {Arg_sModuleTypeName = 1, Arg_UserId}

    new sModuleTypeName[VIPM_MODULES_TYPE_NAME_MAX_LEN];
    get_string(Arg_sModuleTypeName, sModuleTypeName, charsmax(sModuleTypeName));

    // TODO: Потом переделать сразу не хендлеры
    new T_ModuleType:iModuleType = ModuleType_Find(sModuleTypeName);
    if (iModuleType == Invalid_ModuleType) {
        log_error(0, "Module type '%s' not found.", sModuleTypeName);
        return Invalid_Trie;
    }

    new UserId = get_param(Arg_UserId);
    
    return VipsManager_GetUserParams(UserId, iModuleType);
}
