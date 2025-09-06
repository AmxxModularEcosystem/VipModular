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

new g_sSteamIds[MAX_PLAYERS + 1][64];
new g_sIps[MAX_PLAYERS + 1][32];
new g_sRealMapName[32];

new Trie:g_tUsedInRound = Invalid_Trie;
new Trie:g_tUsedInMap = Invalid_Trie;
new Trie:g_tUsedInGame = Invalid_Trie;

new Float:g_fPlayerSpawnTime[MAX_PLAYERS + 1];

new bool:IsSteamLimitAvailable = false;

public plugin_natives() {
    set_native_filter("@NativeFilter");
}

@NativeFilter(const name[], const index, const bool:trap) {
    if (equal(name, "REU_GetAuthtype")) {
        IsSteamLimitAvailable = trap;
    }
}

// TODO: AddParamsEx
// TODO: Move to core plugin
public VipM_Limits_OnInited() {
    register_plugin(PluginName, PluginVersion, PluginAuthor);

    VipM_Limits_RegisterType("ForAll", false, true);
    VipM_Limits_SetStaticValue("ForAll", true);
    VipM_Limits_RegisterType("Always", false, true);
    VipM_Limits_SetStaticValue("Always", true);

    VipM_Limits_RegisterType("Never", false, true);
    VipM_Limits_SetStaticValue("Never", false);

    VipM_Limits_RegisterType("Steam", true, true);

    VipM_Limits_RegisterType("Alive", true, false);
    VipM_Limits_RegisterTypeEvent("Alive", Limit_OnCheck, "@OnAliveCheck");

    VipM_Limits_RegisterType("Bot", true, true);

    VipM_Limits_RegisterType("Name", true, false);
    VipM_Limits_AddTypeParams("Name",
        "Name", ptString, true
    );
    VipM_Limits_RegisterTypeEvent("Name", Limit_OnCheck, "@OnNameCheck");

    VipM_Limits_RegisterType("Flags", true, false);
    VipM_Limits_AddTypeParams("Flags",
        "Flags", ptString, true,
        "Strict", ptBoolean, false
    );
    VipM_Limits_RegisterTypeEvent("Flags", Limit_OnCheck, "@OnFlagsCheck");

    VipM_Limits_RegisterType("SteamId", true, false);
    VipM_Limits_AddTypeParams("SteamId",
        "SteamId", ptString, true
    );
    VipM_Limits_RegisterTypeEvent("SteamId", Limit_OnCheck, "@OnSteamIdCheck");

    VipM_Limits_RegisterType("Ip", true, false);
    VipM_Limits_AddTypeParams("Ip",
        "Ip", ptString, true
    );
    VipM_Limits_RegisterTypeEvent("Ip", Limit_OnCheck, "@OnIpCheck");

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

    VipM_Limits_RegisterType("Round", false, false);
    VipM_Limits_AddTypeParams("Round",
        "Min", ptInteger, false,
        "Max", ptInteger, false
    );
    VipM_Limits_RegisterTypeEvent("Round", Limit_OnCheck, "@OnRoundCheck");

    VipM_Limits_RegisterType("WeekDay", false, false);
    VipM_Limits_AddTypeParams("WeekDay",
        "Day", ptString, true
    );
    VipM_Limits_RegisterTypeEvent("WeekDay", Limit_OnRead, "@OnWeekDayRead");
    VipM_Limits_RegisterTypeEvent("WeekDay", Limit_OnCheck, "@OnWeekDayCheck");

    VipM_Limits_RegisterType("RoundTime", false, false);
    VipM_Limits_AddTypeParams("RoundTime",
        "Min", ptInteger, false,
        "Max", ptInteger, false
    );
    VipM_Limits_RegisterTypeEvent("RoundTime", Limit_OnCheck, "@OnRoundTimeCheck");

    VipM_Limits_RegisterType("LifeTime", true, false);
    VipM_Limits_AddTypeParams("LifeTime",
        "Min", ptInteger, false,
        "Max", ptInteger, false
    );
    VipM_Limits_RegisterTypeEvent("LifeTime", Limit_OnCheck, "@OnLifeTimeCheck");

    VipM_Limits_RegisterType("InFreezyTime", false, false);
    VipM_Limits_AddTypeParams("InFreezyTime",
        "Reverse", ptBoolean, false
    );
    VipM_Limits_RegisterTypeEvent("InFreezyTime", Limit_OnCheck, "@OnInFreezyTimeCheck");

    // thx for idea: https://dev-cs.ru/members/7658/
    VipM_Limits_RegisterType("InBuyZone", true, false);
    VipM_Limits_AddTypeParams("InBuyZone",
        "Reverse", ptBoolean, false
    );
    VipM_Limits_RegisterTypeEvent("InBuyZone", Limit_OnCheck, "@OnInBuyZoneCheck");

    VipM_Limits_RegisterType("OncePerRound", true, false);
    VipM_Limits_RegisterTypeEvent("OncePerRound", Limit_OnCheck, "@OnOncePerRoundCheck");

    // thx for idea: https://dev-cs.ru/threads/24759/page-2#post-141912
    VipM_Limits_RegisterType("OncePerMap", true, false);
    VipM_Limits_RegisterTypeEvent("OncePerMap", Limit_OnCheck, "@OnOncePerMapCheck");

    VipM_Limits_RegisterType("OncePerGame", true, false);
    VipM_Limits_RegisterTypeEvent("OncePerGame", Limit_OnCheck, "@OnOncePerGameCheck");

    VipM_Limits_RegisterType("Time", false, false);
    VipM_Limits_AddTypeParams("Time",
        "Before", ptString, false,
        "After", ptString, false
    );
    VipM_Limits_RegisterTypeEvent("Time", Limit_OnRead, "@OnTimeRead");
    VipM_Limits_RegisterTypeEvent("Time", Limit_OnCheck, "@OnTimeCheck");

    VipM_Limits_RegisterType("Frags", true, false);
    VipM_Limits_AddTypeParams("Frags",
        "Min", ptInteger, false,
        "Max", ptInteger, false
    );
    VipM_Limits_RegisterTypeEvent("Frags", Limit_OnCheck, "@OnFragsCheck");

    VipM_Limits_RegisterType("GameTime", true, false);
    VipM_Limits_AddTypeParams("GameTime",
        "Min", ptFloat, false,
        "Max", ptFloat, false
    );
    VipM_Limits_RegisterTypeEvent("GameTime", Limit_OnCheck, "@OnGameTimeCheck");

    VipM_Limits_RegisterType("Logic-OR", false, false);
    VipM_Limits_AddTypeParams("Logic-OR",
        "Limits", ptLimits, true
    );
    VipM_Limits_RegisterTypeEvent("Logic-OR", Limit_OnCheck, "@OnOrCheck");

    VipM_Limits_RegisterType("Logic-XOR", false, false);
    VipM_Limits_AddTypeParams("Logic-XOR",
        "Limits", ptLimits, true
    );
    VipM_Limits_RegisterTypeEvent("Logic-XOR", Limit_OnCheck, "@OnXorCheck");

    VipM_Limits_RegisterType("Logic-AND", false, false);
    VipM_Limits_AddTypeParams("Logic-AND",
        "Limits", ptLimits, true
    );
    VipM_Limits_RegisterTypeEvent("Logic-AND", Limit_OnCheck, "@OnAndCheck");

    VipM_Limits_RegisterType("Logic-NOT", false, false);
    VipM_Limits_AddTypeParams("Logic-NOT",
        "Limits", ptLimits, true
    );
    VipM_Limits_RegisterTypeEvent("Logic-NOT", Limit_OnCheck, "@OnNotCheck");

    RegisterHookChain(RG_CSGameRules_RestartRound, "@OnRestartRound", true);
    RegisterHookChain(RG_CBasePlayer_Spawn, "@OnPlayerSpawn", true);

    g_tUsedInRound = TrieCreate();
    g_tUsedInMap = TrieCreate();
    g_tUsedInGame = TrieCreate();
}

