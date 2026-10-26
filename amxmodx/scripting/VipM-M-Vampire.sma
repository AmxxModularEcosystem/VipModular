#include <amxmodx>
#include <reapi>
#include <VipModular>

public stock const PluginName[] = "[VipM-M] Vampire";
public stock const PluginVersion[] = _VIPM_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = _VIPM_PLUGIN_URL;
public stock const PluginDescription[] = "Vip modular`s module - Vampire";

new const MODULE_NAME[] = "Vampire";

public VipM_Modules_OnInited() {
    register_plugin(PluginName, PluginVersion, PluginAuthor);
    register_dictionary("VipM-Vampire.ini");

    VipM_Modules_Register(MODULE_NAME);
    VipM_Modules_AddParamsEx(MODULE_NAME,
        PCParam("ByKill", DEFAULT_PARAMS_INT_NAME),
        PCParam("ByHead", DEFAULT_PARAMS_INT_NAME),
        PCParam("ByKnife", DEFAULT_PARAMS_INT_NAME),
        PCParam("ByGrenade", DEFAULT_PARAMS_INT_NAME)
    );
    VipM_Modules_AddParamsEx(MODULE_NAME,
        PCParam("MaxHealth", DEFAULT_PARAMS_INT_NAME),
        PCParam("Limits", VIPM_PARAM_TYPE_LIMITS_NAME)
    );
    VipM_Modules_RegisterEvent(MODULE_NAME, Module_OnActivated, "@Event_ModuleActivate");
}

@Event_ModuleActivate() {
    RegisterHookChain(RG_CBasePlayer_Killed, "@Event_PlayerKilled", true);
}

@Event_PlayerKilled(const victimIndex, playerIndex, inflictorIndex) {
    if (
        playerIndex == victimIndex
        || !is_user_alive(playerIndex)
        || !is_user_connected(victimIndex)
    ) {
        return;
    }

    new Trie:p = VipM_Modules_GetParams(MODULE_NAME, playerIndex);
    if (p == Invalid_Trie) {
        return;
    }

    if (!PCGet_VipmLimitsCheck(p, "Limits", playerIndex, Limit_Exec_AND)) {
        return;
    }

    new maxHealth = PCGet_Int(p, "MaxHealth", floatround(get_entvar(playerIndex, var_max_health)));
    new currentHealth = floatround(get_entvar(playerIndex, var_health));
    if (currentHealth >= maxHealth) {
        return;
    }
    
    new healthByKill = PCGet_Int(p, "ByKill", 0);
    new addHealth = 0;
    new activeItem = get_member(playerIndex, m_pActiveItem);

    if (
        !(get_member(victimIndex, m_bitsDamageType) & DMG_SLASH)
        && is_entity(activeItem)
        && rg_get_iteminfo(activeItem, ItemInfo_iId) == CSW_KNIFE
    ) {
        addHealth = PCGet_Int(p, "ByKnife", healthByKill);
    } else if(get_member(victimIndex, m_bHeadshotKilled)) {
        addHealth = PCGet_Int(p, "ByHead", healthByKill);
    } else if (get_member(victimIndex, m_bKilledByGrenade)) {
        addHealth = PCGet_Int(p, "ByGrenade", healthByKill);
    } else {
        addHealth = healthByKill;
    }

    if (addHealth <= 0) {
        return;
    }

    new newHealth = clamp(currentHealth + addHealth, 1, maxHealth < 1 ? cellmax : maxHealth);
    set_entvar(playerIndex, var_health, float(newHealth));
    
    client_print(playerIndex, print_center, "%L", playerIndex, "VAMPIRE_HEALTH_MESSAGE", addHealth);
}
