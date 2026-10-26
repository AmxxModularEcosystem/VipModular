#include <amxmodx>
#include <reapi>
#include <ItemsController>
#include <ParamsController>

DefaultObjects_ItemType_Armor_Register() {
    new T_IC_ItemType:type = IC_ItemType_SimpleRegister(
        .name = "Armor",
        .onGive = "@OnArmorGive"
    );
    IC_ItemType_AddParams(type,
        PCParam("Armor", DEFAULT_PARAMS_INT_NAME, true),
        PCParam("MaxArmor", DEFAULT_PARAMS_INT_NAME),
        PCParam("SetArmor", DEFAULT_PARAMS_BOOL_NAME),
        PCParam("Helmet", DEFAULT_PARAMS_BOOL_NAME)
    );
}

@OnArmorGive(const playerIndex, const Trie:p) {
    new armor = PCGet_Int(p, "Armor");
    new ArmorType:helm = PCGet_Bool(p, "Helmet", false) ? ARMOR_VESTHELM : ARMOR_KEVLAR;

    if (!PCGet_Bool(p, "SetArmor", false)) {
        armor = min(rg_get_user_armor(playerIndex) + armor, PCGet_Int(p, "MaxArmor", 100));
    }

    rg_set_user_armor(playerIndex, armor, helm);
}
