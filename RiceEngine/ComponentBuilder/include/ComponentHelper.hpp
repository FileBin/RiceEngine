#include "SerializableComponent.hpp"

#define REFLECTABLE __attribute__((annotate("reflectable")))

#define PPCAT_NX(A, B) A##B
#define PPCAT(A, B) PPCAT_NX(A, B)

#define MOC__ PPCAT(moc__, COMPONENT_NAME)

#ifdef COMPONENT_NSP
#define COMPONENT_NSP_BEGIN namespace COMPONENT_NSP {
#define COMPONENT_NSP_END }
#else
#define COMPONENT_NSP_BEGIN
#define COMPONENT_NSP_END
#endif

#define REGION(v) __REG_##v
#define __REG_public_fields                                                                                                                                                                                                                              \
  public:                                                                                                                                                                                                                                                \
    struct REFLECTABLE Public
#define __REG_private_fields                                                                                                                                                                                                                             \
  public:                                                                                                                                                                                                                                                \
    struct REFLECTABLE Private

#define __REG_editor                                                                                                                                                                                                                                     \
  public:                                                                                                                                                                                                                                                \
    struct REFLECTABLE Editor

#define __REG_methods                                                                                                                                                                                                                                    \
  public:                                                                                                                                                                                                                                                \
    class REFLECTABLE PPCAT(COMPONENT_NAME, _methods) : public virtual Rice::Components::SerializableComponent<PPCAT(COMPONENT_NAME, _methods), Public, Private>

#define COMPONENT_BEGIN                                                                                                                                                                                                                                  \
    COMPONENT_NSP_BEGIN class MOC__ {                                                                                                                                                                                                                    \
        template <typename T> friend struct serializator;                                                                                                                                                                                                \
        friend struct Editor;

#define COMPONENT_END                                                                                                                                                                                                                                    \
    }                                                                                                                                                                                                                                                    \
    ;                                                                                                                                                                                                                                                    \
    typedef MOC__::PPCAT(COMPONENT_NAME, _methods) COMPONENT_NAME;                                                                                                                                                                                       \
    COMPONENT_NSP_END

#include "EditorHelper.hpp"
#include "nameof/nameof.hpp"

#define SHOW_IN_EDITOR(v) static constexpr auto NAMEOF(v) = &MOC__::v