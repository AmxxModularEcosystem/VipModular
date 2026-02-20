#include <amxmodx>
#include <ParamsController>
#include <VipModular>

DefaultObjects_Limit_Time_Register() {
    VipM_Limits_RegisterType("Time", false, false);
    VipM_Limits_AddParamsEx("Time",
        "Before", DEFAULT_PARAMS_TIME_NAME, false,
        "After", DEFAULT_PARAMS_TIME_NAME, false
    );
    VipM_Limits_RegisterTypeEvent("Time", Limit_OnCheck, "@DefaultObjects_Limit_Time_OnCheck");
}

@DefaultObjects_Limit_Time_OnCheck(const Trie:p) {
    new before = PCGet_Int(p, "Before", 0);
    new after = PCGet_Int(p, "After", 0);

    if (!before && !after) {
        return true;
    }
    
    new currentTime = get_systime() % (24 * 60 * 60);

    if (!before) {
        return currentTime > after;
    }

    if (!after) {
        return currentTime < before;
    }

    if (before == after) {
        return before == currentTime;
    }

    if (after < before) {
        return (
            currentTime > after
            && currentTime < before
        );
    }

    if (after > before) {
        return (
            currentTime > after
            || currentTime < before
        );
    }
    
    return false;
}
