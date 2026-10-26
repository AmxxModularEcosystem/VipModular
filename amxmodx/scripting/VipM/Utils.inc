#if defined _vipmodular_src_glUtils_included
    #endinput
#endif
#define _vipmodular_src_glUtils_included

// Надо наверное как-то их обьединить)
#include "VipM/ArrayTrieUtils"

#include <amxmodx>
#include <json>
#include <reapi>
#include <VipModular>

stock const __SLANG_TEMPLATE_STR[] = "%l";
stock const __CLANG_TEMPLATE_STR[] = "%L";
stock const __LANG_MSG_LAYOUT_KEY[] = "MSG_LAYOUT";

stock const bool:__CompositeMacros_AlwaysFalse = false;
#define CompositeMacros(%1) \
    do { %1 } while(__CompositeMacros_AlwaysFalse)

stock IntToStr(const int) {
    new str[11];
    num_to_str(int, str, charsmax(str));

    return str;
}

stock IsPlayerIndexValid(const playerIndex) {
    return playerIndex >= 1 && playerIndex <= 32;
}

#define Lang(%1) \
    fmt(__SLANG_TEMPLATE_STR, %1)

#define uLang(%1) \
    fmt(__CLANG_TEMPLATE_STR, %1)

#define ChatPrint(%1,%2) \
    client_print_color(%1, print_team_default, __CLANG_TEMPLATE_STR, IsPlayerIndexValid(%1) ? %1 : -1, __LANG_MSG_LAYOUT_KEY, fmt(%2))

#define ChatPrintL(%1,%2) \
    ChatPrint(%1, __CLANG_TEMPLATE_STR, %1, %2)

#define ChatPrintLIf(%3,%1,%2) \
    ((%3) && ChatPrint(%1, __CLANG_TEMPLATE_STR, %1, %2))

#define json_array_foreach_vars(%1:%2,[%3]) \
    if (json_is_array(%1)) \
        for (new %3, %2 = 0; %2 < json_array_get_count(%1); %2++)

#define json_array_foreach_value(%1:%2=>%3) \
    json_array_foreach_vars(%1: %2, [JSON:%3]) \
        if ((%3 = json_array_get_value(%1, %2)) != Invalid_JSON)

#define json_get_count(%1) \
    (json_is_array(%1) ? json_array_get_count(%1) : 1)

stock Array:json_get_strings_list(const JSON:jValue, const iStringsLen = MAX_FMT_LENGTH, Array:aList = Invalid_Array) {
    static sBuffer[MAX_FMT_LENGTH];
    ArrayCreateIfNotCreated(aList, iStringsLen, json_get_count(jValue));
    
    if (json_is_string(jValue)) {
        json_get_string(jValue, sBuffer, iStringsLen - 1);
        ArrayPushString(aList, sBuffer);
    } else if (json_is_array(jValue)) {
        for (new i = 0, len = json_array_get_count(jValue); i < len; i++) {
            json_array_get_string(jValue, i, sBuffer, iStringsLen - 1);
            ArrayPushString(aList, sBuffer);
        }
    }

    return aList;
}

stock Array:json_object_get_strings_list(
    const JSON:jObj,
    const sKey[],
    const iStringLen = MAX_FMT_LENGTH,
    Array:aList = Invalid_Array,
    const bool:bDotNot = false
) {
    new JSON:jValue = json_object_get_value(jObj, sKey, bDotNot);
    aList = json_get_strings_list(jValue, iStringLen, aList);
    json_free(jValue);
    return aList;
}

#define BitSet(%1,%2) \
    %1 |= (1 << %2)

#define BitReset(%1,%2) \
    %1 &= ~(1 << %2)

// by @the_hunter1 :))
#define BitSetIf(%1,%2,%3) ( \
    %3 \
    && (%1 |= (1 << (%2 & 31))) \
    || (%1 &= ~(1 << (%2 & 31))) \
)

#define BitIs(%1,%2) \
    (bool:(%1 & (1 << %2)))

#define CallOnce() CompositeMacros( \
    static bool:__CallOnce_bCalled; \
    if (__CallOnce_bCalled) return; \
    __CallOnce_bCalled = true; \
)

stock ItemHasClip(const ItemId) {
    new WeaponIdType:WeaponId = get_member(ItemId, m_iId);
    switch (WeaponId) {
        case WEAPON_KNIFE, WEAPON_HEGRENADE, WEAPON_SMOKEGRENADE, WEAPON_FLASHBANG, WEAPON_SHIELDGUN:
            return false;
        default:
            return true;
    }
    return true;
}

stock InstantReloadAllWeapons(const UserId) {
    for (new InventorySlotType:iSlot = PRIMARY_WEAPON_SLOT; iSlot <= PISTOL_SLOT; iSlot++) {
        new ItemId = get_member(UserId, m_rgpPlayerItems, iSlot);
        while (!is_nullent(ItemId)) {
            if (ItemHasClip(ItemId)) {
                set_member(ItemId, m_Weapon_iClip, rg_get_iteminfo(ItemId, ItemInfo_iMaxClip));
            }
            ItemId = get_member(ItemId, m_pNext);
        }
    }
}

stock InstantReloadActiveWeapon(const UserId) {
    new ItemId = get_member(UserId, m_pActiveItem);
    if (is_nullent(ItemId)) {
        return;
    }

    if (ItemHasClip(ItemId)) {
        set_member(ItemId, m_Weapon_iClip, rg_get_iteminfo(ItemId, ItemInfo_iMaxClip));
    }
}

stock bool:IsUserInBuyZone(const UserId) {
    new Signal[UnifiedSignals];
    get_member(UserId, m_signals, Signal);
    return ((SignalState:Signal[US_State] & SIGNAL_BUY) == SIGNAL_BUY);
}
