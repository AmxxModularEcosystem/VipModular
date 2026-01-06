#if defined __vipm_core_objects_limits_type_included
    #endinput
#endif
#define __vipm_core_objects_limits_type_included

#include <amxmodx>
#include <ParamsController>
#include <VipM/Limits>
#include <VipM/Params>
#include "VipM/Utils"
#include "VipM/ArrayMap"
#include "VipM/ArrayTrieUtils"
#include "VipM/Core/Objects/Param"

enum _:S_LimitType {
    LimitType_Name[VIPM_LIMITS_TYPE_NAME_MAX_LEN],
    Array:LimitType_Params,

    bool:LimitType_Static,
    LimitType_StaticValue,

    bool:LimitType_ForPlayer,

    LimitType_Events[E_LimitEvent],
}

enum T_LimitType { Invalid_LimitType = -1 }

static ArrayMap(LimitTypes); // S_LimitType

LimitType_Init() {
    CallOnce();
    log_amx("Init limit types...");

    ArrayMapCreate(LimitTypes, S_LimitType);
    
    log_amx("Limit types inited.");
}

Array:LimitType_GetAll() {
    return ArrayClone(LimitTypes[AM_Arr]);
}

LimitType_GetCount() {
    return ArraySizeSafe(LimitTypes[AM_Arr]);
}

T_LimitType:LimitType_Construct(const sName[], const bool:bForPlayer = false, const bool:bStatic = false) {
    if (!ArrayMapCreated(LimitTypes)) {
        abort(AMX_ERR_GENERAL, "Attempt to create limit type before limits controller init.");
        return Invalid_LimitType;
    }

    if (LimitType_Find(sName) != Invalid_LimitType) {
        abort(AMX_ERR_PARAMS, "Limit type '%s' already exists.", sName);
        return Invalid_LimitType;
    }

    new typeObject[S_LimitType];

    copy(typeObject[LimitType_Name], charsmax(typeObject[LimitType_Name]), sName);
    typeObject[LimitType_Params] = ArrayCreate(1, 1);

    typeObject[LimitType_Static] = bStatic;
    typeObject[LimitType_StaticValue] = 0;

    typeObject[LimitType_ForPlayer] = bForPlayer;

    arrayset(typeObject[LimitType_Events], -1, sizeof(typeObject[LimitType_Events]));
    
    log_amx("Limit type '%s' registered.", typeObject[LimitType_Name]);

    return T_LimitType:ArrayMapPushArray(LimitTypes, typeObject, typeObject[LimitType_Name]);
}

T_LimitType:LimitType_Find(const sName[]) {
    if (ArrayMapKeyExists(LimitTypes, sName)) {
        return T_LimitType:ArrayMapGetIndex(LimitTypes, sName);
    } else {
        return Invalid_LimitType;
    }
}

static LimitType__Get(const T_LimitType:type, typeObject[S_LimitType]) {
    ArrayMapGetArray(LimitTypes, _:type, typeObject);
}

static LimitType__Set(const typeObject[S_LimitType]) {
    ArrayMapSetArrayByKey(LimitTypes, typeObject[LimitType_Name], typeObject);
}

LimitType_AddParams(const T_LimitType:type, const Array:params) {
    if (params == Invalid_Array) {
        return;
    }

    new typeObject[S_LimitType];
    LimitType__Get(type, typeObject);
    
    for (new i = 0, ii = ArraySize(params); i < ii; ++i) {
        ArrayPushCell(typeObject[LimitType_Params], ArrayGetCell(params, i));
    }
}

