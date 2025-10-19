#include <amxmodx>
#include <json>
#include <reapi>
#include <VipModular>
#include <ItemsController>

public stock const PluginName[] = "[VipM-M] Spawn Items";
public stock const PluginVersion[] = _VIPM_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = _VIPM_PLUGIN_URL;
public stock const PluginDescription[] = "Vip modular`s module - Spawn Items";

new const MODULE_NAME[] = "SpawnItems";

public VipM_Modules_OnInited() {
    register_plugin(PluginName, PluginVersion, PluginAuthor);
    IC_Init();
    
    VipM_Modules_Register(MODULE_NAME);
    VipM_Modules_AddParamsEx(MODULE_NAME,
        "Items", IC_PARAM_TYPE_ITEMS_NAME, true,
        "Limits", VIPM_PARAM_TYPE_LIMITS_NAME, false
    );
    VipM_Modules_RegisterEvent(MODULE_NAME, Module_OnActivated, "@OnModuleActivate");
}

@OnModuleActivate() {
    RegisterHookChain(RG_CBasePlayer_Spawn, "@OnPlayerSpawned", true);
}

@OnPlayerSpawned(const playerIndex) {
    RequestFrame("@GivePlayerItems", playerIndex);
}

@GivePlayerItems(const playerIndex) {
    if (!is_user_alive(playerIndex)) {
        return;
    }
    
    if (!VipM_Modules_HasModule(MODULE_NAME, playerIndex)) {
        return;
    }
    
    new Trie:p = VipM_Modules_GetParams(MODULE_NAME, playerIndex);

    if (!PCGet_VipmLimitsCheck(p, "Limits", playerIndex, Limit_Exec_AND)) {
        return;
    }
    
    PCGet_IcItemsGive(p, "Items", playerIndex);
}
