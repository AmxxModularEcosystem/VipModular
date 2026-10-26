#include <amxmodx>
#include "VipM/Utils"
#include "VipM/Core/VipsManager"

API_Main_Init() {
    register_native("VipM_UserUpdate", "@_UserUpdate");
    register_native("VipM_Json_LogForFile", "@_Json_LogForFile");
}

@_UserUpdate() {
    enum {Arg_UserId = 1}

    new UserId = get_param(Arg_UserId);
    if (!is_user_connected(UserId)) {
        return;
    }
    
    VipsManager_UserReload(UserId);
}

// native VipM_Json_LogForFile(const JSON:jValue, const sPrefix[], const sMessage[], any:...);
@_Json_LogForFile() {
    enum {Arg_jValue = 1, Arg_sPrefix, Arg_sMessage, Arg_fmtArgs}

    static sMessage[512], sPrefix[32];
    vdformat(sMessage, charsmax(sMessage), Arg_sMessage, Arg_fmtArgs);
    get_string(Arg_sPrefix, sPrefix, charsmax(sPrefix));

    PCJson_LogForFile(JSON:get_param(Arg_jValue), sPrefix, sMessage);
}