Trie:LimitType_ReadParams(const T_LimitType:type, const JSON:paramsJson) {
    new typeObject[S_LimitType];
    LimitType__Get(type, typeObject);

    // У статических лимитов не может быть параметров
    if (typeObject[LimitType_Static]) {
        return Invalid_Trie;
    }
    
    new E_ParamsReadErrorType:errType, errParam[PARAM_KEY_MAX_LEN];
    new Trie:params = ParamsController_Param_ReadList(
        typeObject[LimitType_Params], paramsJson,
        .iErrType = errType,
        .sErrParamName = errParam,
        .iErrParamNameLen = charsmax(errParam)
    );

    if (errType != ParamsReadError_None) {
        TrieDestroy(params);

        switch (errType) {
            case ParamsReadError_RequiredParamNotPresented: {
                PCJson_ErrorForFile(paramsJson, "Param '%s' required for '%s' limit.", errParam, typeObject[LimitType_Name]);
            }
            case ParamsReadError_ParamValueIsInvalid: {
                PCJson_ErrorForFile(paramsJson, "Param '%s' has invalid value.", errParam);
            }
        }
        
        return Invalid_Trie;
    }

    if (typeObject[LimitType_Events][Limit_OnRead] >= 0) {
        new ret = VIPM_STOP;
        ExecuteForward(typeObject[LimitType_Events][Limit_OnRead], ret, paramsJson, params);

        if (ret == VIPM_STOP) {
            TrieDestroy(params);
            return Invalid_Trie;
        }
    }
    
    return params;
}

LimitType_SetEventListener(
    const T_LimitType:type,
    const E_LimitEvent:iEvent,
    const PluginId,
    const sFuncName[]
) {
    new typeObject[S_LimitType];
    LimitType__Get(type, typeObject);

    if (typeObject[LimitType_Events][iEvent] >= 0) {
        DestroyForward(typeObject[LimitType_Events][iEvent]);
    }
    
    typeObject[LimitType_Events][iEvent] = LimitType__MakeEventForward(iEvent, PluginId, sFuncName);

    if (typeObject[LimitType_Events][iEvent] < 0) {
        abort(AMX_ERR_PARAMS, "[ERROR] Can't create forward for func '%s' in plugin #%d.", sFuncName, PluginId);
    }

    LimitType__Set(typeObject);
}

static LimitType__MakeEventForward(const E_LimitEvent:iEvent, const PluginId, const sFuncName[]) {
    switch (iEvent) {
        case Limit_OnRead: // (const JSON:jCfg, Trie:tParams)
            return CreateOneForward(PluginId, sFuncName, FP_CELL, FP_CELL);
            
        case Limit_OnCheck: // (const Trie:tParams, const UserId)
            return CreateOneForward(PluginId, sFuncName, FP_CELL, FP_CELL);
    }

    abort(AMX_ERR_GENERAL, "[ERROR] Invalid event index (%d).", iEvent);
    return -1;
}

bool:LimitType_IsStatic(const T_LimitType:type) {
    new typeObject[S_LimitType];
    LimitType__Get(type, typeObject);
    
    return typeObject[LimitType_Static];
}

LimitType_SetStaticValue(const T_LimitType:type, const bool:bValue, const UserId = 0) {
    new typeObject[S_LimitType];
    LimitType__Get(type, typeObject);
    
    if (!typeObject[LimitType_Static]) {
        abort(AMX_ERR_PARAMS, "Trying to set static value for a non-static limit.");
        return;
    }

    if (UserId > 0) {
        BitSetIf(typeObject[LimitType_StaticValue], UserId - 1, bValue);
    } else {
        typeObject[LimitType_StaticValue] = bValue;
    }

    LimitType__Set(typeObject);
}

bool:LimitType_Execute(const T_LimitType:type, const Trie:tParams, const UserId = 0) {
    new typeObject[S_LimitType];
    LimitType__Get(type, typeObject);
    
    new bool:bRes = false;
    if (typeObject[LimitType_Static]) {
        if (typeObject[LimitType_ForPlayer]) {
            bRes = BitIs(typeObject[LimitType_StaticValue], UserId - 1);
        } else {
            bRes = !!typeObject[LimitType_StaticValue];
        }
    } else {
        if (typeObject[LimitType_Events][Limit_OnCheck] < 0) {
            abort(AMX_ERR_GENERAL, "'Limit_OnCheck' event listener for non-static limit type '%s' must be implemented.", typeObject[LimitType_Name]);
            bRes = false;
        } else {
            ExecuteForward(typeObject[LimitType_Events][Limit_OnCheck], bRes, tParams, UserId);
        }
    }

    return bRes;
}

T_LimitType:PCSingle_ObjVipmLimitType(const JSON:objectJson, const key[], const T_LimitType:def = Invalid_LimitType, const bool:dotNot = false, const bool:orFail = false) {
    return PCSingle_ObjCell(objectJson, key, VIPM_PARAM_TYPE_LIMIT_TYPE_NAME, def, dotNot, orFail);
}
