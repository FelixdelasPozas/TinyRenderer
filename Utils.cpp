/*
 File: Utils.cpp
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

// Project
#include <Utils.h>

// C++
#include <iostream>

//--------------------------------------------------------------------
Utils::Timer::Timer(const std::string& id)
: m_id       {id}
, m_startTime{std::chrono::high_resolution_clock::now()}
{

}

//--------------------------------------------------------------------
Utils::Timer::~Timer()
{
  auto endTime = std::chrono::high_resolution_clock::now();
  auto count   = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_startTime).count();

  std::cout << "\"" << m_id << "\" block time: " << count << " milliseconds (" << count/1000 << " seconds)."<< std::endl << std::flush;
}
