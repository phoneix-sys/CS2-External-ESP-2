#include "offset_validator.h"
#include "../offsets/offsets.h"

OffsetValidator::OffsetValidator(uintptr_t client, DWORD process_id)
    : client(client), process_id(process_id) {
    LOG_INFO("OffsetValidator initialized");
}

bool OffsetValidator::CanReadMemory(uintptr_t address, size_t size) {
    BYTE buffer[8] = { 0 };
    SIZE_T bytes_read = 0;

    HANDLE process = OpenProcess(PROCESS_VM_READ, FALSE, process_id);
    if (!process) return false;

    bool result = ReadProcessMemory(process, (LPVOID)address, buffer, size, &bytes_read) && bytes_read > 0;
    CloseHandle(process);

    return result;
}

OffsetValidationResult OffsetValidator::ValidateOffset(const std::string& name, uintptr_t offset) {
    OffsetValidationResult result;
    result.offset_name = name;
    result.offset_value = offset;

    if (!CanReadMemory(client + offset, 4)) {
        result.is_valid = false;
        result.validation_reason = "Cannot read memory at offset";
        return result;
    }

    result.is_valid = true;
    result.validation_reason = "Valid";
    return result;
}

bool OffsetValidator::ValidateCriticalOffsets() {
    LOG_INFO("Validating critical offsets...");

    std::map<std::string, uintptr_t> critical_offsets = {
        { "dwLocalPlayerPawn", dwLocalPlayerPawn },
        { "dwEntityList", dwEntityList },
        { "dwViewMatrix", dwViewMatrix },
        { "m_iHealth", m_iHealth },
        { "m_iTeamNum", m_iTeamNum },
        { "m_vOldOrigin", m_vOldOrigin }
    };

    for (const auto& [name, offset] : critical_offsets) {
        auto result = ValidateOffset(name, offset);
        validation_results[name] = result;

        char buffer[256];
        snprintf(buffer, sizeof(buffer), "[%s] %s: %s", name.c_str(),
            result.is_valid ? "VALID" : "INVALID", result.validation_reason.c_str());
        if (result.is_valid) {
            LOG_INFO(buffer);
        }
        else {
            LOG_WARNING(buffer);
        }
    }

    return AllCriticalOffsetsValid();
}

bool OffsetValidator::AllCriticalOffsetsValid() const {
    const std::vector<std::string> critical_names = {
        "dwLocalPlayerPawn", "dwEntityList", "dwViewMatrix"
    };

    for (const auto& name : critical_names) {
        auto it = validation_results.find(name);
        if (it == validation_results.end() || !it->second.is_valid) {
            return false;
        }
    }

    return true;
}

float OffsetValidator::GetValidOffsetPercentage() const {
    if (validation_results.empty()) return 0.0f;

    int valid_count = 0;
    for (const auto& [name, result] : validation_results) {
        if (result.is_valid) valid_count++;
    }

    return (valid_count / static_cast<float>(validation_results.size())) * 100.0f;
}
