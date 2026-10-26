#include <amxmodx>
#include <regex>
#include <ParamsController>
#include <VipModular>

new RealCurrentMapName[32];

DefaultObjects_Limit_Map_Register() {
    VipM_Limits_RegisterType("Map", true, false);
    VipM_Limits_AddParamsEx("Map",
        PCParam("Map", DEFAULT_PARAMS_SHORT_STR_NAME),
        PCParam("Regexp", DEFAULT_PARAMS_REGEXP_NAME),
        PCParam("Real", DEFAULT_PARAMS_BOOL_NAME),
        PCParam("Prefix", DEFAULT_PARAMS_BOOL_NAME)
    );
    VipM_Limits_RegisterTypeEvent("Map", Limit_OnCheck, "@DefaultObjects_Limit_Map_OnCheck");

    rh_get_mapname(RealCurrentMapName, charsmax(RealCurrentMapName), MNT_TRUE);
}

static DefaultObjects_Limit_Map_GetCurrentName(out[], const outLen, const bool:real) {
    if (real) {
        copy(out, outLen, RealCurrentMapName);
    } else {
        rh_get_mapname(out, outLen, MNT_SET);
    }
}

@DefaultObjects_Limit_Map_OnCheck(const Trie:p, const playerIndex) {
    new bool:real = PCGet_Bool(p, "Real", false);

    if (real && TrieKeyExists(p, "$Result")) {
        new cachedResult;
        TrieGetCell(p, "$Result", cachedResult);
        return cachedResult;
    }

    static currentMapName[32];
    DefaultObjects_Limit_Map_GetCurrentName(currentMapName, charsmax(currentMapName), real);

    new result;
    if (PCGet_Regexp(p, "Regexp") != REGEX_PATTERN_FAIL) {
        result = PCGet_RegexpMatch(p, "Regexp", currentMapName);
    } else {
        static map[32];
        new len = PCGet_Str(p, "Map", map, charsmax(map));

        if (!len) {
            result = false;
        } else {
            if (!PCGet_Bool(p, "Prefix", false)) {
                len = 0;
            }
            result = equali(map, currentMapName, len);
        }
    }

    if (real) {
        TrieSetCell(p, "$Result", result);
    }

    return result;
}
