#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <stdexcept>

namespace catapult {

class BaseRenderer {
  public:
    virtual ~BaseRenderer () = default;
};

template<class Renderable>
class Renderer {
  public:
    virtual ~Renderer() = default;
    virtual void render (const Renderable &renderable) const = 0;
};

class BaseRenderable {
  public:
    virtual ~BaseRenderable () = default;
    virtual void acceptRenderer (BaseRenderer &renderer) = 0;
};

template<class RenderableClass>
class Renderable: public BaseRenderable {
  public:
    void acceptRenderer (BaseRenderer &renderer) override {
      try {
        Renderer<RenderableClass> &subRenderer = dynamic_cast<Renderer<RenderableClass> &> (renderer);
        subRenderer.render (static_cast<RenderableClass &> (*this));
      }
      catch ([[maybe_unused]] const std::bad_cast &badCast) {
      };
    }
};

}  // namespace catapult


#endif  // __RENDERER_H__
