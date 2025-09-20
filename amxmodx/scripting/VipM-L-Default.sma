#include <amxmodx>
#include <reapi>
#include <VipModular>
#include "VipM/Utils"
#include "VipM/DebugMode"

#pragma semicolon 1
#pragma compress 1

public stock const PluginName[] = "[VipM-L] Default";
public stock const PluginVersion[] = _VIPM_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = _VIPM_PLUGIN_URL;

new g_sRealMapName[32];

new Float:g_fPlayerSpawnTime[MAX_PLAYERS + 1];

// TODO: AddParamsEx
// TODO: Move to core plugin
public VipM_Limits_OnInited() {
    register_plugin(PluginName, PluginVersion, PluginAuthor);

    VipM_Limits_RegisterType("Alive", true, false);
    VipM_Limits_RegisterTypeEvent("Alive", Limit_OnCheck, "@OnAliveCheck");

    VipM_Limits_RegisterType("Bot", true, true);

    VipM_Limits_RegisterType("Flags", true, false);
    VipM_Limits_AddTypeParams("Flags",
        "Flags", ptString, true,
        "Strict", ptBoolean, false
    );
    VipM_Limits_RegisterTypeEvent("Flags", Limit_OnCheck, "@OnFlagsCheck");

    VipM_Limits_RegisterType("Map", false, false);
    VipM_Limits_AddTypeParams("Map",
        "Map", ptString, true,
        "Real", ptBoolean, false,
        "Prefix", ptBoolean, false
    );
    VipM_Limits_RegisterTypeEvent("Map", Limit_OnCheck, "@OnMapCheck");
    rh_get_mapname(g_sRealMapName, charsmax(g_sRealMapName), MNT_TRUE);

    VipM_Limits_RegisterType("HasPrimaryWeapon", true, false);
    VipM_Limits_AddTypeParams("HasPrimaryWeapon",
        "HasNot", ptBoolean, false
    );
    VipM_Limits_RegisterTypeEvent("HasPrimaryWeapon", Limit_OnCheck, "@OnHasPrimaryWeaponCheck");

    VipM_Limits_RegisterType("LifeTime", true, false);
    VipM_Limits_AddTypeParams("LifeTime",
        "Min", ptInteger, false,
        "Max", ptInteger, false
    );
    VipM_Limits_RegisterTypeEvent("LifeTime", Limit_OnCheck, "@OnLifeTimeCheck");

    // thx for idea: https://dev-cs.ru/members/7658/
    VipM_Limits_RegisterType("InBuyZone", true, false);
    VipM_Limits_AddTypeParams("InBuyZone",
        "Reverse", ptBoolean, false
    );
    VipM_Limits_RegisterTypeEvent("InBuyZone", Limit_OnCheck, "@OnInBuyZoneCheck");

    VipM_Limits_RegisterType("Frags", true, false);
    VipM_Limits_AddTypeParams("Frags",
        "Min", ptInteger, false,
        "Max", ptInteger, false
    );
    VipM_Limits_RegisterTypeEvent("Frags", Limit_OnCheck, "@OnFragsCheck");

    RegisterHookChain(RG_CBasePlayer_Spawn, "@OnPlayerSpawn", true);

}

public client_authorized(UserId, const AuthId[]) {
    VipM_Limits_SetStaticValue("Bot", bool:is_user_bot(UserId), UserId);
}

@OnPlayerSpawn(const UserId) {
    g_fPlayerSpawnTime[UserId] = get_gametime();
}

@OnFragsCheck(const Trie:params, const playerIndex) {
    new frags = get_user_frags(playerIndex);

    new min;
    if (TrieGetCell(params, "Min", min) && frags < min) {
        return false;
    }

    new max;
    if (TrieGetCell(params, "Max", max) && frags > max) {
        return false;
    }

    return true;
}

@OnInBuyZoneCheck(const Trie:Params, const UserId) {
    new bool:bInBuyZone = IsUserInBuyZone(UserId);
    return PCGet_Bool(Params, "Reverse", false) ? !bInBuyZone : bInBuyZone;
}

@OnLifeTimeCheck(const Trie:Params, const UserId) {
    new iMin = PCGet_Int(Params, "Min", 0);
    new iMax = PCGet_Int(Params, "Max", 0);
    new iLifeTime = floatround(get_gametime() - g_fPlayerSpawnTime[UserId]);

    return (
        (!iMin || iLifeTime >= iMin)
        && (!iMax || iLifeTime <= iMax)
    );
}

@OnAliveCheck(const Trie:Params, const UserId) {
    return is_user_alive(UserId);
}

bool:@OnFlagsCheck(const Trie:Params, const UserId) {
    static sFlags[16];
    PCGet_Str(Params, "Flags", sFlags, charsmax(sFlags));

    return HasUserFlagsStr(UserId, sFlags, PCGet_Bool(Params, "Strict", false));
}

@OnMapCheck(const Trie:Params) {
    static sMap[32];
    PCGet_Str(Params, "Map", sMap, charsmax(sMap));

    new iCount = PCGet_Bool(Params, "Prefix", false) ? strlen(sMap) : 0;

    if (PCGet_Bool(Params, "Real", false)) {
        return equali(sMap, g_sRealMapName, iCount);
    } else {
        static sSetMapName[32];
        rh_get_mapname(sSetMapName, charsmax(sSetMapName), MNT_SET);
        return equali(sMap, sSetMapName, iCount);
    }
}

@OnHasPrimaryWeaponCheck(const Trie:Params, const UserId) {
    new bool:res = get_member(UserId, m_bHasPrimary);
    return PCGet_Bool(Params, "HasNot", false) ? !res : res;
}
