#include <amxmodx>
#include <ParamsController>
#include <VipModular>
#include "VipM/Utils"

DefaultObjects_Limit_GameTime_Register() {
    VipM_Limits_RegisterType("GameTime", true, false);
    VipM_Limits_AddTypeParams("GameTime",
        "Min", ptFloat, false, // TODO: Добавить типы "Time" в контроллер параметров
        "Max", ptFloat, false
    );
    VipM_Limits_RegisterTypeEvent("GameTime", Limit_OnCheck, "@DefaultObjects_Limit_GameTime_OnCheck");
}


@DefaultObjects_Limit_GameTime_OnCheck(const Trie:p) {
    new Float:gameTime = get_gametime();

    new Float:min;
    if (TrieGetCell(p, "Min", min) && gameTime < min) {
        return false;
    }

    new Float:max;
    if (TrieGetCell(p, "Max", max) && gameTime > max) {
        return false;
    }

    return true;
}
