#pragma once

#include <cstdint>
#include <link.h>
#include <sys/mman.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <cerrno>

namespace RemapTools {

    struct ProcMapInfo {
        uintptr_t start;
        uintptr_t end;
        uintptr_t offset;
        uint8_t perms;
        ino_t inode;
        std::string dev;
        std::string path;
    };

    std::vector<ProcMapInfo> ListModulesWithName(const std::string& name) {
        std::vector<ProcMapInfo> result;

        std::ifstream maps("/proc/self/maps");
        if (!maps.is_open()) {
            LOGE("Failed to open /proc/self/maps");
            return result;
        }

        std::string line;
        while (std::getline(maps, line)) {
            if (line.find(name) == std::string::npos)
                continue;

            ProcMapInfo info{};
            char perms[5] = {};
            char dev[16] = {};
            unsigned long offset = 0;
            unsigned long inode = 0;
            char path[256] = {};

#if __LP64__
            int count = sscanf(line.c_str(), "%lx-%lx %4s %lx %15s %lu %255[^\n]",
                               &info.start, &info.end, perms, &offset, dev, &inode, path);
#else
            unsigned int start = 0, end = 0;
            int count = sscanf(line.c_str(), "%x-%x %4s %lx %15s %lu %255[^\n]",
                               &start, &end, perms, &offset, dev, &inode, path);
            info.start = start;
            info.end = end;
#endif

            if (count < 6 || info.start == 0 || info.end <= info.start)
                continue;

            info.offset = offset;
            info.inode = inode;
            info.dev = dev;
            info.path = (count == 7) ? path : "";

            info.perms = 0;
            if (strchr(perms, 'r')) info.perms |= PROT_READ;
            if (strchr(perms, 'w')) info.perms |= PROT_WRITE;
            if (strchr(perms, 'x')) info.perms |= PROT_EXEC;

            result.push_back(info);
        }

        LOGI("Found %zu matching regions for %s", result.size(), name.c_str());
        return result;
    }

    void RemapLibrary(const std::string& name) {
        std::vector<ProcMapInfo> maps = ListModulesWithName(name);

        if (maps.empty()) {
            LOGE("No regions found for %s", name.c_str());
            return;
        }

        for (const ProcMapInfo& info : maps) {
            if (info.start == 0 || info.end <= info.start || info.path.empty()) {
                LOGE("Skipping invalid region: 0x%lx - 0x%lx", info.start, info.end);
                continue;
            }

            if (!(info.perms & PROT_READ)) {
                LOGE("Skipping region without read permission: 0x%lx - 0x%lx", info.start, info.end);
                continue;
            }

            if (info.perms & PROT_EXEC) {
                LOGI("Skipping RX region: 0x%lx - 0x%lx", info.start, info.end);
                continue;
            }

            size_t size = info.end - info.start;
            if (size == 0 || size > 512 * 1024 * 1024) {
                LOGE("Skipping suspicious size region: 0x%lx - 0x%lx", info.start, info.end);
                continue;
            }

            void* address = reinterpret_cast<void*>(info.start);

            std::string permsStr;
            if (info.perms & PROT_READ) permsStr += "R";
            if (info.perms & PROT_WRITE) permsStr += "W";
            if (info.perms & PROT_EXEC) permsStr += "X";

            LOGI("Remapping region: 0x%lx - 0x%lx (size: %zu bytes) with perms: %s",
                 info.start, info.end, size, permsStr.c_str());

            void* tempMap = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
            if (tempMap == MAP_FAILED) {
                LOGE("mmap failed: %s", strerror(errno));
                continue;
            }

            if (memcpy(tempMap, address, size) == nullptr) {
                LOGE("memcpy failed unexpectedly");
                munmap(tempMap, size);
                continue;
            }

            void* result = mremap(tempMap, size, size, MREMAP_MAYMOVE | MREMAP_FIXED, address);
            if (result == MAP_FAILED) {
                LOGE("mremap failed: %s", strerror(errno));
                munmap(tempMap, size);
                continue;
            }

            if (mprotect(address, size, info.perms) != 0) {
                LOGE("mprotect restore perms failed: %s", strerror(errno));
            } else {
                LOGI("Remap successful for region 0x%lx", info.start);
            }
        }
    }
}