/*
 * file: ComponentClass.hpp
 * author: filebin
 * date: 2022-10-05 22:30:38
 */

#ifndef COMPONENT_NAME

#define COMPONENT_NAME ComponentClass

#include "ComponentHelper.h"

COMPONENT_BEGIN

struct Private {};

struct Public {};

COMPONENT_END

#endif // COMPONENT_NAME

COMPONENT_NSP_BEGIN
class COMPONENT_NAME;
COMPONENT_NSP_END

#include "Rice/Scene/Component.hpp"

COMPONENT_NSP_BEGIN

class COMPONENT_NAME : virtual public ::Rice::Components::Component,
                       public PUBLIC,
                       private PRIVATE {
  public:
    COMPONENT_NAME();
    ~COMPONENT_NAME();

  private:
};

COMPONENT_NSP_END
