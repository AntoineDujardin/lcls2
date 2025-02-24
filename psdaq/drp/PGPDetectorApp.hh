#pragma once

#include <thread>
#include "DrpBase.hh"
#include "PGPDetector.hh"
#include "psdaq/service/Collection.hh"

namespace Drp {

class PGPDetectorApp : public CollectionApp
{
public:
    PGPDetectorApp(Parameters& para);
    void shutdown();
    nlohmann::json connectionInfo() override;
private:
    void handleConnect(const nlohmann::json& msg) override;
    void handleDisconnect(const nlohmann::json& msg) override;
    void handlePhase1(const nlohmann::json& msg) override;
    void handleReset(const nlohmann::json& msg) override;
    DrpBase m_drp;
    Parameters& m_para;
    std::thread m_pgpThread;
    std::thread m_collectorThread;
    std::unique_ptr<PGPDetector> m_pgpDetector;
    Detector* m_det;
};

}
