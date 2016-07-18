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

#ifndef TGA_H_
#define TGA_H_

// C++
#include <memory>

namespace Image
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
    Color & operator =(const Color &c)
    {
      if (this != &c)
      {
        bytespp = c.bytespp;
        value = c.value;
      }
      return *this;
    }
  };

  /** \class TGA
   * \brief Implements TGA image format.
   *
   */
  class TGA
  {
    public:
      /** \brief TGA class constructor.
       * \param[in] width image width.
       * \param[in] height image height.
       * \param[in] bpp bits per pixel.
       *
       */
      explicit TGA(const short width, const short height, const short bpp);

      /** \brief TGA class copy constructor.
       *
       */
      TGA(const TGA &img);

      /** \brief TGA class move constructor.
       *
       */
      TGA(TGA &&img);

      /** \brief TGA class virtual destructor.
       *
       */
      virtual ~TGA();

      TGA & operator =(const TGA &img);

      /** TGA image bits per pixel options. */
      enum Format { GRAYSCALE=1, RGB=3, RGBA=4 };

      /** \brief Reads a TGA file from disk and returns a TGA image.
       * \param[in] filename file name.
       *
       */
      static std::shared_ptr<TGA> read(const std::string &filename);

      /** \brief Writes the current image to disk.
       * \param[in] filename file name.
       * \param[in] rle true to use run-lenght encoding and false otherwise.
       *
       */
      bool write(const std::string &filename, bool rle=true);

      /** \brief Flips the image horizontally.
       *
       */
      void flipHorizontally();

      /** \brief Flips the image vertically.
       *
       */
      void flipVertically();

      /** \brief Scales the image.
       * \param[in] width new image width.
       * \param[in] height new image height.
       *
       */
      void scale(const unsigned short width, const unsigned short height);

      /** \brief Returns the color of the given pixel.
       * \param[in] x pixel x coordinate.
       * \param[in] y pixel y coordinate.
       *
       */
      Color get(unsigned short x, unsigned short y);

      /** \brief Sets the color of the given pixel.
       * \param[in] x pixel x coordinate.
       * \param[in] y pixel y coordinate.
       * \param[in] color new pixel color.
       *
       */
      void set(unsigned short x, unsigned short y, const Color &color);

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
      const short getBytespp() const;

      /** \brief Returns a pointer to the raw image data.
       *
       */
      unsigned char *buffer() const;

      /** \brief Returns a pointer to the const data.
       *
       */
      const unsigned char *constBuffer() const;

      /** \brief Sets the image to black color.
       *
       */
      void clear();

    public:
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

      short int      m_width;  /** image width.     */
      short int      m_height; /** image height.    */
      short int      m_bpp;    /** bytes per pixel. */
      unsigned char *m_data;   /** data buffer.     */
  };

} // namespace Image

#endif // TGA_H_
