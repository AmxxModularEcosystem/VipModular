#if defined __vipm__core__vips_manager__included
    #endinput
#endif
#define __vipm__core__vips_manager__included

#include <amxmodx>
#include <regex>
#include "VipM/Forwards"
#include "VipM/ArrayTrieUtils"
#include "VipM/JsonUtils"
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

bool:VipsManager_LoadFromFile(const sFilePath[]) {
    if (!file_exists(sFilePath)) {
        return false;
    }

    new JSON:jVipUnits = PCJson_ParseFile(sFilePath, g_sRootDir);
    g_aVips = VipUnit_ReadList(jVipUnits, g_aVips);
    PCJson_Free(jVipUnits);

    return true;
}

bool:VipsManager_LoadFromFolder(sFolderPath[]) {
    if (!dir_exists(sFolderPath)) {
        return false;
    }

    new sFile[PLATFORM_MAX_PATH], iDirHandler, FileType:iType;
    iDirHandler = open_dir(sFolderPath, sFile, charsmax(sFile), iType);
    if (!iDirHandler) {
        return false;
    }

    new Regex:iRegEx_FileName, ret;
    iRegEx_FileName = regex_compile("(.+).json$", ret, "", 0, "i");

    do {
        if (iType != FileType_File || regex_match_c(sFile, iRegEx_FileName) <= 0) {
            continue;
        }

        VipsManager_LoadFromFile(fmt("%s/%s", sFolderPath, sFile));
    } while (next_file(iDirHandler, sFile, charsmax(sFile), iType));

    regex_free(iRegEx_FileName);
    close_dir(iDirHandler);

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

VipsManager_UserReset(const UserId) {
    if (g_tUserModules[UserId] == Invalid_Trie) {
        return;
    }

    new TrieIter:Iter = TrieIterCreate(g_tUserModules[UserId]);
    while (!TrieIterEnded(Iter)) {
        new Trie:tParams = Invalid_Trie;
        if (TrieIterGetCell(Iter, tParams)) {
            ModuleUnit_FreeParamsIfTemp(tParams);
        }
        TrieIterNext(Iter);
    }
    TrieIterDestroy(Iter);

    TrieDestroy(g_tUserModules[UserId]);
}

Trie:VipsManager_GetUserParams(const UserId, const T_ModuleType:iModuleType) {
    new Trie:tParams = Invalid_Trie;
    TrieGetCell(g_tUserModules[UserId], ModuleType_iGetName(iModuleType), tParams);
    return tParams;
}
