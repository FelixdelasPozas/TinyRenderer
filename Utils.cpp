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
#include <TGA.h>

// C++
#include <iostream>

using namespace Image;

//--------------------------------------------------------------------
Utils::BlockTimer::BlockTimer(const std::string& id)
: m_id       {id}
, m_startTime{std::chrono::high_resolution_clock::now()}
{
}

//--------------------------------------------------------------------
Utils::BlockTimer::~BlockTimer()
{
  auto endTime = std::chrono::high_resolution_clock::now();
  auto count   = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_startTime).count();

  std::cout << "\"" << m_id << "\" block time: " << count << " milliseconds (" << count/1000 << " seconds)."<< std::endl << std::flush;
}

//--------------------------------------------------------------------
Utils::zBuffer::zBuffer(const short width, const short height)
: m_width {width}
, m_height{height}
, m_min   {std::numeric_limits<float>::max()}
, m_max   {-std::numeric_limits<float>::max()}
{
  assert(width > 0 && height > 0);

  m_data = new float[width*height];

  std::fill_n(m_data, width*height, -std::numeric_limits<float>::max());
}

//--------------------------------------------------------------------
Utils::zBuffer::~zBuffer()
{
  delete [] m_data;
}

//--------------------------------------------------------------------
double Utils::zBuffer::get(const unsigned short x, const unsigned short y)
{
  assert(x < m_width && y < m_height);
  std::lock_guard<std::mutex> lock(m_mutex);

  return m_data[y*m_width + x];
}

//--------------------------------------------------------------------
void Utils::zBuffer::set(const unsigned short x, const unsigned short y, double value)
{
  assert(x < m_width && y < m_height);
  std::lock_guard<std::mutex> lock(m_mutex);

  if(m_min > value) m_min = value;
  if(m_max < value) m_max = value;

  m_data[y*m_width + x] = value;
}

//--------------------------------------------------------------------
unsigned short Utils::zBuffer::getWidth() const
{
  return m_width;
}

//--------------------------------------------------------------------
unsigned short Utils::zBuffer::getHeight() const
{
  return m_height;
}

//--------------------------------------------------------------------
float Utils::zBuffer::getMinimum() const
{
  std::lock_guard<std::mutex> lock(m_mutex);

  return m_min;
}

//--------------------------------------------------------------------
float Utils::zBuffer::getMaximum() const
{
  std::lock_guard<std::mutex> lock(m_mutex);

  return m_max;
}

//--------------------------------------------------------------------
void Utils::zBuffer::write(const std::string& filename)
{
  auto image = std::make_shared<Image::TGA>(m_width, m_height, Image::TGA::GRAYSCALE);
  auto min   = std::fabs(m_min);
  auto delta = 256./(m_max + min);

  for(unsigned short x = 0; x < m_width; ++x)
  {
    for(unsigned short y = 0; y < m_height; ++y)
    {
      auto value = get(x,y) + min;
      if(value == -std::numeric_limits<double>::max()) continue;
      image->set(x, y, Color(value*delta, Image::TGA::GRAYSCALE));
    }
  }

  image->flipVertically();
  image->write(filename);
}
