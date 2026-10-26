#include <amxmodx>
#include <reapi>
#include <ItemsController>
#include <ParamsController>

static Float:PlayerGivenDmgMult[MAX_PLAYERS + 1] = {1.0, ...};
static Float:PlayerTakenDmgMult[MAX_PLAYERS + 1] = {1.0, ...};

DefaultObjects_ItemType_DamageMult_Register() {
    new T_IC_ItemType:type = IC_ItemType_SimpleRegister(
        .name = "DamageMult",
        .onGive = "@ItemType_DamageMult_OnGive"
    );
    IC_ItemType_AddParams(type,
        PCParam("Given", DEFAULT_PARAMS_FLOAT_NAME),
        PCParam("Taken", DEFAULT_PARAMS_FLOAT_NAME)
    );
    
    RegisterHookChain(RG_CBasePlayer_Spawn, "@ItemType_DamageMult_OnPlayerSpawnPre", false);
    RegisterHookChain(RG_CBasePlayer_TakeDamage, "@ItemType_DamageMult_OnPlayerTakeDamage", false);
}

@ItemType_DamageMult_OnGive(const playerIndex, const Trie:p) {
    PlayerGivenDmgMult[playerIndex] = PCGet_Float(p, "Given", 1.0);
    PlayerTakenDmgMult[playerIndex] = PCGet_Float(p, "Taken", 1.0);
}

@ItemType_DamageMult_OnPlayerSpawnPre(const playerIndex) {
    PlayerGivenDmgMult[playerIndex] = 1.0;
    PlayerTakenDmgMult[playerIndex] = 1.0;
}

@ItemType_DamageMult_OnPlayerTakeDamage(const victimIndex, inflictorIndex, attackerInde, Float:damage, damageType) {
    if (is_user_connected(attackerInde)) {
        damage *= PlayerGivenDmgMult[attackerInde];
    }

    if (is_user_connected(victimIndex)) {
        damage *= PlayerTakenDmgMult[victimIndex];
    }

    SetHookChainArg(4, ATYPE_FLOAT, damage);
    return HC_CONTINUE;
}
