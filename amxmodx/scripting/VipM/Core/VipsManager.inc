#if defined __vipm__core__vips_manager__included
    #endinput
#endif
#define __vipm__core__vips_manager__included

#include <amxmodx>
#include <regex>
#include "VipM/Forwards"
#include "VipM/ArrayTrieUtils"
#include "VipM/Core/Objects/VipUnit"

static Array:g_aVips = Invalid_Array; // T_VipUnit[]
static g_sRootDir[PLATFORM_MAX_PATH] = "";
static Trie:g_tUserModules[MAX_PLAYERS + 1] = {Invalid_Trie, ...};

VipsManager_Init() {
    CallOnce();

    VipUnit_Init();

    g_aVips = ArrayCreate(1, 1);
    Forwards_Reg("VipM_OnUserUpdated", ET_IGNORE, FP_CELL);
}

VipsManager_VipsCount() {
    return ArraySize(g_aVips);
}

VipsManager_SetRootDir(const sRootDir[]) {
    copy(g_sRootDir, charsmax(g_sRootDir), sRootDir);
}

bool:VipsManager_LoadFromFile(const filePath[]) {
    if (!file_exists(filePath)) {
        return false;
    }

    new JSON:unitsJson = PCJson_ParseFile(filePath, g_sRootDir);
    g_aVips = VipUnit_ReadList(unitsJson, g_aVips);
    PCJson_Free(unitsJson);

    return true;
}

bool:VipsManager_LoadFromFolder(dirPath[]) {
    if (!dir_exists(dirPath)) {
        return false;
    }

    new filePath[PLATFORM_MAX_PATH], dirHandler, FileType:fileType;
    dirHandler = open_dir(dirPath, filePath, charsmax(filePath), fileType);
    if (!dirHandler) {
        return false;
    }

    new Regex:iRegEx_FileName, ret;
    iRegEx_FileName = regex_compile("(.+).json$", ret, "", 0, "i");

    do {
        if (fileType != FileType_File || regex_match_c(filePath, iRegEx_FileName) <= 0) {
            continue;
        }

        VipsManager_LoadFromFile(fmt("%s/%s", dirPath, filePath));
    } while (next_file(dirHandler, filePath, charsmax(filePath), fileType));

    regex_free(iRegEx_FileName);
    close_dir(dirHandler);

    return true;
}

VipsManager_UserReload(const playerIndex) {
    VipsManager_UserReset(playerIndex);
    
    Dbg_Log("VipsManager_UserReload(%n) start", playerIndex);

    g_tUserModules[playerIndex] = TrieCreate();
    for (new i = 0, ii = ArraySize(g_aVips); i < ii; ++i) {
        Dbg_Log("VipsManager_UserReload(%n) - vip #%d", playerIndex, i);

        new T_VipUnit:vip = ArrayGetCell(g_aVips, i);
        if (!VipUnit_CheckUserAccess(vip, playerIndex)) {
            Dbg_Log("VipsManager_UserReload(%n) - - no access", playerIndex);
            continue;
        }
        Dbg_Log("VipsManager_UserReload(%n) - - has access", playerIndex);

        new Array:modules = VipUnit_GetModules(vip);
        for (new j = 0, jj = ArraySize(modules); j < jj; ++j) {
            new T_ModuleUnit:module = ArrayGetCell(modules, j);

            new moduleName[VIPM_MODULES_TYPE_NAME_MAX_LEN]; // TODO: Посмотреть можно ли юзать просто хендлер
            ModuleUnit_GetTypeName(module, moduleName, charsmax(moduleName));
            
            Dbg_Log("VipsManager_UserReload(%n) - - - module %s (#%d)", playerIndex, moduleName, j);

            new Trie:params = ModuleUnit_GetParams(module);
            if (TrieKeyExists(g_tUserModules[playerIndex], moduleName)) {
                Dbg_Log("VipsManager_UserReload(%n) - - - - merge", playerIndex);
                new Trie:oldParams;
                TrieGetCell(g_tUserModules[playerIndex], moduleName, oldParams);

                TrieSetCell(g_tUserModules[playerIndex], moduleName, ModuleUnit_Merge(module, oldParams, params));
            } else {
                Dbg_Log("VipsManager_UserReload(%n) - - - - add", playerIndex);
                TrieSetCell(g_tUserModules[playerIndex], moduleName, params);
            }
        }
    }

    Forwards_CallP("VipM_OnUserUpdated", playerIndex);
}

VipsManager_UserReset(const playerIndex) {
    if (g_tUserModules[playerIndex] == Invalid_Trie) {
        return;
    }

    new TrieIter:iter = TrieIterCreate(g_tUserModules[playerIndex]);
    while (!TrieIterEnded(iter)) {
        new Trie:tParams = Invalid_Trie;
        if (TrieIterGetCell(iter, tParams)) {
            ModuleUnit_FreeParamsIfTemp(tParams);
        }
        TrieIterNext(iter);
    }
    TrieIterDestroy(iter);

    TrieDestroy(g_tUserModules[playerIndex]);
}

Trie:VipsManager_GetUserParams(const playerIndex, const T_ModuleType:type) {
    if (g_tUserModules[playerIndex] == Invalid_Trie) {
        return Invalid_Trie;
    }

    new Trie:p = Invalid_Trie;
    TrieGetCell(g_tUserModules[playerIndex], ModuleType_iGetName(type), p);

    return p;
}
