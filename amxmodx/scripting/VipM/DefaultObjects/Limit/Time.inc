#include <amxmodx>
#include <ParamsController>
#include <VipModular>
#include "VipM/Utils"

DefaultObjects_Limit_Time_Register() {
    VipM_Limits_RegisterType("Time", false, false);
    VipM_Limits_AddTypeParams("Time",
        "Before", ptString, false, // TODO: Добавить тип "DayTime" в контроллер параметров
        "After", ptString, false
    );
    VipM_Limits_RegisterTypeEvent("Time", Limit_OnRead, "@DefaultObjects_Limit_Time_OnRead");
    VipM_Limits_RegisterTypeEvent("Time", Limit_OnCheck, "@DefaultObjects_Limit_Time_OnCheck");
}

@DefaultObjects_Limit_Time_OnRead(const JSON:valueJson, const Trie:p) {
    new sTime[8];
    
    TrieGetString(p, "Before", sTime, charsmax(sTime));
    TrieSetCell(p, "Before", ParseColonTime(sTime), .replace = true);

    TrieGetString(p, "After", sTime, charsmax(sTime));
    TrieSetCell(p, "After", ParseColonTime(sTime), .replace = true);

    return VIPM_CONTINUE;
}

@DefaultObjects_Limit_Time_OnCheck(const Trie:p) {
    new iBefore = PCGet_Int(p, "Before", 0);
    new iAfter = PCGet_Int(p, "After", 0);
    new iCurrent = GetDayTimeInSeconds();

    Dbg_Log("@OnTimeCheck(%d):", p);
    Dbg_Log("  iBefore = %d", iBefore);
    Dbg_Log("  iAfter = %d", iAfter);
    Dbg_Log("  iCurrent = %d", iCurrent);

    if (!iBefore && !iAfter) {
        return true;
    }

    if (!iBefore) {
        return iCurrent > iAfter;
    }

    if (!iAfter) {
        return iCurrent < iBefore;
    }

    if (iBefore == iAfter) {
        return iBefore == iCurrent;
    }

    if (iAfter < iBefore) {
        return (
            iCurrent > iAfter
            && iCurrent < iBefore
        );
    }

    if (iAfter > iBefore) {
        return (
            iCurrent > iAfter
            || iCurrent < iBefore
        );
    }
    
    return false;
}
