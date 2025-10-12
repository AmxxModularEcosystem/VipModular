#include <amxmodx>
#include <ParamsController>
#include <VipModular>

static PlayerSteamIds[MAX_PLAYERS + 1][MAX_AUTHID_LENGTH];

DefaultObjects_Limit_SteamId_Register() {
    VipM_Limits_RegisterType("SteamId", true, false);
    VipM_Limits_AddParamsEx("SteamId",
        "SteamId", DEFAULT_PARAMS_SHORT_STR_NAME, true
    );
    VipM_Limits_RegisterTypeEvent("SteamId", Limit_OnCheck, "@DefaultObjects_Limit_SteamId_OnCheck");
}

DefaultObjects_Limit_SteamId_OnClientAuth(const playerIndex, const steamId[]) {
    copy(PlayerSteamIds[playerIndex], charsmax(PlayerSteamIds[]), steamId);
}

@DefaultObjects_Limit_SteamId_OnCheck(const Trie:p, const playerIndex) {
    static steamId[MAX_AUTHID_LENGTH];
    PCGet_Str(p, "SteamId", steamId, charsmax(steamId));

    return equali(PlayerSteamIds[playerIndex], steamId);
}
