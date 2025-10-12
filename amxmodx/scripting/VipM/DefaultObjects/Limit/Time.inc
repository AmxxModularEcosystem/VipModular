#include <amxmodx>
#include <ParamsController>
#include <VipModular>
#include "VipM/Utils"

DefaultObjects_Limit_Time_Register() {
    VipM_Limits_RegisterType("Time", false, false);
    VipM_Limits_AddTypeParams("Time",
        "Before", DEFAULT_PARAMS_TIME_NAME, false,
        "After", DEFAULT_PARAMS_TIME_NAME, false
    );
    VipM_Limits_RegisterTypeEvent("Time", Limit_OnRead, "@DefaultObjects_Limit_Time_OnRead");
    VipM_Limits_RegisterTypeEvent("Time", Limit_OnCheck, "@DefaultObjects_Limit_Time_OnCheck");
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
