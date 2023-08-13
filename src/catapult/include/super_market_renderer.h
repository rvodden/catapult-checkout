#ifndef __SUPER_MARKET_RENDERER_H__
#define __SUPER_MARKET_RENDERER_H__

#include <iostream>

#include "renderer.h"
#include "super_market.h"

namespace catapult
{

class TextRenderer: public BaseRenderer, public Renderer<Inventory> {
  public:
    TextRenderer() = default;
    TextRenderer( std::ostream& outputStream ): _outputStream { outputStream } {};
    void render(const Inventory& inventory) const override;

  private:
    std::reference_wrapper<std::ostream> _outputStream { std::cout };
};

} // namespace catapult


#endif // __SUPER_MARKET_RENDERER_H__
