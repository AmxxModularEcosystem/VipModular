#if defined _UTILS_src_Forwards_included
    #endinput
#endif
#define _UTILS_src_Forwards_included

/*
Functions & Macroses:

    Forwards_Init();
    Forwards_Reg(const name[], const stopType, const ...?param_types);
    Forwards_DefaultReturn(const iDefaultReturnValue);
    Forwards_Call(const name[]);
    Forwards_CallP(const name[], ...param_values);
    Forwards_GetHandler(const name[]);
    Forwards_RegAndCallP(const name[], const stopType, [...param_types], [...param_values]);
    Forwards_RegAndCall(const name[], const stopType);
*/

#include <amxmodx>

stock static Trie:Forwards_gMap = Invalid_Trie;

stock __Forwards_Call_ret;
stock __Forwards_RegAndCall_handler;

stock Forwards_Init() {
    Forwards_gMap = TrieCreate();
}

stock Forwards_AddHandler(const name[], const handler) {
    TrieSetCell(Forwards_gMap, name, handler, false);
    return handler;
}

stock Forwards_GetHandler(const name[]) {
    new handler;
    TrieGetCell(Forwards_gMap, name, handler);
    return handler;
}

stock Forwards_DefaultReturn(const val) {
    __Forwards_Call_ret = val;
}

stock Forwards_GetReturn(any:...) {
    return __Forwards_Call_ret;
}

// Forwards_Reg(const name[], const stopType, const ...?param_types);
#define Forwards_Reg(%1,%2) \
    Forwards_AddHandler(%1, CreateMultiForward(%1, %2))
#define Forwards_RegP Forwards_Reg

// Forwards_CallP(const name[], ...param_values);
#define Forwards_CallP(%1,%2) \
    Forwards_GetReturn(ExecuteForward(Forwards_GetHandler(%1), __Forwards_Call_ret, %2))

// Forwards_Call(const name[]);
#define Forwards_Call(%1) \
    Forwards_GetReturn(ExecuteForward(Forwards_GetHandler(%1), __Forwards_Call_ret))

// Forwards_RegAndCallP(const name[], const stopType, [...param_types], [...param_values]);
#define Forwards_RegAndCallP(%1,%2,[%3],[%4]) \
    Forwards_GetReturn(DestroyForward( \
        ExecuteForward( \
            __Forwards_RegAndCall_handler = CreateMultiForward(%1, %2, %3), \
            __Forwards_Call_ret, %4 \
        ) ? __Forwards_RegAndCall_handler : __Forwards_RegAndCall_handler \
    ))

// Forwards_RegAndCall(const name[], const stopType);
#define Forwards_RegAndCall(%1,%2) \
    Forwards_GetReturn(DestroyForward( \
        ExecuteForward( \
            __Forwards_RegAndCall_handler = CreateMultiForward(%1, %2), \
            __Forwards_Call_ret \
        ) ? __Forwards_RegAndCall_handler : __Forwards_RegAndCall_handler \
    ))