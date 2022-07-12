#include "Rice/Engine/Log.hpp"
#include "Rice/Math/Vectors/Vector3.hpp"
#include "Rice/Networking/NetProtocol.hpp"
#include "Rice/defines.h"
#include "pch.h"

#include "Rice/Networking/VirtualClient.hpp"
#include "Rice/Scene/Components/Transform.hpp"
#include <chrono>
#include <cstdint>
#include <stop_token>

NSP_NET_BEGIN

VirtualClient::VirtualClient(ClientInfo info)
    : info(info), client_thread([this](std::stop_token stoken) {
          clientThreadFn(stoken);
      }) {}

VirtualClient::JoinResult VirtualClient::join(pIServer serv) {
    // TODO try to get key from local storage
    auto response = serv->response(Request::joinRequest(info));
    if (response.is(Response::JOIN_REFUSE))
        return {false, response.getRefuseData()->msg};

    if (!response.is(Response::JOIN_ACCEPT))
        THROW_EXCEPTION("Server response is not valid");

    auto acceptData = response.getAcceptData();
    player_data = acceptData->initial_data;
    key = acceptData->key;

    server = serv;

    return {true, "Yay, you joined the server!"};
}

void VirtualClient::clientThreadFn(std::stop_token stoken) {
    while (!stoken.stop_requested()) {
        auto resp =
            server->response(Request::sendPlayerData(key, info, player_data));
        if (!resp.is(Response::SEND_OK)) {
            Log::log(Log::Error, "Sending data to server failed!");
            continue;
        }
        resp = server->response(Request::getSceneState(key, info));

        if (!resp.is(Response::SEND_SCENE_STATE)) {
            THROW_EXCEPTION("Server response is not valid");
        }
        auto scene_state = resp.getSceneState();
        auto &objects = scene_state->objects_state;

        uint n = objects.size();

        dbl tick_offset = getTickOffset();

        updateClock(scene_state->server_tick);

        for (uint i = 0; i < n; i++) {
            auto &obj = objects[i];
            auto scene_obj = scene->getObject(obj.objectUUID);
            if (scene_obj.isNull()) {
                // push it into the scene
                auto obj_resp = server->response(
                    Request::getObject(key, info, obj.objectUUID));

                scene_obj = obj_resp.getObject()->object;

                scene->pushObjectWithUUID(scene_obj, obj.objectUUID);
            }

            auto transform = scene_obj->getComponent<Components::Transform>();

            Vector3 dir = obj.tick_data.position - obj.prev_tick_data.position;

            transform->setPosition(obj.prev_tick_data.position +
                                   dir * tick_offset);
            transform->setRotation(
                obj.tick_data.rotation); // TODO add interpolation

            dir = obj.tick_data.scale - obj.prev_tick_data.scale;
            transform->setScale(obj.prev_tick_data.scale + dir * tick_offset);
        }
    }
}

void VirtualClient::updateClock(num srv_tick) {
    using namespace std::chrono;
    num tick_count = srv_tick - client_tick;
    auto now = high_resolution_clock::now();
    tick_duration = now - client_clock;
    tick_duration /= tick_count;
    client_clock = now;
}

dbl VirtualClient::getTickOffset() { using namespace std::chrono; 
    dbl dt = duration<dbl, milliseconds::period>(high_resolution_clock::now() - client_clock).count();
    dbl t = duration<dbl, milliseconds::period>(tick_duration).count();
    return dt / t;
}

VirtualClient::~VirtualClient() {
    client_thread.request_stop();
    client_thread.join();
};

NSP_NET_END