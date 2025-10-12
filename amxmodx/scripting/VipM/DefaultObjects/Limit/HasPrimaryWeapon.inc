#include <amxmodx>
#include <ParamsController>
#include <VipModular>

DefaultObjects_Limit_HasPrimaryWeapon_Register() {
    VipM_Limits_RegisterType("HasPrimaryWeapon", true, false);
    VipM_Limits_AddParamsEx("HasPrimaryWeapon",
        "HasNot", DEFAULT_PARAMS_BOOL_NAME, false
    );
    VipM_Limits_RegisterTypeEvent("HasPrimaryWeapon", Limit_OnCheck, "@DefaultObjects_Limit_HasPrimaryWeapon_OnCheck");
}

@DefaultObjects_Limit_HasPrimaryWeapon_OnCheck(const Trie:p, const playerIndex) {
    new bool:res = get_member(playerIndex, m_bHasPrimary);

    return PCGet_Bool(p, "HasNot", false) ? !res : res;
}
