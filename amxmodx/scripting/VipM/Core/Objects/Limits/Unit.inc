#if defined __vipm_core_objects_limits_unit_included
    #endinput
#endif
// Два кванта
#define __vipm_core_objects_limits_unit_included

#include <amxmodx>
#include <VipM/Limits>
#include "VipM/Utils"
#include "VipM/ArrayTrieUtils"

#include "VipM/Core/Objects/Limits/Type"

// T_LimitUnit

enum _:S_LimitUnit {
    T_LimitType:LimitUnit_Type,
    Trie:LimitUnit_Params,
}

static Array:LimitUnits = Invalid_Array;

LimitUnit_Init() {
    CallOnce();
    log_amx("Init limit units...");

    LimitType_Init();
    
    LimitUnits = ArrayCreate(S_LimitUnit, 1);
    
    Forwards_RegAndCall("VipM_Limits_OnInited", ET_IGNORE);
    log_amx("Limit units inited.");
}

LimitUnit_GetCount() {
    return ArraySizeSafe(LimitUnits);
}

static T_LimitUnit:LimitUnit__Construct(const T_LimitType:type, const Trie:p) {
    if (LimitUnits == Invalid_Array) {
        abort(AMX_ERR_GENERAL, "Attempt to create limit unit before init.");
        return Invalid_LimitUnit;
    }
    
    new unitObject[S_LimitUnit];

    unitObject[LimitUnit_Type] = type;
    unitObject[LimitUnit_Params] = p;

    return T_LimitUnit:ArrayPushArray(LimitUnits, unitObject);
}

static bool:LimitUnit__Get(const T_LimitUnit:limit, unitObject[S_LimitUnit], const bool:orFail = true) {
    if (_:limit < 0 || _:limit >= LimitUnit_GetCount()) {
        if (orFail) {
           abort(AMX_ERR_NOTFOUND, "Invalid limit unit index (%d).", limit);
        }
        
        return false;
    }

    ArrayGetArray(LimitUnits, _:limit, unitObject);
    return true;
}

T_LimitUnit:LimitUnit_Read(const JSON:valueJson) {
    static Trie:cache = Invalid_Trie;
    TrieCreateIfNotCreated(cache);
    
    new JSON:linked, T_LimitUnit:cached = Invalid_LimitUnit, linkPath[PLATFORM_MAX_PATH];
    if (PCJson_HandleCachedLinkedValue(cache, valueJson, linked, cached, linkPath, charsmax(linkPath)) == PCJson_CachedLinkedValue_Cached) {
        return cached;
    }

    new T_LimitType:type = PCSingle_ObjVipmLimitType(linked, "Limit", .orFail = true);
    if (type == Invalid_LimitType) {
        PCJson_FreeLinked(linked, valueJson);
        return Invalid_LimitUnit;
    }

    new Trie:p = Invalid_Trie;
    if (!LimitType_IsStatic(type)) {
        p = LimitType_ReadParams(type, linked);
        if (p == Invalid_Trie) {
            PCJson_FreeLinked(linked, valueJson);
            return Invalid_LimitUnit;
        }
    }

    cached = LimitUnit__Construct(type, p);
    if (linkPath[0] != EOS && cached != Invalid_LimitUnit) {
        TrieSetCell(cache, linkPath, cached);
    }

    PCJson_FreeLinked(linked, valueJson);
    return cached;
}

Array:LimitUnit_ReadList(const JSON:limitsJson, &Array:append = Invalid_Array) {
    if (append == Invalid_Array) {
        append = ArrayCreate(1, 1);
    }

    if (!json_is_array(limitsJson)) {
        new T_LimitUnit:limit = LimitUnit_Read(limitsJson);
        if (limit != Invalid_LimitUnit) {
            ArrayPushCell(append, limit);    
        }
    } else {
        json_array_foreach_value (limitsJson: i => limitJson) {
            append = LimitUnit_ReadList(limitJson, append);
            json_free(limitJson);
        }
    }

    return append;
}

bool:LimitUnit_Execute(const T_LimitUnit:limit, const UserId = 0) {
    new unitObject[S_LimitUnit];
    LimitUnit__Get(limit, unitObject);

    return LimitType_Execute(unitObject[LimitUnit_Type], unitObject[LimitUnit_Params], UserId);
}

bool:LimitUnit_ExecuteList(
    const Array:aLimitUnits,
    const UserId = 0,
    const E_LimitsExecType:iOperator
) {
    new bool:bXor = false;

    for (new i = 0, ii = ArraySizeSafe(aLimitUnits); i < ii; ++i) {
        new T_LimitUnit:limit = T_LimitUnit:ArrayGetCell(aLimitUnits, i);
        new bool:bRes = LimitUnit_Execute(limit, UserId);

        switch (iOperator) {
            case Limit_Exec_OR: {
                if (bRes) {
                    return true;
                }
            }
            case Limit_Exec_AND: {
                if (!bRes) {
                    return false;
                }
            }
            case Limit_Exec_XOR: {
                if (bRes && bXor) {
                    return false;
                }
                bXor = bXor || bRes;
            }
        }
    }
    
    switch (iOperator) {
        case Limit_Exec_OR: {
            return false;
        }
        case Limit_Exec_AND: {
            return true;
        }
        case Limit_Exec_XOR: {
            return bXor;
        }
    }

    return false;
}
