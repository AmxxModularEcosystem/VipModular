#include <amxmodx>
#include <reapi>
#include <VipModular>
#include "VipM/Utils"

#define RELOAD_ON_PLAYER_SPAWN 1
#define RELOAD_ON_ROUND_START 1
#define RELOAD_ON_ROUND_END 1

public stock const PluginName[] = "[VipM] Misc";
public stock const PluginVersion[] = _VIPM_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = _VIPM_PLUGIN_URL;
public stock const PluginDescription[] = "Auto reload player's privilegies.";

public VipM_OnLoaded() {
    register_plugin(PluginName, PluginVersion, PluginAuthor);
    
    #if RELOAD_ON_PLAYER_SPAWN
        RegisterHookChain(RG_CBasePlayer_Spawn, "@ReloadPlayer", false);
    #endif
    
    #if RELOAD_ON_ROUND_START
        RegisterHookChain(RG_CSGameRules_RestartRound, "@ReloadAllPlayers", false);
    #endif
    
    #if RELOAD_ON_ROUND_END
        RegisterHookChain(RG_RoundEnd, "@ReloadAllPlayers", false);
    #endif

    register_srvcmd("vipm_update_users", "@SrvCmd_ReloadPlayers");
    register_srvcmd("vipm_reload_players", "@SrvCmd_ReloadPlayers");
    register_srvcmd("vipm_reload_player", "@SrvCmd_ReloadPlayer");
}

@ReloadPlayer(const playerIndex) {
    VipM_UserUpdate(playerIndex);
}

@ReloadAllPlayers() {
    for (new playerIndex = 1; playerIndex <= MAX_PLAYERS; ++playerIndex) {
        if (
            is_user_connected(playerIndex)
            && !is_user_bot(playerIndex)
            && !is_user_hltv(playerIndex)
        ) {
            @ReloadPlayer(playerIndex);
        }
    }
}

@SrvCmd_ReloadPlayers() {
    @ReloadAllPlayers();
}

@SrvCmd_ReloadPlayer() {
    enum {Arg_PlayerIndex = 1}

    if (read_argc() < Arg_PlayerIndex) {
        server_print("Invalid command params.");
        server_print("Usage: vipm_reload_players <playerIndex>");
        return;
    }

    new playerIndex = read_argv_int(Arg_PlayerIndex);

    @ReloadPlayer(playerIndex);
}
