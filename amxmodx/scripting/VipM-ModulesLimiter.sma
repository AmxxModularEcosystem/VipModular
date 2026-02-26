#include <amxmodx>
#include <json>
#include <VipModular>
#include <ParamsController>
#include "VipM/Utils"
#include "VipM/ArrayTrieUtils"

public stock const PluginName[] = "[VipM] Modules Limiter";
public stock const PluginVersion[] = _VIPM_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = _VIPM_PLUGIN_URL;
public stock const PluginDescription[] = "Modules activation controller";

new const CONFIG_FILE_PATH[] = "Modules.json";

new Trie:ModulesLimits = Invalid_Trie;

public VipM_Modules_OnInited() {
    register_plugin(PluginName, PluginVersion, PluginAuthor);
    
    ModulesLimits = LoadModulesLimitsFromFile(PCPath_iMakePath(fmt("%s/%s", VIPM_CONFIGS_FOLDER_NAME, CONFIG_FILE_PATH)));
}

public VipM_Modules_OnActivate(const moduleName[]) {
    if (
        ModulesLimits == Invalid_Trie
        || !TrieKeyExists(ModulesLimits, moduleName)
    ) {
        log_amx("Module `%s` is not limited.", moduleName);
        return VIPM_CONTINUE;
    }

    new Array:limits;
    TrieGetCell(ModulesLimits, moduleName, limits);
    if (!VipM_Limits_ExecuteList(limits)) {
        log_amx("Module `%s` is disabled by limits.", moduleName);
        return VIPM_STOP;
    } else {
        log_amx("Module `%s` is enabled by limits.", moduleName);
    }
    
    return VIPM_CONTINUE;
}

Trie:LoadModulesLimitsFromFile(const filePath[], &Trie:modules = Invalid_Trie) {
    if (modules == Invalid_Trie) {
        modules = TrieCreate();
    }

    new JSON:fileJson = PCJson_ParseFile(filePath);
    if (fileJson == Invalid_JSON) {
        log_error(0, "Invalid JSON syntax. File `%s`.", filePath);
        return modules;
    }

    if (!json_is_array(fileJson)) {
        PCJson_LogForFile(fileJson, "WARNING", "Root value must be an array.");
        PCJson_Free(fileJson);
        return modules;
    }

    json_array_foreach_value (fileJson: i => itemJson) {
        if (!json_is_object(itemJson)) {
            PCJson_LogForFile(itemJson, "WARNING", "Array item #%d isn`t object.", i);
            json_free(itemJson);
            continue;
        }

        new Array:limits = PCSingle_ObjVipmLimits(itemJson, "Limits");
        new Array:moduleNames = json_object_get_strings_list(itemJson, "Modules", VIPM_MODULES_TYPE_NAME_MAX_LEN);

        ArrayForeachString (moduleNames: j => moduleName[VIPM_MODULES_TYPE_NAME_MAX_LEN]) {
            if (TrieKeyExists(modules, moduleName)) {
                PCJson_LogForFile(itemJson, "WARNING", "Duplicate limits for module `%s`.", moduleName);
                continue;
            }

            TrieSetCell(modules, moduleName, limits);
        }
        
        json_free(itemJson);
        ArrayDestroy(moduleNames);
    }

    PCJson_Free(fileJson);
    return modules;
}
