#include <amxmodx>
#include <json>
#include <ItemsController>

#include "ItemsController/Objects/Items/Type"
#include "ItemsController/Objects/Items/Instance"

// API для совместимости со старой версией
// Все зарегистрированные тут нативы устаревшие.

API_Compat_Register() {
    register_native("VipM_IC_RegisterType", "@_VipM_IC_RegisterType");
    register_native("VipM_IC_RegisterTypeEvent", "@_VipM_IC_RegisterTypeEvent");
    register_native("VipM_IC_JsonGetItem", "@_VipM_IC_JsonGetItem");
    register_native("VipM_IC_GiveItem", "@_VipM_IC_GiveItem");
}

T_IC_ItemType:@_VipM_IC_RegisterType() {
    enum {Arg_Name = 1}

    new name[IC_ITEM_TYPE_NAME_MAX_LEN];
    get_string(Arg_Name, name, charsmax(name));

    return ItemType_Construct(name);
}

@_VipM_IC_RegisterTypeEvent(const pluginIndex) {
    enum {Arg_Type = 1, Arg_Event, Arg_FuncName}

    new typeName[IC_ITEM_TYPE_NAME_MAX_LEN];
    get_string(Arg_Type, typeName, charsmax(typeName));
    new T_IC_ItemType:type = ItemType_Find(typeName, .orFail = true);

    new E_ItemTypeEvent:event = E_ItemTypeEvent:get_param(Arg_Event);
    new funcName[IC_EVENT_LISTENER_FUNCTION_NAME_MAX_LEN];
    get_string(Arg_FuncName, funcName, charsmax(funcName));

    new listener = ItemType_MakeEventListener(event, pluginIndex, funcName);
    ItemType_SetEventListener(type, event, listener);
}

T_IC_Item:@_VipM_IC_JsonGetItem() {
    enum {Arg_InstanceJson = 1}

    new JSON:isntanceJson = JSON:get_param_byref(Arg_InstanceJson);

    new T_IC_Item:item = ItemInstance_ReadFromJsonObject(isntanceJson);
    json_free(isntanceJson);
    return item;
}


bool:@_VipM_IC_GiveItem() {
    enum {Arg_PlayerIndex = 1, Arg_Item}

    new playerIndex = get_param(Arg_PlayerIndex);
    new T_IC_Item:item = T_IC_Item:get_param(Arg_Item);

    return ItemInstance_Give(playerIndex, item);
}
