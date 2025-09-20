#include <amxmodx>
#include <VipModular>

static Trie:UsedInRound = Invalid_Trie;
static Trie:UsedInMap = Invalid_Trie;
static Trie:UsedInGame = Invalid_Trie;

DefaultObjects_Limit_OncePer_Register() {
    UsedInRound = TrieCreate();
    UsedInMap = TrieCreate();
    UsedInGame = TrieCreate();
    RegisterHookChain(RG_CSGameRules_RestartRound, "@DefaultObjects_Limit_OncePer_OnRestartRound", true);
    
    VipM_Limits_RegisterType("OncePerRound", true, false);
    VipM_Limits_RegisterTypeEvent("OncePerRound", Limit_OnCheck, "@DefaultObjects_Limit_OncePer_OnOncePerRoundCheck");

    // thx for idea: https://dev-cs.ru/threads/24759/page-2#post-141912
    VipM_Limits_RegisterType("OncePerMap", true, false);
    VipM_Limits_RegisterTypeEvent("OncePerMap", Limit_OnCheck, "@DefaultObjects_Limit_OncePer_OnOncePerMapCheck");

    VipM_Limits_RegisterType("OncePerGame", true, false);
    VipM_Limits_RegisterTypeEvent("OncePerGame", Limit_OnCheck, "@DefaultObjects_Limit_OncePer_OnOncePerGameCheck");

    // TODO: Универсальный лимит "RateLimited"
}

@DefaultObjects_Limit_OncePer_OnRestartRound() {
    TrieClear(UsedInRound);
    if (get_member_game(m_bCompleteReset)) {
        TrieClear(UsedInGame);
    }
}

@DefaultObjects_Limit_OncePer_OnOncePerGameCheck(const Trie:p, const playerIndex) {
    static sTrieKey[MAX_AUTHID_LENGTH + 12];
    formatex(sTrieKey, charsmax(sTrieKey), "%d|%d", get_user_userid(playerIndex), p);
    // В случае лимитов, хендлер Trie параметров можно считать уникальным для каждого инстанса лимита
    // Нужно чтобы можно было указать этот лимит в двух местах и чтобы они при этом не пересекались
    // Ну а SteamID для того, чтобы после перезахода оно не сбрасывалось
    //
    // P.S. Или лучше всё же пихать в параметры идентификатор, по котому их разделять?
    // Или оба сразу?)
    //
    // UPD: Устарело, не стоит рассчитывать на кэширование ссылок.

    if (TrieKeyExists(UsedInGame, sTrieKey)) {
        return false;
    }

    TrieSetCell(UsedInGame, sTrieKey, true);
    return true;
}

@DefaultObjects_Limit_OncePer_OnOncePerMapCheck(const Trie:p, const playerIndex) {
    static sTrieKey[MAX_AUTHID_LENGTH + 12];
    formatex(sTrieKey, charsmax(sTrieKey), "%d|%d", get_user_userid(playerIndex), p);

    if (TrieKeyExists(UsedInMap, sTrieKey)) {
        return false;
    }

    TrieSetCell(UsedInMap, sTrieKey, true);
    return true;
}

@DefaultObjects_Limit_OncePer_OnOncePerRoundCheck(const Trie:p, const playerIndex) {
    static sTrieKey[MAX_AUTHID_LENGTH + 12];
    formatex(sTrieKey, charsmax(sTrieKey), "%d|%d", get_user_userid(playerIndex), p);

    if (TrieKeyExists(UsedInRound, sTrieKey)) {
        return false;
    }

    TrieSetCell(UsedInRound, sTrieKey, true);
    return true;
}
