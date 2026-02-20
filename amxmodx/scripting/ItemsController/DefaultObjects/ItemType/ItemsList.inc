#include <amxmodx>
#include <reapi>
#include <ItemsController>
#include <ParamsController>

DefaultObjects_ItemType_ItemsList_Register() {
    new T_IC_ItemType:type = IC_ItemType_SimpleRegister(
        .name = "ItemsList",
        .onGive = "@OnItemsListGive"
    );
    IC_ItemType_AddParams(type,
        "Items", IC_PARAM_TYPE_ITEMS_NAME, true
    );
}

@OnItemsListGive(const playerIndex, const Trie:p) {
    return PCGet_IcItemsGive(p, "Items", playerIndex) ? IC_RET_GIVE_SUCCESS : IC_RET_GIVE_FAIL; 
}
