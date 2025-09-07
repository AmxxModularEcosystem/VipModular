#include <amxmodx>
#include <reapi>
#include <hamsandwich>
#include <VipModular>
#include <ItemsController>
#include <ParamsController>

DefaultObjects_ItemType_If_Register() {
    if (!LibraryExists(VIPM_LIBRARY, LibType_Library)) {
        return;
    }

    new T_IC_ItemType:type = IC_ItemType_SimpleRegister(
        .name = "If",
        .onGive = "@OnIfGive"
    );
    IC_ItemType_AddParams(type,
        "Limits", "VipM-Limits", true,
        "Items", "IC-Items", false,
        "ElseItems", "IC-Items", false
    );
}

@OnIfGive(const playerIndex, const Trie:p) {
    if (!PCGet_VipmLimitsCheck(p, "Limits", playerIndex, Limit_Exec_AND)) {
        return PCGet_IcItemsGive(p, "ElseItems", playerIndex) ? IC_RET_GIVE_SUCCESS : IC_RET_GIVE_FAIL;
    }

    return PCGet_IcItemsGive(p, "Items", playerIndex) ? IC_RET_GIVE_SUCCESS : IC_RET_GIVE_FAIL;
}

bool:DefaultObjects_ItemType_If_NativeFilter(const name[]) {
    if (equal(name, "VipM_Limits_ExecuteList")) {
        return true;
    }

    return false;
}
