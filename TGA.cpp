/*
 File: TGA.cpp
 Created on: 17/07/2016
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
#include <TGA.h>

// C++
#include <iostream>
#include <fstream>
#include <cassert>
#include <cstring>
#include <iosfwd>

using namespace Image;

//--------------------------------------------------------------------
TGA::TGA(const short width, const short height, const short bpp)
: m_width {width}
, m_height{height}
, m_bpp   {bpp}
, m_data  {new unsigned char[width*height*bpp]}
{
  clear();
}

//--------------------------------------------------------------------
Image::TGA::TGA(const TGA& img)
{
  m_width  = img.m_width;
  m_height = img.m_height;
  m_bpp    = img.m_bpp;

  delete [] m_data;
  auto size = m_width*m_height*m_bpp;
  m_data = new unsigned char[size];
  memcpy(reinterpret_cast<void *>(m_data), reinterpret_cast<void *>(img.m_data), size);
}

//--------------------------------------------------------------------
Image::TGA::TGA(TGA&& img)
: m_width {std::move(img.m_width)}
, m_height{std::move(img.m_height)}
, m_bpp   {std::move(img.m_bpp)}
, m_data  {std::move(img.m_data)}
{
  img.m_data = nullptr;
}

//--------------------------------------------------------------------
TGA::~TGA()
{
  if(m_data) delete [] m_data;
}

//--------------------------------------------------------------------
TGA& TGA::operator =(const TGA& img)
{
  m_width  = img.m_width;
  m_height = img.m_height;
  m_bpp    = img.m_bpp;

  delete [] m_data;
  auto size = m_width*m_height*m_bpp;
  m_data = new unsigned char[size];
  memcpy(reinterpret_cast<void *>(m_data), reinterpret_cast<void *>(img.m_data), size);

  return *this;
}

//--------------------------------------------------------------------
std::shared_ptr<TGA> TGA::read(const std::string& filename)
{
  std::ifstream in;
  in.open(filename, std::ios::binary);
  if (!in.is_open())
  {
    std::cerr << "can't open file " << filename << std::endl;
    in.close();
    return nullptr;
  }

  TGA_Header header;
  in.read(reinterpret_cast<char *>(&header), sizeof(header));
  if (!in.good())
  {
    in.close();
    std::cerr << "an error occured while reading the header." << std::endl;
    return nullptr;
  }

  header.bitsperpixel = header.bitsperpixel >> 3;
  if (header.width <= 0 || header.height <= 0 || (header.bitsperpixel != GRAYSCALE && header.bitsperpixel != RGB && header.bitsperpixel != RGBA))
  {
    in.close();
    std::cerr << "bad bpp (or width/height) value." << std::endl;
    return nullptr;
  }

  auto image = std::make_shared<TGA>(static_cast<short int>(header.width & 0xFFFF), static_cast<short int>(header.height & 0xFFFF), header.bitsperpixel);
  unsigned long nbytes = image->m_bpp * image->m_width * image->m_height;

  if (3 == header.datatypecode || 2 == header.datatypecode)
  {
    in.read(reinterpret_cast<char *>(image->m_data), nbytes);
    if (!in.good())
    {
      in.close();
      std::cerr << "an error occured while reading the data." << std::endl;
      return nullptr;
    }
  }
  else
  {
    if (10 == header.datatypecode || 11 == header.datatypecode)
    {
      if (!image->loadRLEdata(in))
      {
        in.close();
        std::cerr << "an error occured while reading the data." << std::endl;
        return nullptr;
      }
    }
    else
    {
      in.close();
      std::cerr << "unknown file format " << static_cast<int>(header.datatypecode) << std::endl;
      return nullptr;
    }
  }

  if (!(header.imagedescriptor & 0x20))
  {
    image->flipVertically();
  }

  if (header.imagedescriptor & 0x10)
  {
    image->flipHorizontally();
  }

  std::cerr << "read: " << filename << " : " << image->m_width << "x" << image->m_height << "/" << image->m_bpp * 8 << std::endl;

  in.close();
  return image;
}

//--------------------------------------------------------------------
bool TGA::write(const std::string& filename, bool rle)
{
  unsigned char developer_area_ref[4] = {0, 0, 0, 0};
  unsigned char extension_area_ref[4] = {0, 0, 0, 0};
  unsigned char footer[18] = {'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O', 'N', '-', 'X', 'F', 'I', 'L', 'E', '.', '\0'};

  std::ofstream out;
  out.open(filename, std::ios::binary|std::ios::trunc);
  if (!out.is_open())
  {
    std::cerr << "can't open file " << filename << std::endl;
    out.close();
    return false;
  }

  TGA_Header header;
  memset(reinterpret_cast<void *>(&header), 0, sizeof(header));
  header.bitsperpixel = static_cast<char>(m_bpp << 3);
  header.width        = m_width;
  header.height       = m_height;
  header.datatypecode = (m_bpp == GRAYSCALE ? (rle ? 11 : 3) : (rle ? 10 : 2));
  header.imagedescriptor = 0x20; // top-left origin

  out.write(reinterpret_cast<char *>(&header), sizeof(header));
  if (!out.good())
  {
    out.close();
    std::cerr << "can't dump the tga file." << std::endl;
    return false;
  }

  if (!rle)
  {
    out.write(reinterpret_cast<char *>(m_data), m_width * m_height * m_bpp);
    if (!out.good())
    {
      std::cerr << "can't unload raw data." << std::endl;
      out.close();
      return false;
    }
  }
  else
  {
    if (!unloadRLEdata(out))
    {
      out.close();
      std::cerr << "can't unload rle data." << std::endl;
      return false;
    }
  }

  out.write(reinterpret_cast<char *>(developer_area_ref), sizeof(developer_area_ref));
  if (!out.good())
  {
    std::cerr << "can't dump the tga file." << std::endl;
    out.close();
    return false;
  }

  out.write(reinterpret_cast<char *>(extension_area_ref), sizeof(extension_area_ref));
  if (!out.good())
  {
    std::cerr << "can't dump the tga file." << std::endl;
    out.close();
    return false;
  }

  out.write(reinterpret_cast<char *>(footer), sizeof(footer));
  if (!out.good())
  {
    std::cerr << "can't dump the tga file." << std::endl;
    out.close();
    return false;
  }

  out.close();

  std::cout << "write: " << filename << " " << m_width << "x" << m_height << "/" << (m_bpp * 8) << std::endl;

  return true;
}

//--------------------------------------------------------------------
void TGA::flipHorizontally()
{
  for(unsigned short x = 0; x < m_width/2; ++x)
  {
    for(unsigned short y = 0; y < m_height; ++y)
    {
      auto color1 = get(x,y);
      auto color2 = get(m_width-1-x, y);

      set(x,y,color2);
      set(m_width-1-x, y, color1);
    }
  }
}

//--------------------------------------------------------------------
void TGA::flipVertically()
{
  unsigned long lineSize = m_width*m_bpp;
  auto line = new unsigned char[lineSize];

  for (unsigned short y = 0; y < m_height/2; y++)
  {
    auto posLine1 = y*lineSize;
    auto posLine2 = (m_height-1-y)*lineSize;
    memmove((void *)line,              (void *)(m_data+posLine1), lineSize);
    memmove((void *)(m_data+posLine1), (void *)(m_data+posLine2), lineSize);
    memmove((void *)(m_data+posLine2), (void *)line,              lineSize);
  }

  delete [] line;
}

//--------------------------------------------------------------------
void TGA::scale(const unsigned short width, const unsigned short height)
{
  auto data = new unsigned char[width*height*m_bpp];

  int nscanline = 0;
  int oscanline = 0;
  int erry = 0;
  unsigned long nlinebytes = width * m_bpp;
  unsigned long olinebytes = m_width * m_bpp;
  for (int j = 0; j < m_height; j++)
  {
    int errx = m_width - width;
    int nx = -m_bpp;
    int ox = -m_bpp;
    for (int i = 0; i < m_width; i++)
    {
      ox += m_bpp;
      errx += width;
      while (errx >= (int) m_width)
      {
        errx -= m_width;
        nx += m_bpp;
        memcpy(data + nscanline + nx, m_data + oscanline + ox, m_bpp);
      }
    }

    erry += height;
    oscanline += olinebytes;
    while (erry >= (int) m_height)
    {
      if (erry >= (int) m_height << 1) // it means we jump over a scanline
      memcpy(data + nscanline + nlinebytes, m_data + nscanline, nlinebytes);
      erry -= height;
      nscanline += nlinebytes;
    }
  }

  m_width  = width;
  m_height = height;
  delete [] m_data;
  m_data = data;
}

//--------------------------------------------------------------------
Color TGA::get(unsigned short x, unsigned short y)
{
  assert(x < m_width && y < m_height);

  auto pos = (y*m_width + x)*m_bpp;

  return Color(m_data+pos, m_bpp);
}

//--------------------------------------------------------------------
void TGA::set(unsigned short x, unsigned short y, const Color& color)
{
  if(x < m_width && y < m_height)
  {
    auto pos = (y*m_width + x)*m_bpp;

    memcpy(m_data+pos, color.raw, m_bpp);
  }
}

//--------------------------------------------------------------------
const short TGA::getWidth() const
{
  return m_width;
}

//--------------------------------------------------------------------
const short TGA::getHeight() const
{
  return m_height;
}

//--------------------------------------------------------------------
const short TGA::getBytespp() const
{
  return m_bpp;
}

//--------------------------------------------------------------------
unsigned char* TGA::buffer() const
{
  return m_data;
}

//--------------------------------------------------------------------
const unsigned char* TGA::constBuffer() const
{
  return buffer();
}

//--------------------------------------------------------------------
void TGA::clear()
{
  memset(reinterpret_cast<void *>(m_data), 0, m_width*m_height*m_bpp);
}

//--------------------------------------------------------------------
bool TGA::loadRLEdata(std::ifstream& in)
{
  unsigned long pixelcount   = m_width * m_height;
  unsigned long currentpixel = 0;
  unsigned long currentbyte  = 0;
  Color colorbuffer;

  do
  {
    unsigned char chunkheader = 0;
    chunkheader = in.get();
    if (!in.good())
    {
      std::cerr << "an error occured while reading the data." << std::endl;
      return false;
    }

    if (chunkheader < 128)
    {
      chunkheader++;
      for (int i = 0; i < chunkheader; i++)
      {
        in.read(reinterpret_cast<char *>(colorbuffer.raw), m_bpp);
        if (!in.good())
        {
          std::cerr << "an error occured while reading the header." << std::endl;
          return false;
        }

        for (int t = 0; t < m_bpp; t++)
        {
          m_data[currentbyte++] = colorbuffer.raw[t];
        }

        currentpixel++;
        if (currentpixel > pixelcount)
        {
          std::cerr << "Too many pixels read." << std::endl;
          return false;
        }
      }
    }
    else
    {
      chunkheader -= 127;
      in.read(reinterpret_cast<char *>(colorbuffer.raw), m_bpp);
      if (!in.good())
      {
        std::cerr << "an error occured while reading the header." << std::endl;
        return false;
      }

      for (int i = 0; i < chunkheader; i++)
      {
        for (int t = 0; t < m_bpp; t++)
        {
          m_data[currentbyte++] = colorbuffer.raw[t];
        }

        currentpixel++;
        if (currentpixel > pixelcount)
        {
          std::cerr << "Too many pixels read." << std::endl;
          return false;
        }
      }
    }
  }
  while (currentpixel < pixelcount);

  return true;
}

//--------------------------------------------------------------------
bool TGA::unloadRLEdata(std::ofstream& out)
{
  const unsigned char max_chunk_length = 128;
  unsigned long numPixels = m_width * m_height;
  unsigned long currentPix  = 0;

  while (currentPix < numPixels)
  {
    unsigned long chunkstart  = currentPix * m_bpp;
    unsigned long currentByte = currentPix * m_bpp;
    unsigned char run_length  = 1;
    bool raw = true;

    while (currentPix + run_length < numPixels && run_length < max_chunk_length)
    {
      auto succ_equal = true;
      for (int t = 0; succ_equal && t < m_bpp; t++)
      {
        succ_equal = (m_data[currentByte + t] == m_data[currentByte + t + m_bpp]);
      }
      currentByte += m_bpp;

      if (1 == run_length)
      {
        raw = !succ_equal;
      }

      if (raw && succ_equal)
      {
        run_length--;
        break;
      }

      if (!raw && !succ_equal)
      {
        break;
      }

      run_length++;
    }

    currentPix += run_length;
    out.put(raw ? run_length - 1 : run_length + 127);
    if (!out.good())
    {
      std::cerr << "can't dump the tga file." << std::endl;
      return false;
    }

    out.write(reinterpret_cast<char *>(m_data + chunkstart), (raw ? run_length * m_bpp : m_bpp));
    if (!out.good())
    {
      std::cerr << "can't dump the tga file." << std::endl;
      return false;
    }
  }

  return true;
}
