#include <amxmodx>
#include <VipModular>

DefaultObjects_Limit_Never_Register() {
    VipM_Limits_RegisterType("Never", false, true);
    VipM_Limits_SetStaticValue("Never", false);
}
