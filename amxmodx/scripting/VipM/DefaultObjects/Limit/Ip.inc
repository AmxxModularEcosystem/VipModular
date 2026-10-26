#include <amxmodx>
#include <ParamsController>
#include <VipModular>

static PlayerIps[MAX_PLAYERS + 1][MAX_IP_LENGTH];

DefaultObjects_Limit_Ip_Register() {
    VipM_Limits_RegisterType("Ip", true, false);
    VipM_Limits_AddParamsEx("Ip",
        PCParam("Ip", DEFAULT_PARAMS_SHORT_STR_NAME, true)
    );
    VipM_Limits_RegisterTypeEvent("Ip", Limit_OnCheck, "@DefaultObjects_Limit_Ip_OnCheck");
}

DefaultObjects_Limit_Ip_OnClientAuth(const playerIndex) {
    get_user_ip(playerIndex, PlayerIps[playerIndex], charsmax(PlayerIps[]), true);
}

@DefaultObjects_Limit_Ip_OnCheck(const Trie:p, const playerIndex) {
    static ip[MAX_IP_LENGTH];
    PCGet_Str(p, "SteamId", ip, charsmax(ip));

    return equali(PlayerIps[playerIndex], ip);
}
