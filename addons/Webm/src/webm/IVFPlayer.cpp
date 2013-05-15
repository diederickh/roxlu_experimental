#include <webm/IVFPlayer.h>
#include <roxlu/core/Utils.h>

void ivf_player_read(unsigned char* pixels, size_t nbytes, void* user) {
  IVFPlayer* p = static_cast<IVFPlayer*>(user);
  p->gl_surface.setPixels(pixels, nbytes);
}

// -------------------------------------------------------------

IVFPlayer::IVFPlayer()
  :ivf(ivf_player_read, this)
{
}

bool IVFPlayer::open(std::string filename, bool datapath) {
  if(!ivf.open(filename, datapath)) {
    return false;
  }

  ivf.print();

  gl_surface.setup(ivf.width, ivf.height, GL_RGBA, GL_RGB, GL_UNSIGNED_BYTE);

  return true;
}

void IVFPlayer::draw(int x, int y, int w, int h) {
  gl_surface.draw(x, y, w, h);
}
