#ifdef __cplusplus
#pragma once

#define NSP_ENGINE ::Rice

#define NSP_ENGINE_BEGIN namespace Rice {
#define NSP_ENGINE_END }

#define NSP_NET_BEGIN NSP_ENGINE_BEGIN namespace Networking {
#define NSP_NET_END NSP_ENGINE_END }

#define NSP_COMPONENTS_BEGIN NSP_ENGINE_BEGIN namespace Components {
#define NSP_COMPONENTS_END NSP_ENGINE_END }

#define NSP_UI_BEGIN NSP_ENGINE_BEGIN namespace UI {
#define NSP_UI_END NSP_ENGINE_END }

#undef NSP_TESTS_BEGIN
#undef NSP_TESTS_END
#define NSP_TESTS_BEGIN NSP_ENGINE_BEGIN namespace Tests {
#define NSP_TESTS_END NSP_ENGINE_END }

#endif // __cplusplus
