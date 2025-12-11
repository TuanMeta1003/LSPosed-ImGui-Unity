#pragma once

#ifndef VERSION_NAME
#define VERSION_NAME oxorany("Unknown")
#endif

const char* GetAbiTag() {
#if defined(__aarch64__) || defined(__x86_64__)
    return "[X64]";
#else
    return "[X32]";
#endif
}

#define Method(image, namespaze, clazz, name, args) \
GetMethodOffset(oxorany(image), oxorany(namespaze), oxorany(clazz), oxorany(name), args)
#define Field(image, namespaze, clazz, name) \
GetFieldOffset(oxorany(image), oxorany(namespaze), oxorany(clazz), oxorany(name))
#define StaticField(image, namespaze, clazz, name, out) \
GetStaticFieldValue(oxorany(image), oxorany(namespaze), oxorany(clazz), oxorany(name), &out)

#define TuanMetaHook(image, namespaze, clazz, name, args, hook, orig) \
Tools::Hook((void *)(uintptr_t)Method(image, namespaze, clazz, name, args), (void *)hook, (void **)&orig)
#define TuanMetaHook2(image, namespaze, clazz, name, args, hook) \
Tools::Hook((void *)(uintptr_t)Method(image, namespaze, clazz, name, args), (void *)hook, NULL)

#define TuanMetaHook3(image, namespaze, clazz, name, args, hook, orig) \
Tools::Hook((void *)(uintptr_t)GetMethodOffset(image, namespaze, clazz, name, args), (void *)hook, (void **)&orig)

#define NativeHook(image, namespaze, clazz, name, args, hook, orig) \
hook_func((void *)(uintptr_t)Method(image, namespaze, clazz, name, args), (void *)hook, (void **)&orig)
#define NativeHook2(image, namespaze, clazz, name, args, hook, orig) \
hook_func((void *)(uintptr_t)Method(image, namespaze, clazz, name, args), (void *)hook, NULL)