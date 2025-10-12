#include <amxmodx>
#include <ParamsController>
#include <VipModular>
#include "VipM/Utils"

DefaultObjects_Limit_WeekDay_Register() {
    VipM_Limits_RegisterType("WeekDay", false, false);
    VipM_Limits_AddTypeParams("WeekDay",
        "Day", DEFAULT_PARAMS_WEEK_DAY_NAME, true
    );
    VipM_Limits_RegisterTypeEvent("WeekDay", Limit_OnRead, "@DefaultObjects_Limit_WeekDay_OnRead");
    VipM_Limits_RegisterTypeEvent("WeekDay", Limit_OnCheck, "@DefaultObjects_Limit_WeekDay_OnCheck");
}

@DefaultObjects_Limit_WeekDay_OnCheck(const Trie:p) {
    new sWeekDay[4];
    get_time("%w", sWeekDay, charsmax(sWeekDay));
    
    return str_to_num(sWeekDay) == PCGet_Int(p, "Day", -1);
}
