#pragma once

#include <vector>
#include <string>

class MLSignalGenerator {
public:
    virtual void LoadModel(const std::string& modelPath);
    virtual double GenerateSignal(const std::vector<double>& inputs);

private:
    std::string modelPath;
};
