#if defined __vipm_core_objects_param_included
    #endinput
#endif
#define __vipm_core_objects_param_included

#include <amxmodx>
#include <json>
#include <ParamsController>
#include <VipM/Params>

Array:CfgParam_GetFromNative(const iStartParam, const iNativeParamsCount, &Array:params = Invalid_Array) {
    const ARGS_NUM_PER_PARAM = 3;

    new NullArg = iStartParam - 1;

    if ((iNativeParamsCount - NullArg) % ARGS_NUM_PER_PARAM != 0) {
        log_error(0, "[ERROR] Invalid parameters num.");
        return Invalid_Array;
    }

    new iParamsCount = max(1, (iNativeParamsCount - NullArg) / ARGS_NUM_PER_PARAM);
    if (params == Invalid_Array) {
        params = ArrayCreate(1, iParamsCount);
    } else {
        ArrayResize(params, ArraySize(params) + iParamsCount);
    }

    for (new i = NullArg + 1; i < iNativeParamsCount; i += ARGS_NUM_PER_PARAM) {
        new paramType[PARAM_TYPE_NAME_MAX_LEN];
        switch (E_ParamType:get_param_byref(i + 1)) {
            case ptInteger: {
                paramType = DEFAULT_PARAMS_INT_NAME;
            }
            case ptFloat: {
                paramType = DEFAULT_PARAMS_FLOAT_NAME;
            }
            case ptBoolean: {
                paramType = DEFAULT_PARAMS_BOOL_NAME;
            }
            case ptString: {
                paramType = DEFAULT_PARAMS_STR_NAME;
            }
            case ptColor: {
                paramType = DEFAULT_PARAMS_RGB_NAME;
            }
            case ptVector2: {
                log_amx("[WARNING] Param type ptVector2 is deprecated and will be ignored.");
                continue;
            }
            case ptVector3: {
                log_amx("[WARNING] Param type ptVector3 is deprecated and will be ignored.");
                continue;
            }
            case ptLimit: {
                paramType = VIPM_PARAM_TYPE_LIMIT_NAME;
            }
            case ptLimits: {
                paramType = VIPM_PARAM_TYPE_LIMITS_NAME;
            }
            case ptCustom: {
                // Чтоб не ругалось
                continue;
            }
            default: {
                log_amx("[WARNING] Invalid param type (%d).", get_param_byref(i + 1));
                continue;
            }
        }

        new paramName[PARAM_KEY_MAX_LEN];
        get_string(i, paramName, charsmax(paramName));

        new bool:required = bool:get_param_byref(i + 2);

        new T_Param:param = ParamsController_Param_Construct(paramName, paramType, required);

        ArrayPushCell(params, param);
    }

    return params;
}
