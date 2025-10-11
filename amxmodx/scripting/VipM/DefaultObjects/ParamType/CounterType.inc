#include <amxmodx>
#include <json>
#include <ParamsController>
#include <VipModular>
#include <VipM/L/Counter>

DefaultObjects_ParamType_CounterType_Register() {
    ParamsController_RegSimpleType(VIPM_L_COUNTER_PARAM_TYPE, "@DefaultObjects_ParamType_CounterType_OnRead");
}

bool:@DefaultObjects_ParamType_CounterType_OnRead(const JSON:valueJson) {
    new str[VIPM_L_COUNTER_KEY_MAX_LEN];
    json_get_string(valueJson, str, charsmax(str));

    if (equali(str, "Round") || equali(str, "PerRound")) {
        return ParamsController_SetCell(VipM_L_Counter_PerRound);
    } else if (equali(str, "Life") || equali(str, "PerLife")) {
        return ParamsController_SetCell(VipM_L_Counter_PerLife);
    } else if (equali(str, "Session") || equali(str, "PerSession")) {
        return ParamsController_SetCell(VipM_L_Counter_PerSession);
    } else if (equali(str, "Game") || equali(str, "PerGame")) {
        return ParamsController_SetCell(VipM_L_Counter_PerGame);
    } else if (equali(str, "Map") || equali(str, "PerMap")) {
        return ParamsController_SetCell(VipM_L_Counter_PerMap);
    }

    PCJson_LogForFile(valueJson, "WARNING", "Counter type `%s` is not defined.", str);

    return false;
}
