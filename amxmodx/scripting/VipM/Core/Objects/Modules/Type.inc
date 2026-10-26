#if defined __vipm_core_objects_modules_type_included
    #endinput
#endif
#define __vipm_core_objects_modules_type_included

#include <amxmodx>
#include <VipM/Modules>
#include <VipM/Params>
#include "VipM/Forwards"
#include "VipM/Utils"
#include "VipM/ArrayMap"
#include "VipM/ArrayTrieUtils"
#include "VipM/Core/Objects/Param"

enum _:S_ModuleType {
    ModuleType_Name[VIPM_MODULES_TYPE_NAME_MAX_LEN],
    Array:ModuleType_Params,

    bool:ModuleType_Used,
    bool:ModuleType_Active,

    ModuleType_Events[E_ModuleEvent],
}

enum T_ModuleType { Invalid_ModuleType = -1 }

static ArrayMap(ModuleTypes); // S_ModuleType

ModuleType_Init() {
    CallOnce();
    log_amx("Init module types...");

    ArrayMapCreate(ModuleTypes, S_ModuleType);
    
    Forwards_Reg("VipM_Modules_OnActivate", ET_STOP, FP_STRING);
    
    log_amx("Module types inited.");
}

Array:ModuleType_GetAll() {
    return ArrayClone(ModuleTypes[AM_Arr]);
}

ModuleType_GetCount() {
    return ArrayMapSize(ModuleTypes);
}

T_ModuleType:ModuleType_Construct(const sName[]) {
    if (!ArrayMapCreated(ModuleTypes)) {
        abort(AMX_ERR_GENERAL, "Attempt to create module type before modules init.");
        return Invalid_ModuleType;
    }

    if (ModuleType_Find(sName) != Invalid_ModuleType) {
        abort(AMX_ERR_PARAMS, "Module type '%s' already exists.", sName);
        return Invalid_ModuleType;
    }

    new ModuleType[S_ModuleType];

    copy(ModuleType[ModuleType_Name], charsmax(ModuleType[ModuleType_Name]), sName);
    ModuleType[ModuleType_Params] = ArrayCreate(1, 1);

    ModuleType[ModuleType_Used] = false;
    ModuleType[ModuleType_Active] = false;

    arrayset(ModuleType[ModuleType_Events], -1, sizeof(ModuleType[ModuleType_Events]));
    
    log_amx("Module type '%s' registered.", ModuleType[ModuleType_Name]);

    return T_ModuleType:ArrayMapPushArray(ModuleTypes, ModuleType, ModuleType[ModuleType_Name]);
}

T_ModuleType:ModuleType_Find(const sName[]) {
    if (ArrayMapKeyExists(ModuleTypes, sName)) {
        return T_ModuleType:ArrayMapGetIndex(ModuleTypes, sName);
    } else {
        return Invalid_ModuleType;
    }
}

static ModuleType__Get(const T_ModuleType:iModuleType, ModuleType[S_ModuleType]) {
    ArrayMapGetArray(ModuleTypes, _:iModuleType, ModuleType);
}

static ModuleType__Save(const ModuleType[S_ModuleType]) {
    ArrayMapSetArrayByKey(ModuleTypes, ModuleType[ModuleType_Name], ModuleType);
}

static ModuleType__SetValue(const T_ModuleType:iModuleType, const iCell, const any:iValue) {
    return ArrayMapSetCell(ModuleTypes, iModuleType, iValue, iCell);
}

static any:ModuleType__GetValue(const T_ModuleType:iModuleType, const iCell) {
    return ArrayMapGetCell(ModuleTypes, iModuleType, iCell);
}

ModuleType_GetName(const T_ModuleType:iModuleType, sOut[], const iOutLen) {
    new ModuleType[S_ModuleType];
    ModuleType__Get(iModuleType, ModuleType);

    return copy(sOut, iOutLen, ModuleType[ModuleType_Name]);
}

ModuleType_iGetName(const T_ModuleType:iModuleType) {
    new sName[VIPM_MODULES_TYPE_NAME_MAX_LEN];
    ModuleType_GetName(iModuleType, sName, charsmax(sName));
    return sName;
}

bool:ModuleType_IsActive(const T_ModuleType:iModuleType) {
    return ModuleType__GetValue(iModuleType, ModuleType_Active);
}

