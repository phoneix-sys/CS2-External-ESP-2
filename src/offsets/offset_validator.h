#pragma once

#include <string>
#include <map>
#include <cstdint>
#include <Windows.h>
#include "../memory/memory.h"
#include "../utils/logger.h"

struct OffsetValidationResult {
    std::string offset_name;
    uintptr_t offset_value;
    bool is_valid;
    std::string validation_reason;
};

class OffsetValidator {
public:
    OffsetValidator(uintptr_t client, DWORD process_id);
    ~OffsetValidator() = default;

    // Validate critical offsets
    bool ValidateCriticalOffsets();

    // Validate single offset
    OffsetValidationResult ValidateOffset(const std::string& name, uintptr_t offset);

    // Get validation results
    const std::map<std::string, OffsetValidationResult>& GetResults() const { return validation_results; }

    // Check if all critical offsets are valid
    bool AllCriticalOffsetsValid() const;

    // Get percentage of valid offsets
    float GetValidOffsetPercentage() const;

private:
    uintptr_t client;
    DWORD process_id;
    std::map<std::string, OffsetValidationResult> validation_results;

    // Validation strategies
    bool ValidateEntityListOffset();
    bool ValidatePlayerPawnOffset();
    bool ValidateViewMatrixOffset();
    bool ValidateHealthOffset();
    bool ValidateTeamOffset();

    // Helper
    bool CanReadMemory(uintptr_t address, size_t size);
};
