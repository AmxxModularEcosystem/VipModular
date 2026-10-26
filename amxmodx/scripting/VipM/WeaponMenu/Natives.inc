#include <amxmodx>

public plugin_natives() {
    register_native("VipM_WeaponMenu_SetExpireStatus", "@_SetExpireStatus");
}

@_SetExpireStatus() {
    enum {Arg_UserId = 1, Arg_sStatus};

    get_string(Arg_sStatus, gUserExpireStatus[get_param(Arg_UserId)], charsmax(gUserExpireStatus[]));
}
