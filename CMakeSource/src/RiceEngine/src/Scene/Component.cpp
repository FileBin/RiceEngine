#include "Rice/Engine/Engine.hpp"
#include "pch.h"

#include <Rice/Scene/Component.hpp>
#include <Rice/Scene/Object.hpp>
NSP_COMPONENTS_BEGIN

void Component::onInit() {}
void Component::onEnable() {}
void Component::onDisable() {}
void Component::start() {}
void Component::preUpdate() {}
void Component::update() {} 

pEngine Component::getEngine() {
    return object->getScene()->getEngine();
}

pObject Component::getObject() {
    return object;
}

ComponentData::ComponentData(pComponent component) {
    objectUUID = component->object->getUUID();
}

NSP_COMPONENTS_END