// This is library build config file

// Used graphics api for rendering
//  Aviable values
//     USE_VULKAN_API
//     USE_DIRECT_X_11 (todo)
//     USE_DIRECT_X_12 (not implemented)
//	  USE_OPENGL (not implemented)

#define USE_VULKAN_API

#ifdef DEBUG_MODE
// NOTE: uncomment this line to enable graphics debug mode
#define GL_DEBUG_MODE
#endif