#include <amxmodx>
#include <reapi>
#include <ItemsController>
#include <ParamsController>

DefaultObjects_ItemType_Function_Register() {
    new T_IC_ItemType:type = IC_ItemType_SimpleRegister(
        .name = "Function",
        .onGive = "@OnFunctionGive"
    );
    IC_ItemType_AddParams(type,
        "Plugin", DEFAULT_PARAMS_STR_NAME, true,
        "Function", DEFAULT_PARAMS_STR_NAME, true
    );

    // For AES-Bonuses compat
    IC_ItemType_AddParams(type,
        "Value", DEFAULT_PARAMS_INT_NAME, false,
        "Flags", DEFAULT_PARAMS_SHORT_STR_NAME, false,
        "Days", DEFAULT_PARAMS_INT_NAME, false
    );
}

@OnFunctionGive(const playerIndex, const Trie:p) {
    if (callfunc_begin(PCGet_iStr(p, "Function"), PCGet_iStr(p, "Plugin")) > 0) {
        callfunc_push_int(playerIndex);
        
        new flags[PARAM_SHORT_STRING_MAX_LEN]; // В AES 30, но почему бы не расширить до 64)
        PCGet_Str(p, "Flags", flags, charsmax(flags));
        if (flags[0] != EOS) {
            callfunc_push_str(flags);

            new days = PCGet_Int(p, "Days", 0);
            if (days > 0) {
                callfunc_push_int(days);
            }
        } else {
            callfunc_push_int(PCGet_Int(p, "Value", 0));
        }

        // Вообще не используется в AES
        // if (psh) {
        //     callfunc_push_int(psh);
        // }

        return callfunc_end() > 0 ? IC_RET_GIVE_SUCCESS : IC_RET_GIVE_FAIL;
    }

    abort(AMX_ERR_PARAMS, "Can't call function '%s' in plugin '%s'.", PCGet_iStr(p, "Function"), PCGet_iStr(p, "Plugin"));
    return IC_RET_GIVE_FAIL;
}

/**
 * {
 *  "Item": "Function",
 *  "Plugin": "ItemsController.amxx",
 *  "Function": "@IC_ItemType_Function_TestFunc_Base"
 * }
 */
@IC_ItemType_Function_TestFunc_Base(const playerIndex) {
    client_print_color(playerIndex, print_team_default, "@IC_ItemType_Function_TestFunc_Base(%n)", playerIndex);
}

/**
 * {
 *  "Item": "Function",
 *  "Plugin": "ItemsController.amxx",
 *  "Function": "@IC_ItemType_Function_TestFunc_Value",
 *  "Value": 123
 * }
 */
@IC_ItemType_Function_TestFunc_Value(const playerIndex, const value) {
    client_print_color(playerIndex, print_team_default, "@IC_ItemType_Function_TestFunc_Value(%n, %d)", playerIndex, value);
}

/**
 * {
 *  "Item": "Function",
 *  "Plugin": "ItemsController.amxx",
 *  "Function": "@IC_ItemType_Function_TestFunc_Flags",
 *  "Flags": "qwe"
 * }
 */
@IC_ItemType_Function_TestFunc_Flags(const playerIndex, const flags[]) {
    client_print_color(playerIndex, print_team_default, "@IC_ItemType_Function_TestFunc_Flags(%n, %s)", playerIndex, flags);
}

/**
 * {
 *  "Item": "Function",
 *  "Plugin": "ItemsController.amxx",
 *  "Function": "@IC_ItemType_Function_TestFunc_FlagsDays",
 *  "Flags": "qwe",
 *  "Days": 321
 * }
 */
@IC_ItemType_Function_TestFunc_FlagsDays(const playerIndex, const flags[], const days) {
    client_print_color(playerIndex, print_team_default, "@IC_ItemType_Function_TestFunc_FlagsDays(%n, %s, %d)", playerIndex, flags, days);
}