public client_authorized(UserId, const AuthId[]) {
    if (IsSteamLimitAvailable) {
        VipM_Limits_SetStaticValue("Steam", is_user_steam(UserId), UserId);
    } else {
        VipM_Limits_SetStaticValue("Steam", true, UserId);
    }

    VipM_Limits_SetStaticValue("Bot", bool:is_user_bot(UserId), UserId);

    copy(g_sSteamIds[UserId], charsmax(g_sSteamIds[]), AuthId);
    get_user_ip(UserId, g_sIps[UserId], charsmax(g_sIps[]), true);
}

@OnRestartRound() {
    TrieClear(g_tUsedInRound);
    if (get_member_game(m_bCompleteReset)) {
        TrieClear(g_tUsedInGame);
    }
}

@OnPlayerSpawn(const UserId) {
    g_fPlayerSpawnTime[UserId] = get_gametime();
}

@OnGameTimeCheck(const Trie:params) {
    new Float:gameTime = get_gametime();

    new Float:min;
    if (TrieGetCell(params, "Min", min) && gameTime < min) {
        return false;
    }

    new Float:max;
    if (TrieGetCell(params, "Max", max) && gameTime > max) {
        return false;
    }

    return true;
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

@OnTimeRead(const JSON:jCfg, const Trie:tParams) {
    new sTime[8];
    
    TrieGetString(tParams, "Before", sTime, charsmax(sTime));
    TrieSetCell(tParams, "Before", ParseColonTime(sTime), .replace = true);

    TrieGetString(tParams, "After", sTime, charsmax(sTime));
    TrieSetCell(tParams, "After", ParseColonTime(sTime), .replace = true);

    return VIPM_CONTINUE;
}

@OnTimeCheck(const Trie:tParams) {
    new iBefore = PCGet_Int(tParams, "Before", 0);
    new iAfter = PCGet_Int(tParams, "After", 0);
    new iCurrent = GetDayTimeInSeconds();

    Dbg_Log("@OnTimeCheck(%d):", tParams);
    Dbg_Log("  iBefore = %d", iBefore);
    Dbg_Log("  iAfter = %d", iAfter);
    Dbg_Log("  iCurrent = %d", iCurrent);

    if (!iBefore && !iAfter) {
        return true;
    }

    if (!iBefore) {
        return iCurrent > iAfter;
    }

    if (!iAfter) {
        return iCurrent < iBefore;
    }

    if (iBefore == iAfter) {
        return iBefore == iCurrent;
    }

    if (iAfter < iBefore) {
        return (
            iCurrent > iAfter
            && iCurrent < iBefore
        );
    }

    if (iAfter > iBefore) {
        return (
            iCurrent > iAfter
            || iCurrent < iBefore
        );
    }
    
    return false;
}

@OnOncePerGameCheck(const Trie:tParams, const UserId) {
    static sTrieKey[64];
    formatex(sTrieKey, charsmax(sTrieKey), "%s|%d", g_sSteamIds[UserId], tParams);
    // В случае лимитов, хендлер Trie параметров можно считать уникальным для каждого инстанса лимита
    // Нужно чтобы можно было указать этот лимит в двух местах и чтобы они при этом не пересекались
    // Ну а SteamID для того, чтобы после перезахода оно не сбрасывалось
    //
    // P.S. Или лучше всё же пихать в параметры идентификатор, по котому их разделять?
    // Или оба сразу?)
    //
    // UPD: Устарело, не стоит рассчитывать на кэширование ссылок.

    if (TrieKeyExists(g_tUsedInGame, sTrieKey)) {
        return false;
    }

    TrieSetCell(g_tUsedInGame, sTrieKey, true);
    return true;
}

@OnOncePerMapCheck(const Trie:tParams, const UserId) {
    static sTrieKey[64];
    formatex(sTrieKey, charsmax(sTrieKey), "%s|%d", g_sSteamIds[UserId], tParams);

    if (TrieKeyExists(g_tUsedInMap, sTrieKey)) {
        return false;
    }

    TrieSetCell(g_tUsedInMap, sTrieKey, true);
    return true;
}

@OnOncePerRoundCheck(const Trie:tParams, const UserId) {
    static sTrieKey[64];
    formatex(sTrieKey, charsmax(sTrieKey), "%s|%d", g_sSteamIds[UserId], tParams);

    if (TrieKeyExists(g_tUsedInRound, sTrieKey)) {
        return false;
    }

    TrieSetCell(g_tUsedInRound, sTrieKey, true);
    return true;
}

@OnInBuyZoneCheck(const Trie:Params, const UserId) {
    new bool:bInBuyZone = IsUserInBuyZone(UserId);
    return PCGet_Bool(Params, "Reverse", false) ? !bInBuyZone : bInBuyZone;
}

@OnInFreezyTimeCheck(const Trie:Params) {
    new bool:bFreezyPeriod = get_member_game(m_bFreezePeriod);
    return PCGet_Bool(Params, "Reverse", false) ? !bFreezyPeriod : bFreezyPeriod;
}

@OnRoundTimeCheck(const Trie:Params) {
    new iMin = PCGet_Int(Params, "Min", 0);
    new iMax = PCGet_Int(Params, "Max", 0);
    new iRoundTime = floatround(get_gametime() - Float:get_member_game(m_fRoundStartTime));

    return (
        (!iMin || iRoundTime >= iMin)
        && (!iMax || iRoundTime <= iMax)
    );
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

@OnWeekDayRead(const JSON:jCfg, const Trie:Params) {
    new sWeekDayName[32];
    json_object_get_string(jCfg, "Day", sWeekDayName, charsmax(sWeekDayName));
    new iWeekDayIndex = GetWeekDayIdByName(sWeekDayName);
    if (iWeekDayIndex < 0) {
        log_amx("[WARNING] Undefined week day '%s'.", sWeekDayName);
        return VIPM_STOP;
    }

    TrieSetCell(Params, "Day", iWeekDayIndex);
    return VIPM_CONTINUE;
}

@OnWeekDayCheck(const Trie:Params) {
    new sWeekDay[4];
    get_time("%w", sWeekDay, charsmax(sWeekDay));
    return str_to_num(sWeekDay) == PCGet_Int(Params, "Day", -1);
}

@OnAliveCheck(const Trie:Params, const UserId) {
    return is_user_alive(UserId);
}

@OnNameCheck(const Trie:Params, const UserId) {
    return IsEqualUserName(UserId, PCGet_iStr(Params, "Name"));
}

bool:@OnFlagsCheck(const Trie:Params, const UserId) {
    static sFlags[16];
    PCGet_Str(Params, "Flags", sFlags, charsmax(sFlags));

    return HasUserFlagsStr(UserId, sFlags, PCGet_Bool(Params, "Strict", false));
}

@OnSteamIdCheck(const Trie:Params, const UserId) {
    static sSteamId[64];
    PCGet_Str(Params, "SteamId", sSteamId, charsmax(sSteamId));

    return equali(g_sSteamIds[UserId], sSteamId);
}

@OnIpCheck(const Trie:Params, const UserId) {
    static sIp[32];
    PCGet_Str(Params, "SteamId", sIp, charsmax(sIp));

    return equali(g_sIps[UserId], sIp);
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

@OnRoundCheck(const Trie:Params, const UserId){
    return (
        GetCurrentRoundNum() >= PCGet_Int(Params, "Min", -1)
        && GetCurrentRoundNum() <= PCGet_Int(Params, "Max", cellmax)
    );
}

@OnOrCheck(const Trie:Params, const UserId){
    return PCGet_VipmLimitsCheck(Params, "Limits", UserId, Limit_Exec_OR);
}

@OnXorCheck(const Trie:Params, const UserId){
    return PCGet_VipmLimitsCheck(Params, "Limits", UserId, Limit_Exec_XOR);
}

@OnAndCheck(const Trie:Params, const UserId){
    return PCGet_VipmLimitsCheck(Params, "Limits", UserId, Limit_Exec_AND);
}

@OnNotCheck(const Trie:Params, const UserId){
    return !PCGet_VipmLimitsCheck(Params, "Limits", UserId, Limit_Exec_AND);
}
