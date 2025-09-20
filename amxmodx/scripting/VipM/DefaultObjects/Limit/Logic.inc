#include <amxmodx>
#include <VipModular>

DefaultObjects_Limit_Logic_Register() {
    VipM_Limits_RegisterType("Logic-OR", false, false);
    VipM_Limits_AddParamsEx("Logic-OR",
        "Limits", VIPM_PARAM_TYPE_LIMITS_NAME, true
    );
    VipM_Limits_RegisterTypeEvent("Logic-OR", Limit_OnCheck, "@DefaultObjects_Limit_Logic_OnOrCheck");

    VipM_Limits_RegisterType("Logic-XOR", false, false);
    VipM_Limits_AddParamsEx("Logic-XOR",
        "Limits", VIPM_PARAM_TYPE_LIMITS_NAME, true
    );
    VipM_Limits_RegisterTypeEvent("Logic-XOR", Limit_OnCheck, "@DefaultObjects_Limit_Logic_OnXorCheck");

    VipM_Limits_RegisterType("Logic-AND", false, false);
    VipM_Limits_AddParamsEx("Logic-AND",
        "Limits", VIPM_PARAM_TYPE_LIMITS_NAME, true
    );
    VipM_Limits_RegisterTypeEvent("Logic-AND", Limit_OnCheck, "@DefaultObjects_Limit_Logic_OnAndCheck");

    VipM_Limits_RegisterType("Logic-NOT", false, false);
    VipM_Limits_AddParamsEx("Logic-NOT",
        "Limits", VIPM_PARAM_TYPE_LIMITS_NAME, true
    );
    VipM_Limits_RegisterTypeEvent("Logic-NOT", Limit_OnCheck, "@DefaultObjects_Limit_Logic_OnNotCheck");
}

@DefaultObjects_Limit_Logic_OnOrCheck(const Trie:p, const playerIndex){
    return PCGet_VipmLimitsCheck(p, "Limits", playerIndex, Limit_Exec_OR);
}

@DefaultObjects_Limit_Logic_OnXorCheck(const Trie:p, const playerIndex){
    return PCGet_VipmLimitsCheck(p, "Limits", playerIndex, Limit_Exec_XOR);
}

@DefaultObjects_Limit_Logic_OnAndCheck(const Trie:p, const playerIndex){
    return PCGet_VipmLimitsCheck(p, "Limits", playerIndex, Limit_Exec_AND);
}

@DefaultObjects_Limit_Logic_OnNotCheck(const Trie:p, const playerIndex){
    return !PCGet_VipmLimitsCheck(p, "Limits", playerIndex, Limit_Exec_AND);
}
