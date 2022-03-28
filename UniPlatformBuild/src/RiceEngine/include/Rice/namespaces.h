#pragma once
#ifdef __cplusplus

#define NSP_ENGINE_BEGIN namespace Rice {
#define NSP_ENGINE_END }

#define NSP_GL_BEGIN NSP_ENGINE_BEGIN namespace Graphics {
#define NSP_GL_END NSP_ENGINE_END } 

#define NSP_AL_BEGIN NSP_ENGINE_BEGIN namespace Audio {
#define NSP_AL_END NSP_ENGINE_END } 

#define NSP_UI_BEGIN NSP_ENGINE_BEGIN namespace UI {
#define NSP_UI_END NSP_ENGINE_END }

#define NSP_UTIL_BEGIN NSP_ENGINE_BEGIN namespace Util {
#define NSP_UTIL_END NSP_ENGINE_END }

#endif // __cplusplus