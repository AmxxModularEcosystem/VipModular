#include <amxmodx>
#include <reapi>
#include <json>
#include <ItemsController>

DefaultObjects_ItemType_Random_Register() {
    new T_IC_ItemType:type = IC_ItemType_SimpleRegister(
        .name = "Random",
        .onGive = "@OnRandomGive"
    );
    IC_ItemType_AddParams(type,
        "Items", IC_PARAM_TYPE_ITEMS_NAME, true
    );
}

@OnRandomGive(const playerIndex, const Trie:p) {
    new Array:items = PCGet_IcItems(p, "Items");
    new iRandomIndex = random_num(0, ArraySizeSafe(items) - 1);
    new T_IC_Item:iRandomItem = ArrayGetCell(items, iRandomIndex);

    return IC_Item_Give(playerIndex, iRandomItem) ? IC_RET_GIVE_SUCCESS : IC_RET_GIVE_FAIL;
}
