#include <amxmodx>
#include <ParamsController>
#include <VipModular>

DefaultObjects_Limit_InFreezyTime_Register() {
    VipM_Limits_RegisterType("InFreezyTime", false, false);
    VipM_Limits_AddParamsEx("InFreezyTime",
        "Reverse", DEFAULT_PARAMS_BOOL_NAME, false
    );
    VipM_Limits_RegisterTypeEvent("InFreezyTime", Limit_OnCheck, "@DefaultObjects_Limit_InFreezyTime_OnCheck");
}

@DefaultObjects_Limit_InFreezyTime_OnCheck(const Trie:p) {
    new bool:bFreezyPeriod = get_member_game(m_bFreezePeriod);
    return PCGet_Bool(p, "Reverse", false) ? !bFreezyPeriod : bFreezyPeriod;
}
