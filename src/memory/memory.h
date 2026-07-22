#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>

namespace mem {
    inline HANDLE pHandle;
    inline uintptr_t procID;

    uintptr_t GetProcID(const wchar_t* processName);
    uintptr_t GetModuleBase(uintptr_t processID, const wchar_t* module);

    inline void ReadBuffer(uintptr_t address, void* buffer, size_t size) {
        ReadProcessMemory(pHandle, (LPCVOID)address, buffer, size, nullptr);
    }

    inline std::string ReadString(uintptr_t address, size_t max_length = 128) {
        if (address == 0) return "";

        std::vector<char> buffer(max_length);
        SIZE_T bytesRead = 0;

        if (ReadProcessMemory(pHandle, reinterpret_cast<LPCVOID>(address), buffer.data(), max_length, &bytesRead)) {
            if (bytesRead > 0 && bytesRead <= max_length) {
                buffer[bytesRead - 1] = '\0';
            } else {
                buffer[max_length - 1] = '\0';
            }
            return std::string(buffer.data());
        }
        return "";
    }

    inline std::wstring ReadWString(uintptr_t address, size_t max_length = 128) {
        if (address == 0) return L"";

        std::vector<wchar_t> buffer(max_length);
        SIZE_T bytesRead = 0;

        if (ReadProcessMemory(pHandle, reinterpret_cast<LPCVOID>(address), buffer.data(), max_length * sizeof(wchar_t), &bytesRead)) {
            size_t charCount = bytesRead / sizeof(wchar_t);
            if (charCount > 0 && charCount <= max_length) {
                buffer[charCount - 1] = L'\0';
            } else {
                buffer[max_length - 1] = L'\0';
            }
            return std::wstring(buffer.data());
        }
        return L"";
    }

    template <typename R>
    R ReadMem(uintptr_t address) {
        R mit;
        ReadProcessMemory(pHandle, (LPCVOID)address, &mit, sizeof(R), nullptr);
        return mit;
    }

    template <typename R>
    bool WriteToMem(uintptr_t address, R value) {
        return WriteProcessMemory(pHandle, (LPVOID)address, &value, sizeof(R), nullptr);
    }

    // Batch read multiple values
    template <typename T>
    std::vector<T> ReadBatch(uintptr_t base, const std::vector<uintptr_t>& offsets) {
        std::vector<T> results;
        results.reserve(offsets.size());
        for (const auto& offset : offsets) {
            results.push_back(ReadMem<T>(base + offset));
        }
        return results;
    }

    // Memory cache for frequently accessed addresses
    class MemoryCache {
    private:
        std::unordered_map<uintptr_t, std::pair<uint64_t, std::vector<uint8_t>>> cache;
        std::mutex cache_mtx;
        static constexpr uint64_t CACHE_TTL_MS = 16;

    public:
        template <typename T>
        T ReadCached(uintptr_t address) {
            auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000000;
            std::lock_guard<std::mutex> lock(cache_mtx);

            auto it = cache.find(address);
            if (it != cache.end() && (now - it->second.first) < CACHE_TTL_MS) {
                return *reinterpret_cast<T*>(it->second.second.data());
            }

            T value = ReadMem<T>(address);
            std::vector<uint8_t> data((uint8_t*)&value, (uint8_t*)&value + sizeof(T));
            cache[address] = { now, data };
            return value;
        }

        void Clear() {
            std::lock_guard<std::mutex> lock(cache_mtx);
            cache.clear();
        }
    };
}