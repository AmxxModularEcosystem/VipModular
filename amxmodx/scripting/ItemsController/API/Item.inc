#include <amxmodx>
#include <json>
#include <ItemsController>

#include "ItemsController/Objects/Items/Instance"

API_Item_Register() {
    register_native("IC_Item_ReadFromJson", "@_Item_ReadFromJson");
    register_native("IC_Item_ReadArrayFromJson", "@_Item_ReadArrayFromJson");
    register_native("IC_Item_Give", "@_Item_Give");
    register_native("IC_Item_Free", "@_Item_Free");
}

T_IC_Item:@_Item_ReadFromJson() {
    enum {Arg_InstanceJson = 1}

    new JSON:isntanceJson = JSON:get_param(Arg_InstanceJson);

    return ItemInstance_ReadFromJsonObject(isntanceJson);
}


Array:@_Item_ReadArrayFromJson() {
    enum {Arg_InstancesJson = 1, Arg_Array}

    new JSON:isntancesJson = JSON:get_param(Arg_InstancesJson);
    new Array:array = Array:get_param_byref(Arg_Array);

    array = ItemInstance_ReadArrayFromJsonValue(isntancesJson, array);
    set_param_byref(Arg_Array, _:array);

    return array;
}


bool:@_Item_Give() {
    enum {Arg_PlayerIndex = 1, Arg_Item}

    new playerIndex = get_param(Arg_PlayerIndex);
    new T_IC_Item:item = T_IC_Item:get_param(Arg_Item);

    return ItemInstance_Give(playerIndex, item);
}


T_IC_Item:@_Item_Free() {
    enum {Arg_Item = 1}

    new T_IC_Item:item = T_IC_Item:get_param(Arg_Item);

    return ItemInstance_Free(item);
}
