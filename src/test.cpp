﻿#include <lua.hpp>
#include <assert.h>
#include <string>
#include <windows.h>
#define cdecl_type 0
#define stdcall_type 1
#define thiscall_type 2
#define vtblhook_type 3

const unsigned char tmp[] = {
    'a', 'b', 'a', 'a', 'b', 'a', 'c', 'a',
   // 前缀不匹配
    0x8B, 0xC8, 0xA8, 0x3F, 0xE7, 0x04, 0x00, 0xE9, 0xBE, 0x00, 0x00, 0x00, 0x6A, 0x00, 0x6A, 0x00,
    0x6A, 0xFF, 0x6A ,0xFF, 0xBA ,0x04 ,0x00 ,0x00 ,0x00 ,0x6B ,0xC2 ,0x0E ,0x8B,

    // 匹配
    0x8B, 0xC8, 0xE8, 0x3F, 0xE7, 0x04, 0x00, 0xE9, 0xBE, 0x00, 0x00, 0x00, 0x6A, 0x00, 0x6A, 0x00,
    0x6A, 0xFF, 0x6A ,0xFF, 0xBA ,0x04 ,0x00 ,0x00 ,0x00 ,0x6B ,0xC2 ,0x0E ,0x8B
};

int test_kmdfind(const unsigned char *tmp, int size) {
    return 0;
}

int cdecl_test0() { return 0; }
int cdecl_test1(int a) { return cdecl_test0() + a; }
int cdecl_test2(int a, int b) { return cdecl_test1(a) + b; }
int cdecl_test3(int a, int b, int c) { return cdecl_test2(a, b) + c; }
int cdecl_test4(int a, int b, int c, int d) { return cdecl_test3(a, b, c) + d; }
int cdecl_test5(int a, int b, int c, int d, int e) { return cdecl_test4(a, b, c, d) + e; }

int WINAPI stdcall_test0() { return 0; }
int WINAPI stdcall_test1(int a) { return stdcall_test0() + a; }
int WINAPI stdcall_test2(int a, int b) { return stdcall_test1(a) + b; }
int WINAPI stdcall_test3(int a, int b, int c) { return stdcall_test2(a, b) + c; }
int WINAPI stdcall_test4(int a, int b, int c, int d) { return stdcall_test3(a, b, c) + d; }
int WINAPI stdcall_test5(int a, int b, int c, int d, int e) { return stdcall_test4(a, b, c, d) + e; }

struct TestThisCall {
    int test0() { return 0; }
    int test1(int a) { return test0() + a; }
    int test2(int a, int b) { return test1(a) + b; }
    int test3(int a, int b, int c) { return test2(a, b) + c; }
    int test4(int a, int b, int c, int d) { return test3(a, b, c) + d; }
    int test5(int a, int b, int c, int d, int e) { return test4(a, b, c, d) + e; }
};

struct TestThisCall_Vtbl {
    virtual int test0() { return 0; }
    virtual int test1(int a) { return test0() + a; }
    virtual int test2(int a, int b) { return test1(a) + b; }
    virtual int test3(int a, int b, int c) { return test2(a, b) + c; }
    virtual int test4(int a, int b, int c, int d) { return test3(a, b, c) + d; }
    virtual int test5(int a, int b, int c, int d, int e) { return test4(a, b, c, d) + e; }
};

template<typename memberT>
union u_ptm_cast {
    memberT pmember;
    void *pvoid;
};

template<typename T> lua_Integer MemFuncCast(T t) {
    u_ptm_cast<T> c;
    c.pmember = t;
    return (lua_Integer)c.pvoid;
}

void cdecl_tests() {
    lua_State *l = luaL_newstate();
    luaL_openlibs(l);
    lua_pushinteger(l, (lua_Integer)&test_kmdfind);
    lua_setglobal(l, "test_kmdfind_func_addr");

    int err = luaL_loadfile(l, "./src/test.lua");
    if (err) {
        fprintf(stderr, "%s\n", lua_tostring(l, -1));
        lua_close(l);
        return;
    }
    lua_pushinteger(l, cdecl_type);
    lua_pushinteger(l, (lua_Integer)&cdecl_test0);
    lua_pushinteger(l, (lua_Integer)&cdecl_test1);
    lua_pushinteger(l, (lua_Integer)&cdecl_test2);
    lua_pushinteger(l, (lua_Integer)&cdecl_test3);
    lua_pushinteger(l, (lua_Integer)&cdecl_test4);
    lua_pushinteger(l, (lua_Integer)&cdecl_test5);
    err = lua_pcall(l, 7, 0, 0);
    if (err) {
        fprintf(stderr, "%s\n", lua_tostring(l, -1));
        lua_close(l);
        return;
    }
    printf("test0: %d\n", cdecl_test0());
    printf("test1: %d\n", cdecl_test1(1));
    printf("test2: %d\n", cdecl_test2(1, 1));
    printf("test3: %d\n", cdecl_test3(1, 1, 1));
    printf("test4: %d\n", cdecl_test4(1, 1, 1, 1));
    printf("test5: %d\n", cdecl_test5(1, 1, 1, 1, 1));
    
    printf("test_kmdfind: %08X == %08X\n", test_kmdfind(tmp, sizeof(tmp)), (unsigned int)tmp);

    lua_close(l);
}

