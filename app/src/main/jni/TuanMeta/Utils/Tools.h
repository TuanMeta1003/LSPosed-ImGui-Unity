#pragma once

#include <jni.h>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <libgen.h>
#include <inttypes.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <openssl/md5.h>

#include "oxorany/oxorany.h"
#include "Libraries/Dobby/dobby.h"

namespace Tools
{
	void Hook(void *target, void *replace, void **backup);
	bool Read(void *addr, void *buffer, size_t length);
	bool Write(void *addr, void *buffer, size_t length);
	bool ReadAddr(void *addr, void *buffer, size_t length);
	bool WriteAddr(void *addr, void *buffer, size_t length);

	bool PVM_ReadAddr(void *addr, void *buffer, size_t length);
	bool PVM_WriteAddr(void *addr, void *buffer, size_t length);
	bool IsPtrValid(void *addr);

	uintptr_t GetBaseAddress(const char *name);
	uintptr_t GetEndAddress(const char *name);
	uintptr_t FindPattern(const char *lib, const char* pattern);
	uintptr_t GetRealOffsets(const char *libraryName, uintptr_t relativeAddr);
	uintptr_t String2Offset(const char *c);

	std::string RandomString(const int len);
	std::string GetPackageName(JNIEnv *env, jobject context);
	const char *GetAndroidID(JNIEnv *env, jobject context);
	const char *GetDeviceModel(JNIEnv *env);
	const char *GetDeviceBrand(JNIEnv *env);
	const char *GetDeviceUniqueIdentifier(JNIEnv *env, const char *uuid);
}
