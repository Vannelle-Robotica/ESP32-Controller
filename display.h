#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino_GFX_Library.h>
#include <U8g2lib.h>

namespace Controller {
  class Display{
    public:
    
    void refresh(const String &);

  };
  
}
#endif
