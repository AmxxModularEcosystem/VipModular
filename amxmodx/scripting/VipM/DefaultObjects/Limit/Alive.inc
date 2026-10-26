#include <amxmodx>
#include <VipModular>

DefaultObjects_Limit_Alive_Register() {
    VipM_Limits_RegisterType("Alive", true, true);
    
    RegisterHookChain(RG_CBasePlayer_Spawn, "@DefaultObjects_Limit_Alive_OnPlayerSpawn", true);
    RegisterHookChain(RG_CBasePlayer_Killed, "@DefaultObjects_Limit_Alive_OnPlayerKilled", true);
}

@DefaultObjects_Limit_Alive_OnPlayerSpawn(const playerIndex) {
    VipM_Limits_SetStaticValue("Alive", true, playerIndex);
}

@DefaultObjects_Limit_Alive_OnPlayerKilled(const playerIndex) {
    VipM_Limits_SetStaticValue("Alive", false, playerIndex);
}

DefaultObjects_Limit_Alive_OnClientPutInServer(const playerIndex) {
    VipM_Limits_SetStaticValue("Alive", false, playerIndex);
}
