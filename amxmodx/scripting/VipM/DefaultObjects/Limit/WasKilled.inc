#include <amxmodx>
#include <VipModular>

DefaultObjects_Limit_WasAlive_Register() {
    VipM_Limits_RegisterType("WasAlive", true, true);
    VipM_Limits_RegisterType("WasKilled", true, true);
    
    RegisterHookChain(RG_CBasePlayer_Spawn, "@DefaultObjects_Limit_WasAlive_OnPlayerSpawn", false);
}

@DefaultObjects_Limit_WasAlive_OnPlayerSpawn(const playerIndex) {
    VipM_Limits_SetStaticValue("WasAlive", !!is_user_alive(playerIndex), playerIndex);
    VipM_Limits_SetStaticValue("WasKilled", !is_user_alive(playerIndex), playerIndex);
}

DefaultObjects_Limit_WasAlive_OnClientPutInServer(const playerIndex) {
    VipM_Limits_SetStaticValue("WasAlive", false, playerIndex);
    VipM_Limits_SetStaticValue("WasKilled", true, playerIndex);
}
