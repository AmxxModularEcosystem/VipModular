#include <amxmodx>
#include <json>
#include <ParamsController>
#include <VipModular>

DefaultObjects_ParamType_Limits_Register() {
    ParamsController_RegSimpleType(VIPM_PARAM_TYPE_LIMITS_NAME, "@DefaultObjects_ParamType_Limits_OnRead");
}

@DefaultObjects_ParamType_Limits_OnRead(const JSON:valueJson) {
    if (json_is_array(valueJson) && json_array_get_count(valueJson) < 1) {
        PCJson_LogForFile(valueJson, "WARNING", "Limits array is empty.");
        return false;
    }

    new Array:limits = VipM_Limits_ReadListFromJson(valueJson);
    if (limits == Invalid_Array || ArraySize(limits) < 1) {
        PCJson_LogForFile(valueJson, "WARNING", "All limits are invalid.");
        ArrayDestroy(limits);
        return false;
    }

    return ParamsController_SetCell(limits);
}
