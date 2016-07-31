/*
 File: TGA.h
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

#ifndef IMAGES_H_
#define IMAGES_H_

// C++
#include <cassert>
#include <memory>

namespace Images
{
  struct Color
  {
    union
    {
      struct
      {
        unsigned char b, g, r, a;  /** color RGBA values        */
      };
      unsigned char raw[4];        /** color raw values access. */
      unsigned int  value;         /** color value.             */
    };

    int bytespp;                   /** bits per pixel.          */

    /** \brief Color struct constructor.
     *
     */
    Color()
    : value  {0}
    , bytespp{1}
    {}

    /** \brief Color struct constructor.
     * \param[in] R red value in [0-255].
     * \param[in] G green value in [0-255].
     * \param[in] B blue value in [0-255].
     * \param[in] A alpha value in [0-255].
     *
     */
    Color(const unsigned char R, const unsigned char G, const unsigned char B, const unsigned char A = 255)
    : b      {B}
    , g      {G}
    , r      {R}
    , a      {A}
    , bytespp{4}
    {}

    /** \brief Color struct constructor.
     * \param[in] v color value.
     * \param[in] bpp bits per pixel.
     *
     */
    Color(const unsigned int v, const int bpp)
    : value  {v}
    , bytespp{bpp}
    {}

    /** \brief Color struct copy constructor.
     *
     */
    Color(const Color &c)
    : value  {c.value}
    , bytespp{c.bytespp}
    {}

    /** \brief Color struct move constructor.
     *
     */
    Color(Color &&c)
    : value  {std::move(c.value)}
    , bytespp{std::move(c.bytespp)}
    {}

    /** \brief Color struct constructor.
     * \param[in] p raw values pointer.
     * \param[in] bpp bits per pixel.
     *
     *
     */
    Color(const unsigned char *p, int bpp)
    : value  {0}
    , bytespp{bpp}
    { for (int i=0; i<bpp; i++) raw[i] = p[i]; }

    /** \brief Color struct assignment constructor.
     * \param[in] c color struct.
     *
     */
    Color & operator=(const Color &c)
    {
      if (this != &c)
      {
        bytespp = c.bytespp;
        value = c.value;
      }
      return *this;
    }

    /** \brief Color multiplication operator
     * \param[in] c constant in [0,1]
     *
     */
    Color & operator*(const float c)
    {
      assert(c >= 0 && c <= 1);
      r *= c;
      g *= c;
      b *= c;
      a *= c;

      return *this;
    }

    /** \brief Color inversion, alpha untouched.
     *
     */
    Color & inverse()
    {
      r = 255-r;
      g = 255-g;
      b = 255-b;

      return *this;
    }

    /** \brief Additive operation
     *
     */
    Color & operator+(const Color &color)
    {
      r = std::min(255, r + color.r);
      g = std::min(255, g + color.g);
      b = std::min(255, b + color.b);
      a = std::min(255, a + color.a);

      return *this;
    }
  };

  /** \class Image
   * \brief Image base class API.
   *
   */
  class Image
  {
    public:
      /** Bits per pixel options. */
      enum Format { GRAYSCALE=1, RGB=3, RGBA=4 };

      /** \brief Image class constructor.
       * \param[in] width image width.
       * \param[in] height image height.
       * \param[in] bpp bits per pixel.
       *
       */
      explicit Image(const short width, const short height, const Format bpp);

      /** \brief Image class virtual destructor.
       *
       */
      virtual ~Image();

      /** \brief Flips the image horizontally.
       *
       */
      virtual void flipHorizontally() = 0;

      /** \brief Flips the image vertically.
       *
       */
      virtual void flipVertically() = 0;

      /** \brief Scales the image.
       * \param[in] width new image width.
       * \param[in] height new image height.
       *
       */
      virtual void scale(const unsigned short width, const unsigned short height) = 0;

      /** \brief Returns the color of the given pixel.
       * \param[in] x pixel x coordinate.
       * \param[in] y pixel y coordinate.
       *
       */
      virtual Color get(unsigned short x, unsigned short y) = 0;

      /** \brief Sets the color of the given pixel.
       * \param[in] x pixel x coordinate.
       * \param[in] y pixel y coordinate.
       * \param[in] color new pixel color.
       *
       */
      virtual void set(unsigned short x, unsigned short y, const Color &color) = 0;

      /** \brief Returns the width of the image.
       *
       */
      const short getWidth() const;

      /** \brief Returns the height of the image.
       *
       */
      const short getHeight() const;

      /** \brief Returns the bits per pixel of the image.
       *
       */
      const Format getBytespp() const;

      /** \brief Writes the current image to disk.
       * \param[in] filename file name.
       *
       */
      virtual bool write(const std::string &filename) = 0;

      /** \brief Sets the image to black color.
       *
       */
      virtual void clear() = 0;

    protected:
      short int m_width;  /** image width.     */
      short int m_height; /** image height.    */
      Format    m_bpp;    /** bytes per pixel. */
  };

  /** \class TGA
   * \brief Implements TGA image format.
   *
   */
  class TGA
  : public Image
  {
    public:
      /** \brief TGA class constructor.
       * \param[in] width image width.
       * \param[in] height image height.
       * \param[in] bpp bits per pixel.
       *
       */
      explicit TGA(const short width, const short height, const Format bpp);

      /** \brief TGA class virtual destructor.
       *
       */
      virtual ~TGA();

      /** \brief TGA class copy constructor.
       *
       */
      TGA(const TGA &img);

      /** \brief TGA class move constructor.
       *
       */
      TGA(TGA &&img);

      /** \brief Operator assignment.
       * \param[in] image
       *
       */
      TGA & operator =(const TGA &image);

      /** \brief Reads a TGA file from disk and returns a TGA image.
       * \param[in] filename file name.
       *
       */
      static std::shared_ptr<Image> read(const std::string &filename);

      virtual bool write(const std::string &filename);

      virtual void flipHorizontally();

      virtual void flipVertically();

      virtual void scale(const unsigned short width, const unsigned short height);

      Color get(unsigned short x, unsigned short y);

      void set(unsigned short x, unsigned short y, const Color &color);

      virtual void clear();

    private:
      /** \brief TGA file header.
       *
       * NOTE: some versions of mingw64 will pack this struct wrong, to enable
       *       the correct behavior you need to add "-mno-ms-bitfields" to the
       *       compilation options.
       *
       */
      struct __attribute__((__packed__)) TGA_Header
      {
        char  idlength;
        char  colormaptype;
        char  datatypecode;
        short colormaporigin;
        short colormaplength;
        char  colormapdepth;
        short x_origin;
        short y_origin;
        short width;
        short height;
        char  bitsperpixel;
        char  imagedescriptor;
      };

      /** \brief Returns a pointer to the raw image data.
       *
       */
      unsigned char *buffer() const;

      /** \brief Returns a pointer to the const data.
       *
       */
      const unsigned char *constBuffer() const;

      /** \brief Writes the current image to disk.
       * \param[in] filename file name.
       * \param[in] rle true to use run-lenght encoding and false otherwise.
       *
       */
      bool write(const std::string &filename, bool rle);

      /** \brief Decode run-length encoded data from the given stream.
       * \param[in] in data stream.
       *
       */
      bool loadRLEdata(std::ifstream &in);

      /** \brief Encodes the image data to the give stream.
       * \param[inout] out data stream.
       *
       */
      bool unloadRLEdata(std::ofstream &out);

    private:
      unsigned char *m_data;   /** data buffer. */
  };

} // namespace Images

#endif // IMAGES_H_
