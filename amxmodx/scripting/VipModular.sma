#include <amxmodx>
#include <VipModular>
#include <ParamsController>
#include "VipM/DebugMode"
#include "VipM/ArrayTrieUtils"
#include "VipM/Utils"
#include "VipM/Forwards"

public stock const PluginName[] = "Vip Modular";
public stock const PluginVersion[] = _VIPM_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = _VIPM_PLUGIN_URL;
public stock const PluginDescription[] = "Modular vip system";

#include "VipM/Core/Objects/Modules/Type"
#include "VipM/Core/Objects/Limits/Type"
#include "VipM/Core/VipsManager"
#include "VipM/Core/SrvCmds"
#include "VipM/DefaultObjects/Registrar"

public plugin_precache() {
    register_plugin(PluginName, PluginVersion, PluginAuthor);
    register_library(VIPM_LIBRARY);
    CreateConstCvar("vipm_version", PluginVersion);

    Forwards_Init();

    VipsManager_Init();
    ModuleType_Init();
    SrvCmds_Init();
    Forwards_RegAndCall("VipM_OnInitModules", ET_IGNORE); // deprecated
    
    VipsManager_SetRootDir(PCPath_iMakePath(VIPM_CONFIGS_FOLDER_NAME));
    VipsManager_LoadFromFile(PCPath_iMakePath(fmt("%s/%s", VIPM_CONFIGS_FOLDER_NAME, VIPM_VIPS_CONFIG_FILE_PATH)));
    VipsManager_LoadFromFolder(PCPath_iMakePath(fmt("%s/%s", VIPM_CONFIGS_FOLDER_NAME, VIPM_VIPS_CONFIG_DIR_PATH)));

    ModuleType_ActivateUsed();

    Forwards_RegAndCall("VipM_OnLoaded", ET_IGNORE);
    server_print("[%s v%s] Loaded %d config units.", PluginName, PluginVersion, VipsManager_VipsCount());
    Dbg_PrintServer("Vip Modular run in debug mode!");
}

public client_authorized(playerIndex, const steamId[]) {
    DefaultObjects_OnClientAuth(playerIndex, steamId);
}

public client_putinserver(UserId) {
    if (is_user_bot(UserId)) {
        return;
    }

    RequestFrame("@CallUserUpdate", UserId);
}

@CallUserUpdate(UserId) {
    if (is_user_connected(UserId)) {
        VipsManager_UserReload(UserId);
    }
}

public client_disconnected(UserId) {
    VipsManager_UserReset(UserId);
}

#include "VipM/Core/API/Main"
#include "VipM/Core/API/Limits"
#include "VipM/Core/API/Modules"
public plugin_natives() {
    API_Main_Init();
    API_Limits_Init();
    API_Modules_Init();

    set_native_filter("@NativeFilter");
}

@NativeFilter(const name[], const index, const trap) {
    new ret = PLUGIN_CONTINUE;

    if (DefaultObjects_NativeFilter(name, !!trap)) {
        ret = PLUGIN_HANDLED;
    }

    return ret;
}
