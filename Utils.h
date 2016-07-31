/*
 File: Utils.h
 Created on: 19 jul. 2016
 Author: Felix de las Pozas Alvarez

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UTILS_H_
#define UTILS_H_

// Project
#include <Images.h>
#include "Algebra.h"
#include <chrono>
#include <memory>
#include <string>
#include <mutex>

class Mesh;

namespace Utils
{
  /** \class zBuffer
   * \brief Implements a z buffer.
   *
   */
  class zBuffer
  {
    public:
      /** \brief zBufer class constructor.
       * \param[in] size buffer size.
       *
       */
      zBuffer(const short width, const short height);

      /** \brief zBuffer class destructor.
       *
       */
      ~zBuffer();

      /** \brief Returns the value at the given coordinates.
       * \param[in] x point x coordinate.
       * \param[in] y point y coordinate.
       *
       */
      double get(const unsigned short x, const unsigned short y);

      /** \brief Sets the value at the given coordinate.
       * \param[in] x point x coordinate.
       * \param[in] y point y coordinate.
       * \param[in] value depth value.
       */
      void set(const unsigned short x, const unsigned short y, double value);

      /** \brief Checks the value and sets in the same operation. Returns true on success and false otherwise.
       * \param[in] x point x coordinate.
       * \param[in] y point y coordinate.
       * \param[in] value depth value.
       *
       * NOTE: this avoids race conditions between the get and set in GL triangle method that results in some invalid
       *       pixels in the buffer due to OpenMP. In single thread execution get/set is enough.
       */
      bool checkAndSet(const unsigned short x, const unsigned short y, float value);

      /** \brief Returns the width of the buffer.
       *
       */
      unsigned short getWidth() const;

      /** \brief Returns the height of the buffer.
       *
       */
      unsigned short getHeight() const;

      /** \brief Returns the minimum value in the buffer.
       *
       */
      float getMinimum() const;

      /** \brief Returns the maximum value in the buffer.
       *
       */
      float getMaximum() const;

      /** \brief Writes the buffer to disk.
       *
       */
      void write(const std::string& filename);

    private:
      short      m_width;  /** buffer width.         */
      short      m_height; /** buffer height.        */
      float     *m_data;   /** buffer data.          */
      float      m_min;    /** buffer minimum value. */
      float      m_max;    /** buffer maximum value. */

      mutable std::mutex m_mutex;  /** protects data */
  };

  /** \brief Draws the driangles onto the texture and saves it to disk.
   * \param[in] mesh mesh object.
   * \param[in] filename filename on disk.
   *
   */
  bool dumpTexture(std::shared_ptr<Mesh> mesh, const std::string &filename);

  /** \class Timer
   * \brief Utility class to time an execution block.
   *
   */
  class BlockTimer
  {
    public:
      /** \brief Timer class constructor.
       *
       */
      explicit BlockTimer(const std::string &id);

      /** \brief Timer class destructor.
       *
       */
      ~BlockTimer();

    private:
      const std::string                                    m_id;        /** timer id.                 */
      const std::chrono::high_resolution_clock::time_point m_startTime; /** object construction time. */
  };

} // namespace Utils

#endif // UTILS_H_
