#include <amxmodx>
#include <json>
#include <ParamsController>
#include <ItemsController>

DefaultObjects_ParamType_Item_Register() {
    ParamsController_RegSimpleType(IC_PARAM_TYPE_ITEM_NAME, "@OnItemParamTypeRead");
}

bool:@OnItemParamTypeRead(const JSON:valueJson) {
    new T_IC_Item:item = IC_Item_ReadFromJson(valueJson);

    if (item == Invalid_IC_Item) {
        return false;
    }

    return ParamsController_SetCell(item);
}