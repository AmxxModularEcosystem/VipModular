#include <amxmodx>
#include <ItemsController>
#include <ParamsController>

#include "ItemsController/Objects/Items/Type"

API_ItemType_Register() {
    register_native("IC_ItemType_Register", "@_ItemType_Register");
    register_native("IC_ItemType_SetEventListener", "@_ItemType_SetEventListener");
    register_native("IC_ItemType_AddParams", "@_ItemType_AddParams");
}

T_IC_ItemType:@_ItemType_Register() {
    enum {Arg_Name = 1}

    new name[IC_ITEM_TYPE_NAME_MAX_LEN];
    get_string(Arg_Name, name, charsmax(name));

    return ItemType_Construct(name);
}

@_ItemType_SetEventListener(const pluginIndex) {
    enum {Arg_Type = 1, Arg_Event, Arg_FuncName}

    new T_IC_ItemType:type = T_IC_ItemType:get_param(Arg_Type);
    new E_ItemTypeEvent:event = E_ItemTypeEvent:get_param(Arg_Event);
    new funcName[IC_EVENT_LISTENER_FUNCTION_NAME_MAX_LEN];
    get_string(Arg_FuncName, funcName, charsmax(funcName));

    new listener = ItemType_MakeEventListener(event, pluginIndex, funcName);
    ItemType_SetEventListener(type, event, listener);
}

@_ItemType_AddParams(const pluginIndex, const paramsCount) {
    enum {Arg_Type = 1, Arg_Params}

    new T_IC_ItemType:type = T_IC_ItemType:get_param(Arg_Type);
    new Array:params = ParamsController_Param_ListFromNativeParams(Arg_Params, paramsCount);

    ItemType_AddParams(type, params);

    ArrayDestroy(params);
}
