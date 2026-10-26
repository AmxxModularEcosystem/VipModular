#include <amxmodx>
#include <ParamsController>
#include <VipModular>

DefaultObjects_Limit_GameTime_Register() {
    VipM_Limits_RegisterType("GameTime", false, false);
    VipM_Limits_AddParamsEx("GameTime",
        PCParam("Min", DEFAULT_PARAMS_TIME_INTERVAL_NAME),
        PCParam("Max", DEFAULT_PARAMS_TIME_INTERVAL_NAME)
    );
    VipM_Limits_RegisterTypeEvent("GameTime", Limit_OnCheck, "@DefaultObjects_Limit_GameTime_OnCheck");
}


@DefaultObjects_Limit_GameTime_OnCheck(const Trie:p) {
    new gameTime = floatround(get_gametime());

    return (
        gameTime >= PCGet_Int(p, "Min", cellmin)
        && gameTime <= PCGet_Int(p, "Max", cellmax)
    );
}