Trie:ModuleType_ReadParams(const T_ModuleType:type, const JSON:paramsJson) {
    new typeObject[S_ModuleType];
    ModuleType__Get(type, typeObject);
    
    new E_ParamsReadErrorType:errType, errParam[PARAM_KEY_MAX_LEN];
    new Trie:params = ParamsController_Param_ReadList(
        typeObject[ModuleType_Params], paramsJson,
        .iErrType = errType,
        .sErrParamName = errParam,
        .iErrParamNameLen = charsmax(errParam)
    );

    if (errType != ParamsReadError_None) {
        TrieDestroy(params);

        switch (errType) {
            case ParamsReadError_RequiredParamNotPresented: {
                PCJson_ErrorForFile(paramsJson, "Param '%s' required for '%s' module.", errParam, typeObject[ModuleType_Name]);
            }
            case ParamsReadError_ParamValueIsInvalid: {
                PCJson_ErrorForFile(paramsJson, "Param '%s' has invalid value.", errParam);
            }
        }
        
        return Invalid_Trie;
    }

    if (typeObject[ModuleType_Events][Module_OnRead] >= 0) {
        new iRet = VIPM_STOP;
        ExecuteForward(typeObject[ModuleType_Events][Module_OnRead], iRet, paramsJson, params);

        if (iRet == VIPM_STOP) {
            TrieDestroy(params);
            return Invalid_Trie;
        }
    }
    
    return params;
}

ModuleType_AddParams(const T_ModuleType:module, const Array:params) {
    if (params == Invalid_Array) {
        return;
    }

    new Array:currentParams = ModuleType__GetValue(module, ModuleType_Params);
    
    for (new i = 0, ii = ArraySize(params); i < ii; ++i) {
        ArrayPushCell(currentParams, ArrayGetCell(params, i));
    }
}

ModuleType_SetEventListener(
    const T_ModuleType:type,
    const E_ModuleEvent:event,
    const pluginIndex,
    const func[]
) {
    new typeObject[S_ModuleType];
    ModuleType__Get(type, typeObject);

    if (typeObject[ModuleType_Events][event] >= 0) {
        DestroyForward(typeObject[ModuleType_Events][event]);
    }
    
    typeObject[ModuleType_Events][event] = ModuleType__MakeEventForward(event, pluginIndex, func);

    if (typeObject[ModuleType_Events][event] < 0) {
        abort(AMX_ERR_PARAMS, "[ERROR] Can't create forward for func '%s' in plugin #%d.", func, pluginIndex);
        return;
    }

    ModuleType__Save(typeObject);
}

static ModuleType__MakeEventForward(const E_ModuleEvent:event, const pluginIndex, const func[]) {
    switch (event) {
        case Module_OnRead: // (const JSON:jCfg, Trie:Params)
            return CreateOneForward(pluginIndex, func, FP_CELL, FP_CELL);

        case Module_OnActivated: // ()
            return CreateOneForward(pluginIndex, func);

        case Module_OnMergeParams: // (const Trie:tParams1, const Trie:tParams2)
            return CreateOneForward(pluginIndex, func, FP_CELL, FP_CELL);
    }

    abort(AMX_ERR_GENERAL, "[ERROR] Invalid event index (%d).", event);
    return -1;
}

ModuleType_MarkAsUsed(const T_ModuleType:type, const bool:bState = true) {
    ModuleType__SetValue(type, ModuleType_Used, bState);
}

bool:ModuleType_Activate(const T_ModuleType:type) {
    new ModuleType[S_ModuleType];
    ModuleType__Get(type, ModuleType);

    if (!ModuleType[ModuleType_Active]) {
        Forwards_DefaultReturn(VIPM_CONTINUE);
        if (Forwards_CallP("VipM_Modules_OnActivate", ModuleType[ModuleType_Name]) != VIPM_CONTINUE) {
            return false;
        }

        ModuleType[ModuleType_Active] = true;
        
        if (ModuleType[ModuleType_Events][Module_OnActivated] >= 0) {
            ExecuteForward(ModuleType[ModuleType_Events][Module_OnActivated]);
        }

        ModuleType__Save(ModuleType);
    }

    return ModuleType[ModuleType_Active];
}

ModuleType_ActivateUsed() {
    ArrayMapForeachArray (ModuleTypes: type => ModuleType[S_ModuleType]) {
        if (ModuleType[ModuleType_Used]) {
            ModuleType_Activate(type);
        }
    }
}

Trie:ModuleType_MergeParams(const T_ModuleType:type, const Trie:tParams1, const Trie:tParams2) {
    new ModuleType[S_ModuleType];
    ModuleType__Get(type, ModuleType);

    new Trie:tRes = tParams1;
    if (ModuleType[ModuleType_Events][Module_OnMergeParams] >= 0) {
        ExecuteForward(ModuleType[ModuleType_Events][Module_OnMergeParams], _:tRes, tParams1, tParams2);

        if (tRes != tParams1 && tRes != tParams2) {
            TrieSetCell(tRes, VIPM_MODULES_PARAMS_TEMP_MARK_KEY, true);
        }
    }
    
    return tRes;
}

T_ModuleType:PCSingle_ObjVipmModuleType(const JSON:objectJson, const key[], const T_ModuleType:def = Invalid_ModuleType, const bool:dotNot = false, const bool:orFail = false) {
    return PCSingle_ObjCell(objectJson, key, VIPM_PARAM_TYPE_MODULE_TYPE_NAME, def, dotNot, orFail);
}
