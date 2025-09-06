#include <amxmodx>
#include <json>
#include <reapi>
#include <VipModular>
#include <ItemsController>
#include "VipM/Utils"
#include "VipM/DebugMode"

#include "VipM/Utils"
#include "VipM/ArrayTrieUtils"

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
        "Items", "IC-Items", true,
        "Limits", "VipM-Limits", false
    );
    VipM_Modules_RegisterEvent(MODULE_NAME, Module_OnActivated, "@OnModuleActivate");
}

@OnModuleActivate() {
    RegisterHookChain(RG_CBasePlayer_Spawn, "@OnPlayerSpawned", true);
}

@OnPlayerSpawned(const UserId) {
    RequestFrame("@GivePlayerItems", UserId);
}

@GivePlayerItems(const UserId) {
    if (!is_user_alive(UserId)) {
        return;
    }
    
    if (!VipM_Modules_HasModule(MODULE_NAME, UserId)) {
        return;
    }
    
    new Trie:p = VipM_Modules_GetParams(MODULE_NAME, UserId);

    if (!PCGet_VipmLimitsCheck(p, "Limits", UserId, Limit_Exec_AND)) {
        Dbg_Log("@GivePlayerItems(%n) Limits not passed", UserId);
        return;
    }
    
    if (PCGet_IcItemsGive(p, "Items", UserId)) {
        Dbg_Log("@GivePlayerItems(%n) Items given", UserId);
    } else {
        Dbg_Log("@GivePlayerItems(%n) Items not given", UserId);
    }
}
