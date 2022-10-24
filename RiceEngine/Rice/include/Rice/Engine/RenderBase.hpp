#include "../stdafx.hpp"
NSP_ENGINE_BEGIN
class RenderBase;
NSP_ENGINE_END

#pragma once
#include "../GL/GraphicsManager.hpp"
#include "Engine.hpp"

NSP_ENGINE_BEGIN
class RenderBase {
public:
  RenderBase();
  virtual ~RenderBase();

  virtual void beginFrame() = 0;
  void endFrame();
  void shutdown();
  void setGraphicsManager(Graphics::ptr<GraphicsManager> g_manager);
  void setEngine(ptr<Engine> en);

  Vector2 getViewportSize();

  dbl GetAspectRatio();

  virtual bool init() = 0;
  virtual bool draw() = 0;
  virtual void close() = 0;
  virtual void resize() = 0;

protected:
  ptr<Graphics::GraphicsManager> graphics_mansger;
  ptr<Engine> engine;
};
NSP_ENGINE_END
