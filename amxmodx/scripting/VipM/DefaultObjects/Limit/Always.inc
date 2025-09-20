#include <amxmodx>
#include <VipModular>

DefaultObjects_Limit_Always_Register() {
    VipM_Limits_RegisterType("ForAll", false, true);
    VipM_Limits_SetStaticValue("ForAll", true);
    VipM_Limits_RegisterType("Always", false, true);
    VipM_Limits_SetStaticValue("Always", true);
}
