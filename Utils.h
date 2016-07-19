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

// C++
#include <chrono>
#include <string>

namespace Utils
{
  /** \class Timer
   * \brief Utility class to time an execution block.
   *
   */
  class Timer
  {
    public:
      /** \brief Timer class constructor.
       *
       */
      explicit Timer(const std::string &id);

      /** \brief Timer class destructor.
       *
       */
      ~Timer();

    private:
      const std::string                                    m_id;        /** timer id.                 */
      const std::chrono::high_resolution_clock::time_point m_startTime; /** object construction time. */

  };

} // namespace Utils

#endif // UTILS_H_
