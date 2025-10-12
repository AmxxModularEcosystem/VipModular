#include <amxmodx>
#include <ParamsController>
#include <VipModular>

static Float:PlayerSpawnTime[MAX_PLAYERS + 1] = {0.0, ...};

DefaultObjects_Limit_LifeTime_Register() {
    VipM_Limits_RegisterType("LifeTime", true, false);
    VipM_Limits_AddParamsEx("LifeTime",
        "Min", DEFAULT_PARAMS_TIME_INTERVAL_NAME, false,
        "Max", DEFAULT_PARAMS_TIME_INTERVAL_NAME, false
    );
    VipM_Limits_RegisterTypeEvent("LifeTime", Limit_OnCheck, "@DefaultObjects_Limit_LifeTime_OnCheck");
    
    RegisterHookChain(RG_CBasePlayer_Spawn, "@DefaultObjects_Limit_LifeTime_OnPlayerSpawn", true);
}

@DefaultObjects_Limit_LifeTime_OnPlayerSpawn(const playerIndex) {
    PlayerSpawnTime[playerIndex] = get_gametime();
}

@DefaultObjects_Limit_LifeTime_OnCheck(const Trie:p, const playerIndex) {
    new lifeTime = floatround(get_gametime() - PlayerSpawnTime[playerIndex]);

    return (
        lifeTime >= PCGet_Int(p, "Min", cellmin)
        && lifeTime <= PCGet_Int(p, "Max", cellmax)
    );
}
