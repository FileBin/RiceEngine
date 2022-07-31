#include "Rice/Util/Exceptions/NullPtrException.hpp"
#include "Rice/Util/Interfaces.hpp"
#include "Rice/Util/Util.hpp"
#include <Rice/Scene/Component.hpp>
#include <Rice/Scene/Object.hpp>
NSP_COMPONENTS_BEGIN

void Component::onInit() {}
void Component::onEnable() {}
void Component::onDisable() {}
void Component::start() {}
void Component::preUpdate() {}
void Component::update() {}

ptr<Engine> Component::getEngine() {
    return getObject()->getScene()->getEngine();
}

ptr<Object> Component::getObject() { return object.lock(); }

NSP_COMPONENTS_END