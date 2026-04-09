#include <amxmodx>
#include <reapi>
#include <json>
#include <ItemsController>
#include <ParamsController>

static Float:PlayerSpeedMult[MAX_PLAYERS + 1] = {1.0, ...};

DefaultObjects_ItemType_Speed_Register() {
    new T_IC_ItemType:type = IC_ItemType_SimpleRegister(
        .name = "Speed",
        .onGive = "@ItemType_Speed_OnGive"
    );
    IC_ItemType_AddParams(type,
        "Multiplier", DEFAULT_PARAMS_FLOAT_NAME, true
    );
    
    RegisterHookChain(RG_CBasePlayer_ResetMaxSpeed, "@ItemType_Speed_OnPlayerResetSpeedPost", true);
    RegisterHookChain(RG_CBasePlayer_Spawn, "@ItemType_Speed_OnPlayerSpawnPre", false);
}

@ItemType_Speed_OnGive(const playerIndex, const Trie:p) {
    PlayerSpeedMult[playerIndex] = PCGet_Float(p, "Multiplier", 1.0);
    ItemType_Speed_MultUserSpeed(playerIndex, PlayerSpeedMult[playerIndex]);
}

@ItemType_Speed_OnPlayerSpawnPre(const playerIndex) {
    PlayerSpeedMult[playerIndex] = 1.0;
}

@ItemType_Speed_OnPlayerResetSpeedPost(const playerIndex) {
    ItemType_Speed_MultUserSpeed(playerIndex, PlayerSpeedMult[playerIndex]);
}

static ItemType_Speed_MultUserSpeed(const playerIndex, const Float:mult) {
    if (mult != 1.0) {
        set_entvar(playerIndex, var_maxspeed, Float:get_entvar(playerIndex, var_maxspeed) * mult);
    }
}
