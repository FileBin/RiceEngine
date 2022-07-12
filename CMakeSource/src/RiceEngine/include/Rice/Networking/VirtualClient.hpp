#include "../stdafx.hpp"
#include "BetterCpp/Objects/String.hpp"
#include "Rice/defines.h"
#include <chrono>
#include <cstdint>
#include <stop_token>
#include <thread>

NSP_NET_BEGIN

class VirtualClient;
typedef RefPtr<VirtualClient> pVirtualClient;

NSP_NET_END

#pragma once

#include "../Engine/Engine.hpp"
#include "../Scene/Scene.hpp"
#include "IServer.hpp"
#include "NetProtocol.hpp"

NSP_NET_BEGIN

class VirtualClient {
  public:
    struct JoinResult {
        bool accepted;
        String msg;
    };
  private:
    pScene scene;
    pIServer server;
    PlayerData player_data;
    ClientInfo info;
    ConnectionKey key; //TODO: add local key storage

    num client_tick = 0;

    std::chrono::high_resolution_clock::time_point client_clock;

    std::chrono::high_resolution_clock::duration tick_duration;

    void clientThreadFn(std::stop_token stoken);

    std::jthread client_thread;

    dbl getTickOffset();

    void updateClock(num new_tick);

  public:
    VirtualClient(ClientInfo info);
    ~VirtualClient();
    JoinResult join(pIServer server);
};

NSP_NET_END