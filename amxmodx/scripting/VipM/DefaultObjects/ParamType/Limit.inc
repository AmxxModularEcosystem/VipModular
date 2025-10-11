#include <amxmodx>
#include <json>
#include <ParamsController>
#include <VipModular>

DefaultObjects_ParamType_Limit_Register() {
    ParamsController_RegSimpleType(VIPM_PARAM_TYPE_LIMIT_NAME, "@DefaultObjects_ParamType_Limit_OnRead");
}

bool:@DefaultObjects_ParamType_Limit_OnRead(const JSON:valueJson) {
    new T_LimitUnit:limit = VipM_Limits_ReadFromJson(valueJson);

    if (limit == Invalid_LimitUnit) {
        return false;
    }

    return ParamsController_SetCell(limit);
}
