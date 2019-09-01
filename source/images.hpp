/* This file is part of Walker.
 *
 * Walker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Walker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Walker.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef IMAGES_HPP
#define IMAGES_HPP

#include "utils.hpp"

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
