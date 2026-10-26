#if defined __vipm_core_objects_modules_unit_included
    #endinput
#endif
#define __vipm_core_objects_modules_unit_included

#include <amxmodx>
#include "VipM/Utils"
#include "VipM/Forwards"
#include <VipM/Modules>
#include "VipM/ArrayTrieUtils"

#include "VipM/Core/Objects/Modules/Type"

enum T_ModuleUnit { Invalid_ModuleUnit = -1 }

enum _:S_ModuleUnit {
    T_ModuleType:ModuleUnit_Type,
    Trie:ModuleUnit_Params,
}

static Array:ModuleUnits = Invalid_Array;

ModuleUnit_Init() {
    CallOnce();
    log_amx("Init module units...");

    ModuleType_Init();

    ModuleUnits = ArrayCreate(S_ModuleUnit, 1);
    
    Forwards_RegAndCall("VipM_Modules_OnInited", ET_IGNORE);
    log_amx("Module units inited.");
}

ModuleUnit_GetCount() {
    return ArraySizeSafe(ModuleUnits);
}

static T_ModuleUnit:ModuleUnit__Construct(const T_ModuleType:type, const Trie:p) {
    if (ModuleUnits == Invalid_Array) {
        abort(AMX_ERR_GENERAL, "Attempt to create module unit before init.");
        return Invalid_ModuleUnit;
    }
    
    new unitObject[S_ModuleUnit];

    unitObject[ModuleUnit_Type] = type;
    unitObject[ModuleUnit_Params] = p;

    ModuleType_MarkAsUsed(type);

    return T_ModuleUnit:ArrayPushArray(ModuleUnits, unitObject);
}

static any:ModuleUnit__GetValue(const T_ModuleUnit:iModuleUnit, const iCell) {
    return ArrayGetCell(ModuleUnits, _:iModuleUnit, iCell);
}

T_ModuleUnit:ModuleUnit_Read(const JSON:valueJson) {
    static Trie:cache = Invalid_Trie;
    TrieCreateIfNotCreated(cache);
    
    new JSON:linked, T_ModuleUnit:cached = Invalid_ModuleUnit, linkPath[PLATFORM_MAX_PATH];
    if (PCJson_HandleCachedLinkedValue(cache, valueJson, linked, cached, linkPath, charsmax(linkPath)) == PCJson_CachedLinkedValue_Cached) {
        return cached;
    }

    new T_ModuleType:type = PCSingle_ObjVipmModuleType(linked, "Module", .orFail = true);
    if (type == Invalid_ModuleType) {
        PCJson_FreeLinked(linked, valueJson);
        return Invalid_ModuleUnit;
    }

    new Trie:p = ModuleType_ReadParams(type, linked);
    if (p == Invalid_Trie) {
        PCJson_FreeLinked(linked, valueJson);
        return Invalid_ModuleUnit;
    }

    cached = ModuleUnit__Construct(type, p);
    if (linkPath[0] != EOS && cached != Invalid_ModuleUnit) {
        TrieSetCell(cache, linkPath, cached);
    }

    PCJson_FreeLinked(linked, valueJson);
    return cached;
}

Array:ModuleUnit_ReadList(const JSON:jModuleUnits, &Array:aModuleUnits = Invalid_Array) {
    ArrayCreateIfNotCreated(aModuleUnits);

    if (!json_is_array(jModuleUnits)) {
        new T_ModuleUnit:iModuleUnit = ModuleUnit_Read(jModuleUnits);
        if (iModuleUnit != Invalid_ModuleUnit) {
            ArrayPushCell(aModuleUnits, iModuleUnit);    
        }
    } else {
        json_array_foreach_value (jModuleUnits: i => jModuleUnit) {
            aModuleUnits = ModuleUnit_ReadList(jModuleUnit, aModuleUnits);
            json_free(jModuleUnit);
        }
    }

    return aModuleUnits;
}

Array:JsonObject_GetModuleUnits(const JSON:jObj, const sKey[], const bool:bDotNot = false, &Array:aModuleUnits = Invalid_Array) {
    if (!json_object_has_value(jObj, sKey, .dot_not = bDotNot)) {
        return aModuleUnits;
    }

    new JSON:jModuleUnits = json_object_get_value(jObj, sKey, bDotNot);
    aModuleUnits = ModuleUnit_ReadList(jModuleUnits, aModuleUnits);
    json_free(jModuleUnits);

    return aModuleUnits;
}

Trie:ModuleUnit_GetParams(const T_ModuleUnit:iModuleUnit) {
    return ModuleUnit__GetValue(iModuleUnit, ModuleUnit_Params);
}

ModuleUnit_GetTypeName(const T_ModuleUnit:iModuleUnit, sOut[], const iOutLen) {
    return ModuleType_GetName(ModuleUnit__GetValue(iModuleUnit, ModuleUnit_Type), sOut, iOutLen);
}

// ModuleUnit_iGetTypeName(const T_ModuleUnit:iModuleUnit) {
//     new sName[VIPM_MODULES_TYPE_NAME_MAX_LEN];
//     ModuleUnit_GetModuleName(iModuleUnit, sName, charsmax(sName));
//     return sName;
// }

Trie:ModuleUnit_FreeParamsIfTemp(&Trie:p) {
    if (p == Invalid_Trie) {
        return p;
    }

    if (TrieKeyExists(p, VIPM_MODULES_PARAMS_TEMP_MARK_KEY)) {
        TrieDestroy(p);
    }

    return p;
}

Trie:ModuleUnit_Merge(const T_ModuleUnit:iModuleUnit, const Trie:tParams1, const Trie:tParams2) {
    return ModuleType_MergeParams(ModuleUnit__GetValue(iModuleUnit, ModuleUnit_Type), tParams1, tParams2);
}
