#include <amxmodx>
#include <reapi>
#include <ParamsController>
#include <VipModular>

DefaultObjects_Limit_Round_Register() {
    VipM_Limits_RegisterType("Round", false, false);
    VipM_Limits_AddParamsEx("Round",
        "Min", DEFAULT_PARAMS_INT_NAME, false,
        "Max", DEFAULT_PARAMS_INT_NAME, false
    );
    VipM_Limits_RegisterTypeEvent("Round", Limit_OnCheck, "@DefaultObjects_Limit_Round_OnCheck");
}

@DefaultObjects_Limit_Round_OnCheck(const Trie:Params, const UserId) {
    new currentRound = get_member_game(m_iTotalRoundsPlayed) + 1;

    return (
        currentRound >= PCGet_Int(Params, "Min", cellmin)
        && currentRound <= PCGet_Int(Params, "Max", cellmax)
    );
}
