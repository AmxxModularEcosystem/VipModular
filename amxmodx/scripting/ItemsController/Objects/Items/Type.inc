#if defined __ic__objects__items_type__included
    #endinput
#endif
#define __ic__objects__items_type__included

#include <amxmodx>
#include <ItemsController>
#include "VipM/Forwards"
#include "VipM/Utils"

enum _:S_ItemType {
    ItemType_Name[IC_ITEM_TYPE_NAME_MAX_LEN],
    ItemType_Events[E_ItemTypeEvent],
    Array:ItemType_Params,
}

static Array:ItemTypes = Invalid_Array;
static Trie:ItemTypesMap = Invalid_Trie;

ItemType_Init() {
    CallOnce();
    log_amx("Init item types...");

    ItemTypes = ArrayCreate(S_ItemType, 1);
    ItemTypesMap = TrieCreate();
    
    Forwards_RegAndCall("IC_ItemType_OnInited", ET_IGNORE);
    log_amx("Item types inited.");
}

T_IC_ItemType:ItemType_Construct(const name[]) {
    if (ItemTypes == Invalid_Array) {
        abort(AMX_ERR_GENERAL, "Attempt to create item type before items controller init.");
        return Invalid_IC_ItemType;
    }

    if (ItemType_Find(name) != Invalid_IC_ItemType) {
        abort(AMX_ERR_PARAMS, "Item type '%s' already exists.", name);
        return Invalid_IC_ItemType;
    }

    new typeObject[S_ItemType];
    copy(typeObject[ItemType_Name], charsmax(typeObject[ItemType_Name]), name);
    arrayset(typeObject[ItemType_Events], INVALID_HANDLE, sizeof(typeObject[ItemType_Events]));
    typeObject[ItemType_Params] = ArrayCreate(1, 1);
    
    new T_IC_ItemType:type = T_IC_ItemType:ArrayPushArray(ItemTypes, typeObject);
    TrieSetCell(ItemTypesMap, typeObject[ItemType_Name], type);
    
    log_amx("Item type '%s' registered.", typeObject[ItemType_Name]);

    return type;
}

T_IC_ItemType:ItemType_UpdateObject(const typeObject[S_ItemType]) {
    new T_IC_ItemType:type = ItemType_Find(typeObject[ItemType_Name], .orFail = true);
    ArraySetArray(ItemTypes, _:type, typeObject);
    return type;
}

T_IC_ItemType:ItemType_Find(const name[], const bool:orFail = false) {
    new T_IC_ItemType:type = Invalid_IC_ItemType;
    TrieGetCell(ItemTypesMap, name, type);
    
    if (orFail && type == Invalid_IC_ItemType) {
        abort(AMX_ERR_NOTFOUND, "Invalid item type '%s'.", name);
    }

    return type;
}

bool:ItemType_Get(const T_IC_ItemType:type, typeObject[S_ItemType], const bool:orFail = true) {
    if (_:type < 0 || _:type >= ArraySize(ItemTypes)) {
        if (orFail) {
            abort(AMX_ERR_NOTFOUND, "Invalid item type handler (%d, out of bounds).", type);
        }
        return false;
    }

    ArrayGetArray(ItemTypes, _:type, typeObject);
    return true;
}

ItemType_SetEventListener(const T_IC_ItemType:type, const E_ItemTypeEvent:event, const listener) {
    new typeObject[S_ItemType];
    ItemType_Get(type, typeObject);

    if (typeObject[ItemType_Events][event] >= 0) {
        abort(AMX_ERR_GENERAL, "Item type '%s' already has listener for event #%d.", typeObject[ItemType_Name], event);
        return;
    }

    typeObject[ItemType_Events][event] = listener;
    ItemType_UpdateObject(typeObject);
}

ItemType_MakeEventListener(const E_ItemTypeEvent:event, const pluginIndex, const functionName[]) {
    new listener = INVALID_HANDLE;
    switch (event) {
        case ItemType_OnRead: // IC_RET_READ_*:(const JSON:itemJson, Trie:params)
            listener = CreateOneForward(pluginIndex, functionName, FP_CELL, FP_CELL);

        case ItemType_OnGive: // IC_RET_GIVE_*:(const playerIndex, const Trie:params)
            listener = CreateOneForward(pluginIndex, functionName, FP_CELL, FP_CELL);
    }

    if (listener == INVALID_HANDLE) {
        abort(AMX_ERR_PARAMS, "Can't create forward for func '%s' in plugin #%d.", functionName, pluginIndex);
    }

    return listener;
}

ItemType_AddParams(const T_IC_ItemType:type, const Array:params) {
    new typeObject[S_ItemType];
    ItemType_Get(type, typeObject);

    for (new i = 0, ii = ArraySize(params); i < ii; ++i) {
        ArrayPushCell(typeObject[ItemType_Params], ArrayGetCell(params, i));
    }
}

bool:ItemType_Give(const playerIndex, const T_IC_ItemType:type, const Trie:params) {
    new typeObject[S_ItemType];
    ItemType_Get(type, typeObject);

    if (typeObject[ItemType_Events][ItemType_OnGive] == INVALID_HANDLE) {
        return true;
    }

    new ret = IC_RET_GIVE_FAIL;
    ExecuteForward(typeObject[ItemType_Events][ItemType_OnGive], ret, playerIndex, params);

    return ret == IC_RET_GIVE_SUCCESS;
}

bool:ItemType_ReadParamsFromJsonObject(const T_IC_ItemType:type, const JSON:paramsJson, &Trie:params) {
    new typeObject[S_ItemType];
    ItemType_Get(type, typeObject);

    new errField[PARAM_KEY_MAX_LEN], E_ParamsReadErrorType:errType;
    ParamsController_Param_ReadList(typeObject[ItemType_Params], paramsJson, params, errType, errField, charsmax(errField));
    if (errType != ParamsReadError_None) {
        switch (errType) {
            case ParamsReadError_RequiredParamNotPresented:
                PCJson_ErrorForFile(paramsJson, "Param '%s' is required for item type '%s'.", errField, typeObject[ItemType_Name]);
            case ParamsReadError_ParamValueIsInvalid:
                PCJson_ErrorForFile(paramsJson, "Param '%s' has invalid value.", errField);
        }
        return false;
    }

    // deprecated
    if (!TrieKeyExists(params, "Name")) {
        new nameParam[256];
        json_object_get_string(paramsJson, "Name", nameParam, charsmax(nameParam));
        TrieSetString(params, "Name", nameParam);
    }

    // deprecated?
    if (typeObject[ItemType_Events][ItemType_OnRead] != INVALID_HANDLE) {
        new ret = IC_RET_READ_SUCCESS;
        ExecuteForward(typeObject[ItemType_Events][ItemType_OnRead], ret, paramsJson, params);

        return ret == IC_RET_READ_SUCCESS;
    }

    return true;
}