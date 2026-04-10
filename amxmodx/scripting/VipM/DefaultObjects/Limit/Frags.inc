#include <amxmodx>
#include <ParamsController>
#include <VipModular>

DefaultObjects_Limit_Frags_Register() {
    VipM_Limits_RegisterType("Frags", true, false);
    VipM_Limits_AddParamsEx("Frags",
        PCParam("Min", DEFAULT_PARAMS_INT_NAME),
        PCParam("Max", DEFAULT_PARAMS_INT_NAME)
    );
    VipM_Limits_RegisterTypeEvent("Frags", Limit_OnCheck, "@DefaultObjects_Limit_Frags_OnCheck");
}

@DefaultObjects_Limit_Frags_OnCheck(const Trie:p, const playerIndex) {
    new frags = get_user_frags(playerIndex);

    return (
        frags >= PCGet_Int(p, "Min", cellmin)
        && frags <= PCGet_Int(p, "Max", cellmax)
    );
}
