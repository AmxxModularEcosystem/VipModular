#include <amxmodx>
#include <json>
#include <ParamsController>
#include <VipModular>

#include "VipM/Core/Objects/Limits/Unit"

DefaultObjects_ParamType_Limit_Register() {
    ParamsController_RegSimpleType(VIPM_PARAM_TYPE_LIMIT_NAME, "@DefaultObjects_ParamType_Limit_OnRead");
}

bool:@DefaultObjects_ParamType_Limit_OnRead(const JSON:valueJson) {
    LimitUnit_Init();

    new T_LimitUnit:limit = VipM_Limits_ReadFromJson(valueJson);

    if (limit == Invalid_LimitUnit) {
        return false;
    }

    return ParamsController_SetCell(limit);
}
