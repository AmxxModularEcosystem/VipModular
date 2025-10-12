#include <amxmodx>
#include <ParamsController>
#include <VipModular>

new RealCurrentMapName[32];

// TODO: Указывание нескольких карт/префиксов. Возможно отдельным лимитом.

DefaultObjects_Limit_Map_Register() {
    VipM_Limits_RegisterType("Map", true, false);
    VipM_Limits_AddParamsEx("Map",
        "Map", DEFAULT_PARAMS_SHORT_STR_NAME, true,
        "Real", DEFAULT_PARAMS_BOOL_NAME, false,
        "Prefix", DEFAULT_PARAMS_BOOL_NAME, false
    );
    VipM_Limits_RegisterTypeEvent("Map", Limit_OnCheck, "@DefaultObjects_Limit_Map_OnCheck");

    rh_get_mapname(RealCurrentMapName, charsmax(RealCurrentMapName), MNT_TRUE);
}

@DefaultObjects_Limit_Map_OnCheck(const Trie:p, const playerIndex) {
    static map[32];
    new len = PCGet_Str(p, "Map", map, charsmax(map));

    if (!PCGet_Bool(p, "Prefix", false)) {
        len = 0;
    }

    if (PCGet_Bool(p, "Real", false)) {
        return equali(map, RealCurrentMapName, len);
    } else {
        static currentMapName[32];
        rh_get_mapname(currentMapName, charsmax(currentMapName), MNT_SET);

        return equali(map, currentMapName, len);
    }
}