void stdcall_tests() {
    lua_State *l = luaL_newstate();
    luaL_openlibs(l);
    int err = luaL_loadfile(l, "./src/test.lua");
    if (err) {
        fprintf(stderr, "%s\n", lua_tostring(l, -1));
        lua_close(l);
        return;
    }
    lua_pushinteger(l, stdcall_type);
    lua_pushinteger(l, (lua_Integer)&stdcall_test0);
    lua_pushinteger(l, (lua_Integer)&stdcall_test1);
    lua_pushinteger(l, (lua_Integer)&stdcall_test2);
    lua_pushinteger(l, (lua_Integer)&stdcall_test3);
    lua_pushinteger(l, (lua_Integer)&stdcall_test4);
    lua_pushinteger(l, (lua_Integer)&stdcall_test5);
    err = lua_pcall(l, 7, 0, 0);
    if (err) {
        fprintf(stderr, "%s\n", lua_tostring(l, -1));
        lua_close(l);
        return;
    }
    printf("stdcall_test0: %d\n", stdcall_test0());
    printf("stdcall_test1: %d\n", stdcall_test1(1));
    printf("stdcall_test2: %d\n", stdcall_test2(1, 1));
    printf("stdcall_test3: %d\n", stdcall_test3(1, 1, 1));
    printf("stdcall_test4: %d\n", stdcall_test4(1, 1, 1, 1));
    printf("stdcall_test5: %d\n", stdcall_test5(1, 1, 1, 1, 1));
    lua_close(l);
}

void thiscall_tests() {
    lua_State *l = luaL_newstate();
    luaL_openlibs(l);
    int err = luaL_loadfile(l, "./src/test.lua");
    if (err) {
        fprintf(stderr, "%s\n", lua_tostring(l, -1));
        lua_close(l);
        return;
    }
    lua_pushinteger(l, thiscall_type);
    lua_pushinteger(l, MemFuncCast(&TestThisCall::test0));
    lua_pushinteger(l, MemFuncCast(&TestThisCall::test1));
    lua_pushinteger(l, MemFuncCast(&TestThisCall::test2));
    lua_pushinteger(l, MemFuncCast(&TestThisCall::test3));
    lua_pushinteger(l, MemFuncCast(&TestThisCall::test4));
    lua_pushinteger(l, MemFuncCast(&TestThisCall::test5));
    err = lua_pcall(l, 7, 0, 0);
    if (err) {
        fprintf(stderr, "%s\n", lua_tostring(l, -1));
        lua_close(l);
        return;
    }
    TestThisCall tt;
    printf("thiscall_test0: %d\n", tt.test0());
    printf("thiscall_test1: %d\n", tt.test1(1));
    printf("thiscall_test2: %d\n", tt.test2(1, 1));
    printf("thiscall_test3: %d\n", tt.test3(1, 1, 1));
    printf("thiscall_test4: %d\n", tt.test4(1, 1, 1, 1));
    printf("thiscall_test5: %d\n", tt.test5(1, 1, 1, 1, 1));
    lua_close(l);
}

void vtblhook_tests() {
    lua_State *l = luaL_newstate();
    luaL_openlibs(l);
    int err = luaL_loadfile(l, "./src/test.lua");
    if (err) {
        fprintf(stderr, "%s\n", lua_tostring(l, -1));
        lua_close(l);
        return;
    }
    TestThisCall_Vtbl tt;
    DWORD *vtbl = *(DWORD**)&tt;
    lua_pushinteger(l, vtblhook_type);
    lua_pushinteger(l, (DWORD)&vtbl[0]);
    lua_pushinteger(l, (DWORD)&vtbl[1]);
    lua_pushinteger(l, (DWORD)&vtbl[2]);
    lua_pushinteger(l, (DWORD)&vtbl[3]);
    lua_pushinteger(l, (DWORD)&vtbl[4]);
    lua_pushinteger(l, (DWORD)&vtbl[5]);
    err = lua_pcall(l, 7, 0, 0);
    if (err) {
        fprintf(stderr, "%s\n", lua_tostring(l, -1));
        lua_close(l);
        return;
    }
    TestThisCall_Vtbl *p = &tt;
    printf("vtblhook_test0: %d\n", p->test0());
    printf("vtblhook_test1: %d\n", p->test1(1));
    printf("vtblhook_test2: %d\n", p->test2(1, 1));
    printf("vtblhook_test3: %d\n", p->test3(1, 1, 1));
    printf("vtblhook_test4: %d\n", p->test4(1, 1, 1, 1));
    printf("vtblhook_test5: %d\n", p->test5(1, 1, 1, 1, 1));
    lua_close(l);
}

void messagebox_tests() {
    lua_State *l = luaL_newstate();
    luaL_openlibs(l);
    int err = luaL_dostring(l, " \
        local minhook = require('minhook') \
        minhook.initialize() \
        local messagebox_cb = function(hook, ...) \
            print('in MessageBoxA hook') \
            result = hook(...) \
            print('out MessageBoxA hook') \
            print('==disable MessageBoxA hook') \
            hook:unhook() \
            return result \
        end \
        local h = minhook.create('user32.dll:MessageBoxA', 4, minhook.STDCALL, messagebox_cb) \
        h:hook() \
    ");
    if (err) {
        fprintf(stderr, "%s\n", lua_tostring(l, -1));
        lua_close(l);
        return;
    }
    // hook by lua
    MessageBoxA(0, "in hook call", "caption", 0);

    // raw call
    MessageBoxA(0, "not in hook call", "caption", 0);
    lua_close(l);
}

int main() {
    cdecl_tests();
    stdcall_tests();
    thiscall_tests();
    vtblhook_tests();
    messagebox_tests();
    system("pause");
    return 0;
}
