#include "BetterCpp/Functions.hpp"
#include "Rice/Engine/Log.hpp"
#include "Rice/Math/Vectors/Vector3.hpp"
#include "Rice/Networking/NetProtocol.hpp"
#include "Rice/Scene/Object.hpp"
#include "Rice/Util.hpp"
#include "Rice/defines.h"
#include "pch.h"

#include "Rice/Networking/VirtualClient.hpp"
#include "Rice/Scene/Components/Transform.hpp"
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <mutex>
#include <stop_token>
#include <string>
#include <thread>

using namespace std;

NSP_NET_BEGIN

std::filesystem::path VirtualClient::config_path =
    Util::getAppDirectory() / "config";
std::filesystem::path VirtualClient::config_file = "client_keys.json";

VirtualClient::VirtualClient(
    ClientInfo info, std::chrono::high_resolution_clock::duration update_rate)
    : info(info), client_thread([this](std::stop_token stoken) {
          clientThreadFn(stoken);
      }) {}

VirtualClient::JoinResult VirtualClient::join(pIServer serv) {
    lock_guard<mutex> lock{client_mutex};
    auto response = serv->response(Request::getServerUUID());

    if (!response.is(Response::SEND_UUID))
        THROW_EXCEPTION("Server response is not valid");

    auto servUUID = response.getUUID();

    // try get key from local storage
    bool has_key = tryGetKey(*servUUID, key);

    response = serv->response(Request::joinRequest(info, key));
    if (response.is(Response::JOIN_REFUSE))
        return {false, response.getRefuseData()->msg};

    if (!response.is(Response::JOIN_ACCEPT))
        THROW_EXCEPTION("Server response is not valid");

    auto acceptData = response.getAcceptData();
    player_data = acceptData->initial_data;
    if (!has_key) {
        //if connected first time add key to local storage
        auto new_key = acceptData->key;
        Util::writeToJSON(config_path / config_file,
                          std::to_string(servUUID->getVal()),
                          new_key.toStdString());
        key = new_key;
    }

    server = serv;

    return {true, "Yay, you joined the server!"};
}

void VirtualClient::leave() {
    //TODO leave
}

bool VirtualClient::tryGetKey(UUID servUUID, ConnectionKey &out_key) {
    if (!filesystem::exists(config_path)) {
        filesystem::create_directory(config_path);
    }
    std::string key_str;
    if (!Util::getFromJson(config_path / config_file,
                           std::to_string(servUUID.getVal()), key_str))
        return false;

    out_key = ConnectionKey::fromStdString(key_str);
    return true;
}

void VirtualClient::clientThreadFn(std::stop_token stoken) {
    while (!stoken.stop_requested()) {
        if (server.isNull()) {
            using namespace std;
            this_thread::sleep_for(100ms);
            continue;
        }
        client_mutex.lock();
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

                auto get_obj_func = [this](UUID uuid) {
                    auto obj_resp =
                        server->response(Request::getObject(key, info, uuid));
                    return obj_resp.getObject()->object_data;
                };

                get_obj_func(obj.objectUUID).unpack(scene, get_obj_func);
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
        client_mutex.unlock();
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

dbl VirtualClient::getTickOffset() {
    using namespace std::chrono;
    using dms = duration<dbl, milliseconds::period>;
    dbl dt = dms(high_resolution_clock::now() - client_clock).count();
    dbl t = dms(tick_duration).count();
    return dt / t;
}

VirtualClient::~VirtualClient() {
    leave();
    client_thread.request_stop();
    client_thread.join();
};

NSP_NET_END