#include <amxmodx>
#include <VipModular>

enum E_ModuleParams {
    Param_Enabled = 0,
    Param_Override,
}

public stock const PluginName[] = "[VipM-M] Vip in TAB";
public stock const PluginVersion[] = _VIPM_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = _VIPM_PLUGIN_URL;
public stock const PluginDescription[] = "[VipModular-Module] Show VIP status in TAB.";

new const MODULE_NAME[] = "VipInTab";

new bool:PlayerSettings[MAX_PLAYERS + 1][E_ModuleParams];

public VipM_Modules_OnInited() {
    register_plugin(PluginName, PluginVersion, PluginAuthor);

    VipM_Modules_Register(MODULE_NAME);
    VipM_Modules_AddParamsEx(MODULE_NAME,
        PCParam("Enabled", DEFAULT_PARAMS_BOOL_NAME, true),
        PCParam("Override", DEFAULT_PARAMS_BOOL_NAME)
    );
    VipM_Modules_RegisterEvent(MODULE_NAME, Module_OnActivated, "@OnModuleActivate");
}

public VipM_OnUserUpdated(const playerIndex) {
    new Trie:Params = VipM_Modules_GetParams(MODULE_NAME, playerIndex);

    PlayerSettings[playerIndex][Param_Enabled] = PCGet_Bool(Params, "Enabled", false);
    PlayerSettings[playerIndex][Param_Override] = PCGet_Bool(Params, "Override", false);
}

@OnModuleActivate() {
    register_message(get_user_msgid("ScoreAttrib"), "@OnMsgScoreAttrib");
}

@OnMsgScoreAttrib(const messageIndex, const messageType, const messageDestination) {
    new playerIndex = get_msg_arg_int(1);
    if (!PlayerSettings[playerIndex][Param_Enabled]) {
        return;
    }

    if (
        !PlayerSettings[playerIndex][Param_Override]
        && get_msg_arg_int(2) != 0
    ) {
        return;
    }

    set_msg_arg_int(2, ARG_BYTE, (1<<2));
}
