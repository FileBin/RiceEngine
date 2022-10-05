#include "component.hpp"
#include "Rice/namespaces.h"

dbl MyComponents::Camera::getFov() { return fov; }
void MyComponents::Camera::setFov(dbl newFov) { fov = newFov; }