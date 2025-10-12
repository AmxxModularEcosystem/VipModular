#include <amxmodx>
#include <ParamsController>
#include <VipModular>
#include "VipM/Utils"

DefaultObjects_Limit_Name_Register() {
    VipM_Limits_RegisterType("Name", true, false);
    VipM_Limits_AddParamsEx("Name",
        "Name", DEFAULT_PARAMS_SHORT_STR_NAME, true
    );
    VipM_Limits_RegisterTypeEvent("Name", Limit_OnCheck, "@DefaultObjects_Limit_Name_OnCheck");
}

@DefaultObjects_Limit_Name_OnCheck(const Trie:p, const playerIndex) {
    static name[MAX_NAME_LENGTH];
    get_user_name(playerIndex, name, charsmax(name));

    return equal(name, PCGet_iStr(p, "Name"));
}
