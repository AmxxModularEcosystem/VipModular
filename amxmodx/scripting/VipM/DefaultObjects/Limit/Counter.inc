#include <amxmodx>
#include <reapi>
#include <ParamsController>
#include <VipModular>
#include <VipM/L/Counter>

static Trie:CounterTypes[VipM_L_Counter_Type];

DefaultObjects_Limit_Counter_Register() {
    VipM_Limits_RegisterType("Counter", true, false);
    VipM_Limits_AddParamsEx("Counter",
        "Type", VIPM_L_COUNTER_PARAM_TYPE, true,
        "Key", DEFAULT_PARAMS_SHORT_STR_NAME, true,
        "Max", DEFAULT_PARAMS_INT_NAME, false
    );

    RegisterHookChain(RG_CBasePlayer_Spawn, "@DefaultObjects_Limit_Counter_OnPlayerSpawn", false);
    RegisterHookChain(RG_CSGameRules_RestartRound, "@DefaultObjects_Limit_Counter_OnRestartRound", false);
}

static DefaultObjects_Limit_Counter_ResetCounters(const VipM_L_Counter_Type:type) {
    if (CounterTypes[type] == Invalid_Trie) {
        return;
    }

    new TrieIter:iter = TrieIterCreate(CounterTypes[type]);
    while (!TrieIterEnded(iter)) {
        new Trie:countersByKey;
        if (TrieIterGetCell(iter, countersByKey)) {
            TrieDestroy(countersByKey);
        }
        TrieIterNext(iter);
    }
    TrieIterDestroy(iter);

    TrieDestroy(CounterTypes[type]);
}

static DefaultObjects_Limit_Counter_ResetCountersForPlayer(const VipM_L_Counter_Type:type, const playerIndex) {
    if (CounterTypes[type] == Invalid_Trie) {
        return;
    }

    new TrieIter:iter = TrieIterCreate(CounterTypes[type]);
    while (!TrieIterEnded(iter)) {
        new Trie:countersByKey;
        if (TrieIterGetCell(iter, countersByKey)) {
            TrieDeleteKey(countersByKey, DefaultObjects_Limit_Counter_GetPlayerKey(playerIndex));
        }
        TrieIterNext(iter);
    }
    TrieIterDestroy(iter);
}

static Trie:DefaultObjects_Limit_Counter_PrepareKey(const VipM_L_Counter_Type:type, const key[]) {
    if (CounterTypes[type] == Invalid_Trie) {
        CounterTypes[type] = TrieCreate();
    }

    new Trie:countersByKey = Invalid_Trie;
    if (!TrieGetCell(CounterTypes[type], key, countersByKey)) {
        countersByKey = TrieCreate();
        TrieSetCell(CounterTypes[type], key, countersByKey);
    }

    return countersByKey;
}

static DefaultObjects_Limit_Counter_GetPlayerKey(const playerIndex) {
    new str[MAX_AUTHID_LENGTH];
    get_user_authid(playerIndex, str, charsmax(str));
    return str;
}

@DefaultObjects_Limit_Counter_OnCheck(const Trie:p, const playerIndex) {
    new Trie:counter = DefaultObjects_Limit_Counter_PrepareKey(
        .type = PCGet_Cell(p, "Type"),
        .key = PCGet_iStr(p, "Key")
    );

    new value;
    if (!TrieGetCell(counter, DefaultObjects_Limit_Counter_GetPlayerKey(playerIndex), value)) {
        value = 0;
    }

    if (value >= PCGet_Int(p, "Max")) {
        return false;
    }

    TrieSetCell(counter, DefaultObjects_Limit_Counter_GetPlayerKey(playerIndex), ++value);

    return true;
}

DefaultObjects_Limit_Counter_OnClientPutInServer(const playerIndex) {
    DefaultObjects_Limit_Counter_ResetCountersForPlayer(VipM_L_Counter_PerSession, playerIndex);
}

@DefaultObjects_Limit_Counter_OnPlayerSpawn(const playerIndex) {
    DefaultObjects_Limit_Counter_ResetCountersForPlayer(VipM_L_Counter_PerLife, playerIndex);
}

@DefaultObjects_Limit_Counter_OnRestartRound(const playerIndex) {
    DefaultObjects_Limit_Counter_ResetCounters(VipM_L_Counter_PerRound);
    if (get_member_game(m_bCompleteReset)) {
        DefaultObjects_Limit_Counter_ResetCounters(VipM_L_Counter_PerGame);
    }
}

DefaultObjects_Limit_Counter_RegsiterNatives() {
    register_native("VipM_L_Counter_Get", "@VipM_L_Counter_Get");
    register_native("VipM_L_Counter_Set", "@VipM_L_Counter_Set");
}

@VipM_L_Counter_Get() {
    enum {Arg_Type = 1, Arg_Key, Arg_PlayerIndex}

    new VipM_L_Counter_Type:type = VipM_L_Counter_Type:get_param(Arg_Type);
    new key[VIPM_L_COUNTER_KEY_MAX_LEN];
    get_string(Arg_Key, key, charsmax(key));
    new playerIndex = get_param(Arg_PlayerIndex);

    new value = 0;
    TrieGetCell(
        DefaultObjects_Limit_Counter_PrepareKey(type, key),
        DefaultObjects_Limit_Counter_GetPlayerKey(playerIndex),
        value
    );

    return value;
}

@VipM_L_Counter_Set() {
    enum {Arg_Type = 1, Arg_Key, Arg_PlayerIndex, Arg_Value}

    new VipM_L_Counter_Type:type = VipM_L_Counter_Type:get_param(Arg_Type);
    new key[VIPM_L_COUNTER_KEY_MAX_LEN];
    get_string(Arg_Key, key, charsmax(key));
    new playerIndex = get_param(Arg_PlayerIndex);
    new value = get_param(Arg_Value);

    TrieSetCell(
        DefaultObjects_Limit_Counter_PrepareKey(type, key),
        DefaultObjects_Limit_Counter_GetPlayerKey(playerIndex),
        value
    );

    return value;
}
