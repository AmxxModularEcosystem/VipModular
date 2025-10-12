#include <amxmodx>
#include <ParamsController>
#include <VipModular>

new playerSteamIds[MAX_PLAYERS + 1][MAX_AUTHID_LENGTH];

DefaultObjects_Limit_Flags_Register() {
    VipM_Limits_RegisterType("Flags", true, false);
    VipM_Limits_AddParamsEx("Flags",
        "Flags", DEFAULT_PARAMS_FLAGS_NAME, true
        "Strict", DEFAULT_PARAMS_BOOL_NAME, false
    );
    VipM_Limits_RegisterTypeEvent("Flags", Limit_OnCheck, "@DefaultObjects_Limit_Flags_OnCheck");
}

@DefaultObjects_Limit_Flags_OnCheck(const Trie:p, const playerIndex) {
    new playerFlags = get_user_flags(playerIndex);
    new flags = PCGet_Int(p, "Flags");

    if (PCGet_Bool(p, "Strict", false)) {
        return (playerFlags & flags) == flags;
    } else {
        return playerFlags & flags;
    }
}
