#include <amxmodx>
#include <ParamsController>
#include <VipModular>

DefaultObjects_Limit_GameTime_Register() {
    VipM_Limits_RegisterType("GameTime", true, false);
    VipM_Limits_AddTypeParams("GameTime",
        "Min", DEFAULT_PARAMS_TIME_INTERVAL_NAME, false, // TODO: Добавить типы "Time" в контроллер параметров
        "Max", DEFAULT_PARAMS_TIME_INTERVAL_NAME, false
    );
    VipM_Limits_RegisterTypeEvent("GameTime", Limit_OnCheck, "@DefaultObjects_Limit_GameTime_OnCheck");
}


@DefaultObjects_Limit_GameTime_OnCheck(const Trie:p) {
    return (
        gameTime >= PCGet_Int(p, "Min", cellmin)
        && gameTime <= PCGet_Int(p, "Max", cellmax)
    );
}
