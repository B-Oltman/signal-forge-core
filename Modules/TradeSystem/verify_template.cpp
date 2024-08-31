
#include "TradeSystemBuilder.h"
#include "CommonTypes.h"
#include "TestTradeSystem.h"
#include "DefaultOrderManager.h"
#include "DefaultSignalProcessor.h"
#include "sierrachart.h"

// Verification function
void verify() {
    // Instantiate the builder to verify the template
    auto sc = *(new s_sc());
    TradeSystemBuilder<TradeSystem> builder(&sc, ContextType::Backtesting);
    auto result = builder
           .WithSignalProcessor<DefaultSignalProcessor>()
           .WithOrderProcessor<DefaultOrderProcessor>() // DefaultOrderProcessor doesn't need to be included, the builder will do it. This is for overriding the class with a custom implementation if desired.
           .Build("VerifySystem");
}

int main() {
    verify();        
    return 0;
}
