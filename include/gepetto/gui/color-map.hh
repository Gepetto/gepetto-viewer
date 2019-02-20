// Copyright (c) 2015-2018, LAAS-CNRS
// Authors: Joseph Mirabel (joseph.mirabel@laas.fr)
//
// This file is part of gepetto-viewer.
// gepetto-viewer is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// gepetto-viewer is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Lesser Public License for more details. You should have
// received a copy of the GNU Lesser General Public License along with
// gepetto-viewer. If not, see <http://www.gnu.org/licenses/>.

#ifndef GEPETTO_GUI_COLORMAP_HH
#define GEPETTO_GUI_COLORMAP_HH

#include <QColor>
#include <QDebug>

#include <gepetto/viewer/config-osg.h>

namespace gepetto {
  namespace gui {
    class ColorMap {
      public:
        static QColor interpolate (std::size_t nbColors, std::size_t index)
        {
          if (index > nbColors)
            qDebug() << "Nb colors:" << nbColors << "and index" << index;
          return QColor::fromHslF((qreal)index / (qreal)nbColors, 1, 0.5);
        }

        ColorMap (std::size_t nbColors) :
          nbColors_ (nbColors),
          currentIndex_ (0)
      {
        log2up_ = 0;
        mask_ = 0;
        std::size_t val = (nbColors > 0)?nbColors:1;
        for (log2up_ = 0; val; ++log2up_, val >>= 1) mask_ = 2*mask_ + 1;
      }

        QColor getColor (std::size_t index) const {
          return ColorMap::interpolate(mask_, remap (index));
        }

        void getColor (std::size_t index, osgVector4& color) const {
          QColor c = getColor(index);
          color[0] = (float)c.redF();
          color[1] = (float)c.greenF();
          color[2] = (float)c.blueF();
          color[3] = (float)c.alphaF();
        }

        QColor nextColor () {
          QColor color = getColor (currentIndex_);
          currentIndex(currentIndex_ + 1);
          return color;
        }

        void currentIndex (std::size_t index) {
          currentIndex_ = index % nbColors_;
        }

        /// Reverse the order of the bits in index.
        /// This increases the contrast between colors with close indexes.
        std::size_t remap (const std::size_t& index) const {
          std::size_t ret = 0;
          std::size_t input = index;
          for (std::size_t i = 0; i < log2up_; ++i) {
            ret <<= 1;
            if (input & 1) ++ret;
            input >>= 1;
          }
          return ret;
        }

      private:
        std::size_t nbColors_;
        std::size_t mask_;
        std::size_t log2up_;
        std::size_t currentIndex_;
    };
  } // namespace gui
} // namespace gepetto

#endif // GEPETTO_GUI_COLORMAP_HH
