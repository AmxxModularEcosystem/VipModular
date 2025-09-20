#include <amxmodx>
#include <ParamsController>
#include <VipModular>
#include "VipM/Utils"

DefaultObjects_Limit_WeekDay_Register() {
    VipM_Limits_RegisterType("WeekDay", false, false);
    VipM_Limits_AddTypeParams("WeekDay",
        "Day", ptString, true // TODO: Добавить тип "WeekDayNum" в контроллер параметров
    );
    VipM_Limits_RegisterTypeEvent("WeekDay", Limit_OnRead, "@DefaultObjects_Limit_WeekDay_OnRead");
    VipM_Limits_RegisterTypeEvent("WeekDay", Limit_OnCheck, "@DefaultObjects_Limit_WeekDay_OnCheck");
}

@DefaultObjects_Limit_WeekDay_OnRead(const JSON:valueJson, const Trie:p) {
    new sWeekDayName[32];
    json_object_get_string(valueJson, "Day", sWeekDayName, charsmax(sWeekDayName));

    new iWeekDayIndex = GetWeekDayIdByName(sWeekDayName);
    if (iWeekDayIndex < 0) {
        PCJson_LogForFile(valueJson, "WARNING", "Undefined week day '%s'.", sWeekDayName);
        return VIPM_STOP;
    }

    TrieSetCell(p, "Day", iWeekDayIndex);
    return VIPM_CONTINUE;
}

@DefaultObjects_Limit_WeekDay_OnCheck(const Trie:p) {
    new sWeekDay[4];
    get_time("%w", sWeekDay, charsmax(sWeekDay));
    return str_to_num(sWeekDay) == PCGet_Int(p, "Day", -1);
}
