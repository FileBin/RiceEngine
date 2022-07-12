#include "BetterCpp/Functions.hpp"
#include "BetterCpp/namespaces.h"
#include "Rice/namespaces.h"
#include "pch.h"

#include "Rice/Scene/Component.hpp"
#include "Rice/Scene/Object.hpp"

NSP_ENGINE_BEGIN

pObject ObjectData::unpack(pScene scene,
                           std::function<ObjectData(UUID)> getRelativesData) {
    auto parent = scene->getObject(parentUUID);

    if (parent.isNull()) {
        parent = getRelativesData(parentUUID).unpack(scene, getRelativesData);
    }

    auto self_obj = unpack(parent, getRelativesData);

    return self_obj;
}

pObject ObjectData::unpack(pObject parent, std::function<ObjectData(UUID)> getRelativesData) {
    pObject inst = Object::createEmpty();
    inst->active = active;
    inst->enabled = enabled;
    inst->name = name;
    inst->parent = parent;
    inst->selfUUID = selfUUID;

    for (auto c : componentsData) {
        inst->components.push_back(c->createComponent());
    }

    for (auto childUUID : childrenUUID) {
        auto data = getRelativesData(childUUID);
        data.unpack(inst, getRelativesData);
    }

    parent->children.push_back(inst);

    return inst;
}

ObjectData::~ObjectData() {
    for (auto c : componentsData) {
        if (c) {
            delete c;
        }
    }
}

NSP_ENGINE_END