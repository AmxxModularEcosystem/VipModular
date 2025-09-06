#if defined _vipmodular_src_SrvCmds_included
    #endinput
#endif
#define _vipmodular_src_SrvCmds_included

#include <amxmodx>
#include "VipM/ArrayTrieUtils"
#include "VipM/ArrayMap"

#include "VipM/Core/Objects/Modules/Type"
#include "VipM/Core/Objects/Modules/Unit"
#include "VipM/Core/Objects/Limits/Type"
#include "VipM/Core/Objects/Limits/Unit"
#include "VipM/Core/Objects/Param"

// TODO: Дописать в параметрах компиляции в экшонах
#if !defined VIPM_INFO_COMPILED_BY_GH_ACTIONS
new bool:__VIPM_INFO_COMPILED_BY_GH_ACTIONS = false;
#else
new bool:__VIPM_INFO_COMPILED_BY_GH_ACTIONS = bool:(VIPM_INFO_COMPILED_BY_GH_ACTIONS);
#endif

SrvCmds_Init() {
    register_srvcmd("vipm_info", "@SrvCmd_Info");
    
    register_srvcmd("vipm_modules", "@SrvCmd_Modules");

    register_srvcmd("vipm_limits", "@SrvCmd_Limits");
}

@SrvCmd_Info() {
    new sAmxxVersion[32];
    get_amxx_verstring(sAmxxVersion, charsmax(sAmxxVersion));

    server_print("╓─────────");
    server_print("║ %s v%s by %s:", PluginName, VIPM_VERSION, PluginAuthor);
    server_print("║ │");
    server_print("║ ├─Contacts:");
    server_print("║ │ ├─GitHub: https://github.com/akraneman");
    server_print("║ │ ├─Dev-CS: https://dev-cs.ru/members/949/");
    server_print("║ │ ├─VK: https://vk.com/akraneman");
    server_print("║ │ └─TG: https://t.me/arkaneman");
    server_print("║ ├─Support:");
    server_print("║ │ ├─GitHub Issues: https://github.com/ArKaNeMaN/amxx-VipModular-pub/issues");
    server_print("║ │ ├─Dev-CS Resource: https://dev-cs.ru/resources/1466/");
    server_print("║ │ └─TG Forum: https://t.me/arkanaplugins/5");
    server_print("║ │");
    server_print("║ ├─Stats:");
    server_print("║ │ ├─Modules:");
    server_print("║ │ │ ├─Modules count: %d", ModuleType_GetCount());
    server_print("║ │ │ └─Module units count: %d", ModuleUnit_GetCount());
    server_print("║ │ ├─Limits:");
    server_print("║ │ │ ├─Types count: %d", LimitType_GetCount());
    server_print("║ │ │ └─Limit units count: %d", LimitUnit_GetCount());
    server_print("║ │ ├─Vips:");
    server_print("║ │ │ └─Vips count: %d", VipUnit_GetCount());
    server_print("║ │ ├─Compiled by AmxModX v%s", AMXX_VERSION_STR);
    server_print("║ │ ├─Runned by AmxModX v%s", sAmxxVersion);
    server_print("║ │ └─Builded by GitHub Actions?: %s", __VIPM_INFO_COMPILED_BY_GH_ACTIONS ? "Yes" : "No");
    server_print("║ │");
    server_print("║ └─GitHub repo: https://github.com/ArKaNeMaN/amxx-VipModular-pub");
    server_print("╙─────────");
}

@SrvCmd_Modules() {
    server_print("╔═════╤══════════════════════════════════╤════════╗");
    server_print("║  #  │ Module name                      │ Status ║");
    server_print("╟─────┼──────────────────────────────────┼────────╢");
    new Array:aModuleTypes = ModuleType_GetAll();
    ArrayForeachArray2 (aModuleTypes: i => ModuleType[S_ModuleType]) {
        server_print("║ %03d │ %-32s │ %-6s ║", i + 1, ModuleType[ModuleType_Name], ModuleType[ModuleType_Active] ? "On" : "Off");
    }
    server_print("╟─────┴──────────────────────────────────┴────────╢");
    server_print("║ Total: %-5d                                    ║", ModuleType_GetCount());
    server_print("╚═════════════════════════════════════════════════╝");

    return PLUGIN_HANDLED;
}

@SrvCmd_Limits() {
    server_print("╔═════╤══════════════════════════════════╤════════╤════════╤════════════╗");
    server_print("║  #  │ Limit name                       │ Params │ Static │ For player ║");
    server_print("╟─────┼──────────────────────────────────┼────────┼────────┼────────────╢");

    new Array:aLimitTypes = LimitType_GetAll();
    ArrayForeachArray2 (aLimitTypes: i => LimitType[S_LimitType]) {
        server_print("║ %03d │ %-32s │ %-6d │ %-6s │ %-10s ║",
            i + 1, LimitType[LimitType_Name],
            ArraySizeSafe(LimitType[LimitType_Params]),
            LimitType[LimitType_Static] ? "Yes" : "No",
            LimitType[LimitType_ForPlayer] ? "Yes" : "No"
        );
    }
    ArrayDestroy(aLimitTypes);

    server_print("╟─────┴──────────────────────────────────┴────────┴────────┴────────────╢");
    server_print("║ Total: %-5d                                                          ║", LimitType_GetCount());
    server_print("╚═══════════════════════════════════════════════════════════════════════╝");

    return PLUGIN_HANDLED;
}
