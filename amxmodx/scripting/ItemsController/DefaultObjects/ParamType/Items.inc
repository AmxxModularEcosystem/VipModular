#include <amxmodx>
#include <json>
#include <ParamsController>
#include <ItemsController>

DefaultObjects_ParamType_Items_Register() {
    ParamsController_RegSimpleType(IC_PARAM_TYPE_ITEMS_NAME, "@OnItemsParamTypeRead");
}

@OnItemsParamTypeRead(const JSON:valueJson) {
    new Array:items = IC_Item_ReadArrayFromJson(valueJson);

    if (items == Invalid_Array) {
        return false;
    }

    if (
        (
            json_is_object(valueJson)
            || (json_is_array(valueJson) && json_array_get_count(valueJson) == 0)
        ) && ArraySize(items) < 1
    ) {
        PCJson_LogForFile(valueJson, "WARNING", "All items are invalid.");
        return false;
    }

    return ParamsController_SetCell(items);
}