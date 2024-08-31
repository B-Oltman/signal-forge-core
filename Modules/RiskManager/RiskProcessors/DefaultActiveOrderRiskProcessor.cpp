#include "DefaultActiveOrderRiskProcessor.h"

RiskAssessment DefaultActiveOrderRiskProcessor::EvaluateActiveOrder(const ExecutedOrder& activeOrder) const {
    // Implement your default active order risk evaluation logic here    
    RiskAssessment riskAssessment;
    riskAssessment.acceptable = true;
    return riskAssessment;
}
