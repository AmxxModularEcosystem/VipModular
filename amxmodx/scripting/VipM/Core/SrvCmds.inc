#if defined _vipmodular_src_SrvCmds_included
    #endinput
#endif
#define _vipmodular_src_SrvCmds_included

#include <amxmodx>
#include "VipM/ArrayTrieUtils"

#include "VipM/Core/Objects/Modules/Type"
#include "VipM/Core/Objects/Modules/Unit"
#include "VipM/Core/Objects/Limits/Type"
#include "VipM/Core/Objects/Limits/Unit"
#include "VipM/Core/Objects/Param"

#if !defined GITHUB_ACTIONS_USED
new bool:__VIPM_INFO_COMPILED_BY_GH_ACTIONS = false;
#else
new bool:__VIPM_INFO_COMPILED_BY_GH_ACTIONS = bool:(GITHUB_ACTIONS_USED);
#endif

SrvCmds_Init() {
    register_srvcmd("vipm_info", "@SrvCmd_Info");
    register_srvcmd("vipm_modules", "@SrvCmd_Modules");
    register_srvcmd("vipm_limits", "@SrvCmd_Limits");
}

@SrvCmd_Info() {
    new amxxVersion[32];
    get_amxx_verstring(amxxVersion, charsmax(amxxVersion));

    server_print("╓─────────");
    server_print("║ %s v%s by %s:", PluginName, VIPM_VERSION, PluginAuthor);
    server_print("║ │");
    server_print("║ ├─Contacts:");
    server_print("║ │ ├─GitHub Acc: https://github.com/arkaneman");
    server_print("║ │ ├─GitHub Org: https://github.com/AmxxModularEcosystem");
    server_print("║ │ ├─hlds.run: https://hlds.run/members/26/");
    server_print("║ │ └─TG: https://t.me/arkaneman");
    server_print("║ ├─Support:");
    server_print("║ │ ├─GitHub Issues: https://github.com/AmxxModularEcosystem/VipModular/issues");
    server_print("║ │ ├─hlds.run Resource: https://hlds.run/resources/2/");
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
    server_print("║ │ ├─Runned by AmxModX v%s", amxxVersion);
    server_print("║ │ └─Builded by GitHub Actions?: %s", __VIPM_INFO_COMPILED_BY_GH_ACTIONS ? "Yes" : "No");
    server_print("║ │");
    server_print("║ └─GitHub repo: https://github.com/AmxxModularEcosystem/VipModular");
    server_print("╙─────────");
}

@SrvCmd_Modules() {
    server_print("╔═════╤══════════════════════════════════╤════════╗");
    server_print("║  #  │ Module name                      │ Status ║");
    server_print("╟─────┼──────────────────────────────────┼────────╢");
    new Array:types = ModuleType_GetAll();
    ArrayForeachArray2 (types: i => typeObject[S_ModuleType]) {
        server_print("║ %03d │ %-32s │ %-6s ║", i + 1, typeObject[ModuleType_Name], typeObject[ModuleType_Active] ? "On" : "Off");
    }
    ArrayDestroy(types);
    server_print("╟─────┴──────────────────────────────────┴────────╢");
    server_print("║ Total: %-4d                                     ║", ModuleType_GetCount());
    server_print("╚═════════════════════════════════════════════════╝");

    return PLUGIN_HANDLED;
}

@SrvCmd_Limits() {
    server_print("╔═════╤══════════════════════════════════╤════════╤════════╤════════════╗");
    server_print("║  #  │ Limit name                       │ Params │ Static │ For player ║");
    server_print("╟─────┼──────────────────────────────────┼────────┼────────┼────────────╢");

    new Array:types = LimitType_GetAll();
    ArrayForeachArray2 (types: i => typeObject[S_LimitType]) {
        server_print("║ %03d │ %-32s │ %-6d │ %-6s │ %-10s ║",
            i + 1, typeObject[LimitType_Name],
            ArraySizeSafe(typeObject[LimitType_Params]),
            typeObject[LimitType_Static] ? "Yes" : "No",
            typeObject[LimitType_ForPlayer] ? "Yes" : "No"
        );
    }
    ArrayDestroy(types);

    server_print("╟─────┴──────────────────────────────────┴────────┴────────┴────────────╢");
    server_print("║ Total: %-4d                                                           ║", LimitType_GetCount());
    server_print("╚═══════════════════════════════════════════════════════════════════════╝");

    return PLUGIN_HANDLED;
}
