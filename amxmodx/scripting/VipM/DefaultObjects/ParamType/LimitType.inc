#include <amxmodx>
#include <json>
#include <ParamsController>
#include <VipModular>

#include "VipM/Core/Objects/Limits/Type"

DefaultObjects_ParamType_LimitType_Register() {
    ParamsController_RegSimpleType(VIPM_PARAM_TYPE_LIMIT_TYPE_NAME, "@DefaultObjects_ParamType_LimitType_OnRead");
}

bool:@DefaultObjects_ParamType_LimitType_OnRead(const JSON:valueJson) {
    new typeName[VIPM_LIMITS_TYPE_NAME_MAX_LEN];
    PCSingle_ShortString(valueJson, typeName, charsmax(typeName));
    trim(typeName);

    if (typeName[0] == EOS) {
        return PCJson_LogForFile(valueJson, "WARNING", "Limit type name not specified.");
    }

    new T_LimitType:type = LimitType_Find(typeName);
    if (type == Invalid_LimitType) {
        return PCJson_LogForFile(valueJson, "WARNING", "Limit type '%s' not found.", typeName);
    }

    return ParamsController_SetCell(type);
}
