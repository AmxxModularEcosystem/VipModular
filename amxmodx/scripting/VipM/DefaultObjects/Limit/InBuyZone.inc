#include <amxmodx>
#include <ParamsController>
#include <VipModular>

#include "VipM/Utils"

DefaultObjects_Limit_InBuyZone_Register() {
    VipM_Limits_RegisterType("InBuyZone", true, false);
    VipM_Limits_AddParamsEx("InBuyZone",
        "Reverse", DEFAULT_PARAMS_BOOL_NAME, false
    );
    VipM_Limits_RegisterTypeEvent("InBuyZone", Limit_OnCheck, "@DefaultObjects_Limit_InBuyZone_OnCheck");
}

@DefaultObjects_Limit_InBuyZone_OnCheck(const Trie:p, const playerIndex) {
    new bool:res = IsUserInBuyZone(playerIndex);

    return PCGet_Bool(p, "Reverse", false) ? !res : res;
}
