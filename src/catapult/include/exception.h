#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include <stdexcept>

namespace catapult {

class CatapultException: public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};

}  // namespace catapult


#endif  // __EXCEPTION_H__
