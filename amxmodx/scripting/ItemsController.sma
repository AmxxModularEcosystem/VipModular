#include <amxmodx>
#include <ParamsController>
#include <ItemsController>
#include "VipM/Utils"
#include "VipM/Forwards"

public stock const PluginName[] = "Items Controller";
public stock const PluginVersion[] = IC_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = "https://github.com/ArKaNeMaN/amxx-VipModular-pub";
public stock const PluginDescription[] = "Unified interface for items giving.";

#include "ItemsController/Objects/Items/Instance"
#include "ItemsController/DefaultObjects/Registrar"

public plugin_precache() {
    PluginInit();
}

PluginInit() {
    CallOnce();

    register_plugin(PluginName, PluginVersion, PluginAuthor);
    register_library(IC_LIBRARY);
    PCCvar_Const(IC_VERSION_CVAR, IC_VERSION);
    ParamsController_Init();
    Forwards_Init();
    
    ItemInstance_Init();
    Forwards_RegAndCall("VipM_IC_OnInitTypes", ET_IGNORE); // deprecated
}

#include "ItemsController/API/ItemType"
#include "ItemsController/API/Item"
#include "ItemsController/API/Compat"

public plugin_natives() {
    set_native_filter("@NativeFilter");

    register_native("IC_Init", "@_Init");
    register_native("VipM_IC_Init", "@_Init"); // deprecated

    API_ItemType_Register();
    API_Item_Register();
    API_Compat_Register();
}

@_Init() {
    PluginInit();
}

@NativeFilter(const name[], index, trap) {
    if (DefaultObjects_HandleNativeFilter(name, trap)) {
        return PLUGIN_HANDLED;
    }

    return PLUGIN_CONTINUE;
}
