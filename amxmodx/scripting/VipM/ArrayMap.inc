#if defined _vipmodular_src_ArrayMap_included
    #endinput
#endif
#define _vipmodular_src_ArrayMap_included

#include <amxmodx>

enum ArrayMap {
    Array:AM_Arr,
    Trie:AM_Map,
}
#define ArrayMap(%1) %1[ArrayMap]

stock ArrayMapCreate(ArrayMap(am), const CellSize = 1, const Reserved = 1) {
    am[AM_Arr] = ArrayCreate(CellSize, Reserved);
    am[AM_Map] = TrieCreate();
}

stock bool:ArrayMapCreated(const ArrayMap(am)) {
    return am[AM_Arr] != Invalid_Array;
}

stock ArrayMapDestroy(ArrayMap(am)) {
    if (ArrayMapCreated(am)) {
        ArrayDestroy(am[AM_Arr]);
        TrieDestroy(am[AM_Map]);
    }
}

stock ArrayMapSize(const ArrayMap(am)) {
    if (ArrayMapCreated(am)) {
        return ArraySize(am[AM_Arr]);
    }

    return 0;
}

stock bool:ArrayMapKeyExists(const ArrayMap(am), const key[]) {
    return TrieKeyExists(am[AM_Map], key);
}

stock ArrayMapGetIndex(const ArrayMap(am), const key[]) {
    new index = -1;
    TrieGetCell(am[AM_Map], key, index);

    return index;
}

// cell

stock ArrayMapPushCell(const ArrayMap(am), any:index, const key[]) {
    new index = ArrayPushCell(am[AM_Arr], index);
    TrieSetCell(am[AM_Map], key, index, .replace = false);

    return index;
}

stock ArrayMapGetCell(const ArrayMap(am), const any:index, const block = 0, const bool:asChar = false) {
    return ArrayGetCell(am[AM_Arr], index, block, asChar);
}

stock ArrayMapGetCellByKey(const ArrayMap(am), const key[], const block = 0, const bool:asChar = false) {
    return ArrayGetCell(am[AM_Arr], ArrayMapGetIndex(am, key), block, asChar);
}

stock ArrayMapSetCell(const ArrayMap(am), const any:index, const in, const block = 0, const bool:asChar = false) {
    return ArraySetCell(am[AM_Arr], index, in, block, asChar);
}

stock ArrayMapSetCellByKey(const ArrayMap(am), const key[], const in, const block = 0, const bool:asChar = false) {
    return ArraySetCell(am[AM_Arr], ArrayMapGetIndex(am, key), in, block, asChar);
}

// string

stock ArrayMapPushString(const ArrayMap(am), const Input[], const sKey[]) {
    new index = ArrayPushString(am[AM_Arr], Input);
    TrieSetCell(am[AM_Map], sKey, index, .replace = false);

    return index;
}

stock ArrayMapGetString(const ArrayMap(am), const any:index, out[], const outLen) {
    return ArrayGetString(am[AM_Arr], index, out, outLen);
}

stock ArrayMapGetStringByKey(const ArrayMap(am), const key[], out[], const outLen) {
    return ArrayGetString(am[AM_Arr], ArrayMapGetIndex(am, key), out, outLen);
}

stock ArrayMapSetString(const ArrayMap(am), const any:index, const in[]) {
    return ArraySetString(am[AM_Arr], index, in);
}

stock ArrayMapSetStringByKey(const ArrayMap(am), const key[], const in[]) {
    return ArraySetString(am[AM_Arr], ArrayMapGetIndex(am, key), in);
}

// array

stock ArrayMapPushArray(const ArrayMap(am), const any:Input[], const sKey[], size = -1) {
    new index = ArrayPushArray(am[AM_Arr], Input, size);
    TrieSetCell(am[AM_Map], sKey, index, .replace = false);

    return index;
}

stock ArrayMapGetArray(const ArrayMap(am), const any:index, out[], const size = -1) {
    return ArrayGetArray(am[AM_Arr], index, out, size);
}

stock ArrayMapGetArrayByKey(const ArrayMap(am), const key[], out[], const size = -1) {
    return ArrayGetArray(am[AM_Arr], ArrayMapGetIndex(am, key), out, size);
}

stock ArrayMapSetArray(const ArrayMap(am), const any:index, const in[], const size = -1) {
    return ArraySetArray(am[AM_Arr], index, in, size);
}

stock ArrayMapSetArrayByKey(const ArrayMap(am), const key[], const in[], const size = -1) {
    return ArraySetArray(am[AM_Arr], ArrayMapGetIndex(am, key), in, size);
}

#define ArrayMapForeachArray(%1:%4=>%2[%3]) \
    if (ArrayMapCreated(%1)) \
        for (new %2[%3], any:%4 = 0; %4 < ArrayMapSize(%1); %4++) \
            if (ArrayMapGetArray(%1, %4, %2))
