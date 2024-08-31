#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include "CommonTypes.h"

class ITuningDataProcessor {
public:
    virtual ~ITuningDataProcessor() = default;
    virtual std::optional<MachineLearningInput> ProcessData(const TradeSystemParameterGroup& parameterGroup,
                                             const DataBlob& newData) = 0;
};
