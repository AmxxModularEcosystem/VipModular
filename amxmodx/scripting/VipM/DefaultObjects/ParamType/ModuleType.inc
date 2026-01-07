#include <amxmodx>
#include <json>
#include <ParamsController>
#include <VipModular>

#include "VipM/Core/Objects/Modules/Type"

DefaultObjects_ParamType_ModuleType_Register() {
    ParamsController_RegSimpleType(VIPM_PARAM_TYPE_MODULE_TYPE_NAME, "@DefaultObjects_ParamType_ModuleType_OnRead");
}

bool:@DefaultObjects_ParamType_ModuleType_OnRead(const JSON:valueJson) {
    new typeName[VIPM_MODULES_TYPE_NAME_MAX_LEN];
    PCSingle_ShortString(valueJson, typeName, charsmax(typeName));
    trim(typeName);

    if (typeName[0] == EOS) {
        return PCJson_LogForFile(valueJson, "WARNING", "Module type name not specified.");
    }

    new T_ModuleType:type = ModuleType_Find(typeName);
    if (type == Invalid_ModuleType) {
        return PCJson_LogForFile(valueJson, "WARNING", "Module type '%s' not found.", typeName);
    }

    return ParamsController_SetCell(type);
}
