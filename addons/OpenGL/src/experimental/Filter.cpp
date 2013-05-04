#include <experimental/Filter.h>
#include <experimental/Compositor.h>

namespace gl {

  bool Filter::setup(int w, int h) { 
    return false;
  }

  void Filter::render() {
  }

  // TESTING 
  // --------------------------------------------------------

  void Filter::setInput(Attachment a) {
    input = a;
  }

  void Filter::setOutput(Attachment a) {
    output = a;
  }


} // namespace gl 
