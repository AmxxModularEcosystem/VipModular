#include <amxmodx>
#include <reapi>
#include <hamsandwich>
#include <VipModular>
#include <ItemsController>
#include <ParamsController>

static bool:IsVipModularInstalled = true;

DefaultObjects_ItemType_If_Register() {
    if (!IsVipModularInstalled) {
        return;
    }

    new T_IC_ItemType:type = IC_ItemType_SimpleRegister(
        .name = "If",
        .onGive = "@OnIfGive"
    );
    IC_ItemType_AddParams(type,
        "Limits", VIPM_PARAM_TYPE_LIMITS_NAME, true,
        "Items", IC_PARAM_TYPE_ITEMS_NAME, false,
        "ElseItems", IC_PARAM_TYPE_ITEMS_NAME, false
    );
}

@OnIfGive(const playerIndex, const Trie:p) {
    if (!PCGet_VipmLimitsCheck(p, "Limits", playerIndex, Limit_Exec_AND)) {
        return PCGet_IcItemsGive(p, "ElseItems", playerIndex) ? IC_RET_GIVE_SUCCESS : IC_RET_GIVE_FAIL;
    }

    return PCGet_IcItemsGive(p, "Items", playerIndex) ? IC_RET_GIVE_SUCCESS : IC_RET_GIVE_FAIL;
}

bool:DefaultObjects_ItemType_If_NativeFilter(const name[], const trap) {
    if (equal(name, "VipM_Limits_ExecuteList")) {
        IsVipModularInstalled = !!trap;
        return true;
    }

    return false;
}
