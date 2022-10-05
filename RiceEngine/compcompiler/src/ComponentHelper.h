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
#define __REG_public_fields                                                    \
  private:                                                                     \
    struct Public
#define __REG_private_fields                                                   \
  private:                                                                     \
    struct Private

#define __REG_methods                                                          \
  public:                                                                      \
    class PPCAT(COMPONENT_NAME, _methods)                                      \
        : virtual public ::Rice::Components::Component, public Public,         \
          private Private

#define COMPONENT_BEGIN COMPONENT_NSP_BEGIN class MOC__ {

#define COMPONENT_END                                                          \
    }                                                                          \
    ;                                                                          \
    typedef MOC__::PPCAT(COMPONENT_NAME, _methods) COMPONENT_NAME;             \
    COMPONENT_NSP_END