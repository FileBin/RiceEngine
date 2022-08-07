//This is library build config file


//Used graphics api for rendering
// Aviable values
//    USE_VULKAN_API
//    USE_DIRECT_X_11 (todo)
//    USE_DIRECT_X_12 (not implemented)
//	  USE_OPENGL (not implemented)

#include "Rice/config.h"
#ifdef GL_API_VULKAN
#define USE_VULKAN_API
#endif
#ifdef GL_API_OPENGL
#define USE_OPENGL
#endif
#ifdef GL_API_DIRECTX
//Choose one of the following
#define USE_DIRECT_X_11
//#define USE_DIRECT_X_12
#endif

#ifdef DEBUG_MODE 
//NOTE: uncomment this line to enable graphics debug mode
#define GL_DEBUG_MODE
#endif