#include <amxmodx>
#include <ParamsController>
#include <VipModular>

DefaultObjects_Limit_RoundTime_Register() {
    VipM_Limits_RegisterType("RoundTime", false, false);
    VipM_Limits_AddParamsEx("RoundTime",
        "Min", DEFAULT_PARAMS_TIME_INTERVAL_NAME, false,
        "Max", DEFAULT_PARAMS_TIME_INTERVAL_NAME, false
    );
    VipM_Limits_RegisterTypeEvent("RoundTime", Limit_OnCheck, "@DefaultObjects_Limit_RoundTime_OnCheck");
}

@DefaultObjects_Limit_RoundTime_OnCheck(const Trie:p) {
    new roundTime = floatround(get_gametime() - Float:get_member_game(m_fRoundStartTime));

    return (
        roundTime >= PCGet_Int(p, "Min", cellmin)
        && roundTime <= PCGet_Int(p, "Max", cellmax)
    );
}
