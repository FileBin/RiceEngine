#include "pch.h"

#include "Rice/Scene/Component.hpp"
#include "Rice/Scene/Object.hpp"
#include "Rice/namespaces.h"

NSP_ENGINE_BEGIN

ObjectData Object::pack() {
    ObjectData data;
    data.active = active;
    data.enabled = enabled;
    data.selfUUID = selfUUID;
    if (parent.isNotNull())
        data.parentUUID = parent->selfUUID;
    else
        data.parentUUID = 0;

    uint n = children.size();
    data.childrenUUID.resize(n);
    for (uint i = 0; i < n; i++) {
        data.childrenUUID[i] = children[i]->selfUUID;
    }

    n = components.size();
    data.componentsData.resize(n);
    for (uint i = 0; i < n; i++) {
        data.componentsData[i] = components[i]->pack();
    }

    return data;
}

pObject ObjectData::unpack(pScene scene,
                           std::function<ObjectData(UUID)> getRelativesData) {
    auto parent = scene->getObject(parentUUID);

    if (parent.isNull()) {
        parent = getRelativesData(parentUUID).unpack(scene, getRelativesData);
    }

    auto self_obj = unpack(parent, getRelativesData);

    return self_obj;
}

pObject ObjectData::unpack(pObject parent,
                           std::function<ObjectData(UUID)> getRelativesData) {
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

NSP_ENGINE_END