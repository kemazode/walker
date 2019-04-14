#ifndef IMAGES_HPP
#define IMAGES_HPP

#include "utils.hpp"

enum image_position
{
  IMAGE_POSITION_TOP,
  IMAGE_POSITION_LEFT,
};

enum image
{
  IMAGE_COW,
  IMAGE_CENTAUR,
  IMAGE_HORSEBACK_FIGHT,
  IMAGE_MOUNTAINS,
  IMAGE_PIKEMAN,
  IMAGE_SCROLL_AND_INK_PEN,
  IMAGE_OPEN_BOOK,
};

extern const text *images[];
text images_find(const char *image);

#endif // IMAGES_HPP
