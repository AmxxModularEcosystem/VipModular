#if defined _vipmodular_src_ArrayTrieUtils_included
    #endinput
#endif
#define _vipmodular_src_ArrayTrieUtils_included

#include <amxmodx>

stock ArraySizeSafe(const Array:h) {
    return h == Invalid_Array ? 0 : ArraySize(h);
}

#define ArrayForeachArray2(%1:%4=>%2[%3]) \
    if (%1 != Invalid_Array) \
        for (new %2[%3], %4 = 0; %4 < ArraySizeSafe(%1); %4++) \
            if (ArrayGetArray(%1, %4, %2))

#define ArrayForeachString(%1:%4=>%2[%3]) \
    if (%1 != Invalid_Array) \
        for (new %2[%3], %4 = 0; %4 < ArraySizeSafe(%1); %4++) \
            if (ArrayGetString(%1, %4, %2, charsmax(%2)))

stock Array:ArrayCreateIfNotCreated(&Array:h, const cellsize = 1, const reserved = 1) {
    return h = (h == Invalid_Array) ? ArrayCreate(cellsize, reserved) : h;
}

stock Trie:TrieCreateIfNotCreated(&Trie:h) {
    return h = (h == Invalid_Trie) ? TrieCreate() : h;
}
