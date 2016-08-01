/*
		File: Algebra.h
    Created on: 31/10/2015
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

#ifndef ALGEBRA_H_
#define ALGEBRA_H_

// C++ includes
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cmath>

//--------------------------------------------------------------------
class algebra_error: public std::logic_error
{
  public:
    algebra_error(const std::string& arg)
    : std::logic_error(arg)
    {}
};

//--------------------------------------------------------------------
// Vector class
template<class T, unsigned int N> class Vector
{
  public:
    /** \brief Vector class constructor.
     *
     */
    Vector()
    {
      data.reserve(N);
      for(unsigned int i = 0; i < N; ++i)
      {
        data.push_back(T(0));
      }
    }

    /** \brief Vector copy constructor.
     * \param[in] v vector.
     *
     */
    Vector(const Vector<T, N> &v)
    {
      data.reserve(N);
      for(unsigned int i = 0; i < N; ++i)
      {
        data.push_back(v[i]);
      }
    }

    /** \brief Vector value constructor.
     * \param[in] value T value.
     *
     */
    Vector(const T &value)
    {
      data.reserve(N);
      for(unsigned int i = 0; i < N; ++i)
      {
        data.push_back(value);
      }
    }

    /** \brief Vector initializer list constructor.
     * \param[in] list initializer list.
     *
     */
    template<class P> Vector(std::initializer_list<P> list)
    {
      assert(list.size() == N);
      data = std::vector<T>(begin(list), end(list));
    }

    /** \brief Scalar assignment operator.
     * \param[in] value T value.
     *
     */
    Vector<T, N>& operator=(const T &value)
    {
      for(unsigned int i = 0; i < N; ++i)
      {
        data[i] = value;
      }

      return *this;
    }

    /** \brief Vector assignment operator.
     * \param[in] v vector.
     *
     */
    Vector<T, N>& operator=(const Vector<T, N> &v)
    {
      for(unsigned int i = 0; i < N; ++i)
      {
        data[i] = v[i];
      }

      return *this;
    }

    /** \brief Vector assignment operator.
     * \param[in] v vector.
     *
     */
    template<class X> Vector<T, N>& operator=(const Vector<X, N> &v)
    {
      for(unsigned int i = 0; i < N; ++i)
      {
        data[i] = static_cast<T>(v[i]);
      }

      return *this;
    }

    /** \brief Logical operator ==
     * \param[in] a vector.
     * \param[in] b vector.
     *
     */
    friend bool operator==(const Vector<T, N> &a, const Vector<T, N> &b)
    {
      for(unsigned int i = 0; i < N; ++i)
      {
        if (a[i] != b[i])
        {
          return false;
        }
      }

      return true;
    }

    /** \brief Logical operator !=
     * \param[in] a vector.
     * \param[in] b vector.
     *
     */
    friend bool operator!=(const Vector<T, N> &a, const Vector<T, N> &b)
    {
      return !(a == b);
    }

    /** \brief Subscript operator
     * \param[in] i index value.
     *
     */
    inline T& operator[](unsigned int index)
    {
      assert((0 <= index) && (index < N));

      return data[index];
    }

    /** \brief Const subscript operator
     * \param[in] i index value.
     *
     */
    inline const T& operator[](unsigned int index) const
    {
      assert((0 <= index) && (index < N));

      return data[index];
    }

    /** \brief Unary operator +
     *
     */
    inline const Vector<T, N> operator+()
    {
      Vector<T, N> result = *this;

      return result;
    }

    /** \brief Unary operator -
     *
     */
    inline const Vector<T, N> operator-()
    {
      Vector<T, N> result;
      for(unsigned int i = 0; i < N; ++i)
      {
        result.data[i] = -data[i];
      }

      return result;
    }

    /** \brief Augments the vector with 1 dimension
     *
     */
    inline Vector<T, N+1> augment() const
    {
      Vector<T, N+1> result;

      for(unsigned int i = 0; i < N; ++i)
      {
        result[i] = data[i];
      }
      result[N] = T(1);

      return result;
    }

    /** \brief Projects the vector over the N-1 dimension
     *
     */
    inline Vector<T, N-1> project() const
    {
      Vector<T, N-1> result;

      for(unsigned int i = 0; i < N-1; ++i)
      {
        result[i] = data[i]/data[N-1];
      }

      return result;
    }

    /** \brief Returns the norm of the vector.
     *
     */
    inline double norm()
    {
      double sum = 0;
      for(unsigned int i = 0; i < N; ++i)
      {
        sum += data[i]*data[i];
      }

      return std::sqrt(sum);
    }

    /** \brief Normalizes the vector.
     *
     */
    inline Vector<T, N>& normalize()
    {
      auto _norm = norm();

      for(unsigned int i = 0; i < N; ++i)
      {
        data[i] = data[i] * (1./_norm);
      }

      return *this;
    }

  private:
    std::vector<T> data;
};

/** \brief operator<<
 * \param[in] stream iostream.
 * \param[in] a vector.
 *
 */
template<class T, unsigned int N> std::ostream& operator<<(std::ostream &stream, const Vector<T, N> &v)
{
  stream << "[";
  for(unsigned int i = 0; i < N; ++i)
  {
    stream << v[i] << ((i == N-1) ? "]" : ",");
  }

  return stream;
}

/** \brief operator* (scalar*vector)
 * \param[in] c T value.
 * \param[in] a vector.
 */
template<class T, unsigned int N> inline Vector<T, N> operator*(const T &c, const Vector<T, N> &v)
{
  Vector<T, N> tmp(0);

  for(unsigned int i = 0; i < N; ++i)
  {
    tmp[i] = v[i] * c;
  }

  return tmp;
}

/** \brief operator* (vector*value)
 * \param[in] v vector.
 * \param[in] c T value.
 *
 */
template<class T, unsigned int N> inline Vector<T, N> operator*(const Vector<T, N> &v, const T &c)
{
  Vector<T, N> tmp(0);

  for(unsigned int i = 0; i < N; ++i)
  {
    tmp[i] = v[i] * c;
  }

  return tmp;
}

/** \brief operator* (vector*value)
 * \param[in] v vector.
 * \param[in] c X value.
 *
 */
template<class T, unsigned int N, class X> inline Vector<T, N> operator*(const Vector<T, N> &v, const X &c)
{
  Vector<T, N> tmp(0);

  for(unsigned int i = 0; i < N; ++i)
  {
    tmp[i] = v[i] * static_cast<T>(c);
  }

  return tmp;
}

/** \brief operator/ (vector/value)
 * \param[in] v vector.
 * \param[in] c T value.
 *
 */
template<class T, unsigned int N, class X> inline Vector<T, N> operator/(const Vector<T, N> &v, const X &c)
{
  if (c == T(0))
  {
    throw algebra_error("Vector::operator/: division by zero");
  }

  Vector<T, N> tmp(0);

  for(unsigned int i = 0; i < N; ++i)
  {
    tmp[i] = v[i] / static_cast<T>(c);
  }

  return tmp;
}

/** \brief operator+ (binary)
 * \param[in] a vector.
 * \param[in] b vector.
 *
 */
template<class T, unsigned int N> inline Vector<T, N> operator+(const Vector<T, N> &a, const Vector<T, N> &b)
{
  Vector<T, N> result(0);

  for(unsigned int i = 0; i < N; ++i)
  {
    result[i] = a[i] + b[i];
  }

  return result;
}

/** \brief operator- (binary)
 * \param[in] a vector.
 * \param[in] b vector.
 *
 */
template<class T, unsigned int N> inline Vector<T, N> operator-(const Vector<T, N> &a, const Vector<T, N> &b)
{
  Vector<T, N> result(0);

  for(unsigned int i = 0; i < N; ++i)
  {
    result[i] = a[i] - b[i];
  }

  return result;
}

/** \brief operator* (binary, dot product)
 * \param[in] a vector.
 * \param[in] b vector.
 *
 */
template<class T, unsigned int N> inline T operator*(const Vector<T, N> &a, const Vector<T, N> &b)
{
  T sum = 0;

  for(unsigned int i = 0; i < N; ++i)
  {
    sum += a[i] * b[i];
  }

  return sum;
}

/** \brief operator+= (binary)
 * \param[in] a vector.
 * \param[in] b vector.
 *
 */
template<class T, unsigned int N> inline Vector<T, N> operator+=(Vector<T, N> &a, const Vector<T, N> &b)
{
  for(unsigned int i = 0; i < N; ++i)
  {
    a[i] = a[i] + b[i];
  }

  return a;
}

/** \brief operator-= (binary)
 * \param[in] a vector.
 * \param[in] b vector.
 *
 */
template<class T, unsigned int N> inline Vector<T, N> operator-=(Vector<T, N> &a, const Vector<T, N> &b)
{
  for(unsigned int i = 0; i < N; ++i)
  {
    a[i] = a[i] - b[i];
  }

  return a;
}

/** \brief operator*= (vector*value)
 * \param[in] a vector.
 * \param[in] c X value.
 *
 */
template<class T, unsigned int N> inline Vector<T, N> operator*=(const Vector<T, N> &v, const T &c)
{
  for(unsigned int i = 0; i < N; ++i)
  {
    v[i] = v[i] * c;
  }
}

/** \brief operator*= (vector*value)
 * \param[in] a vector.
 * \param[in] c X value.
 *
 */
template<class T, unsigned int N, class X> inline Vector<T, N> operator*=(const Vector<T, N> &v, const X &c)
{
  for(unsigned int i = 0; i < N; ++i)
  {
    v[i] = v[i] * static_cast<T>(c);
  }
}

//--------------------------------------------------------------------
// Matrix class
//
template<class T, unsigned int N> class Matrix
{
  public:
    /** \brief Matrix3 default constructor.
     *
     */
    Matrix()
    : Matrix{0}
    {
    }

    /** \brief Matrix3 constructor with a matrix.
     * \param[in] a matrix.
     *
     */
    Matrix(const Matrix<T, N> &a)
    {
      data.reserve(N);
      for(unsigned int i = 0; i < N; ++i)
      {
        data.push_back(a.row(i));
      }
    }

    /** \brief Matrix3 constructor with a scalar.
     * \param[in] value scalar value.
     *
     */
    Matrix(const T& scalar)
    {
      for(unsigned int i = 0; i < N; ++i)
      {
        data.push_back(Vector<T,N>(scalar));
      }
    }

    /** \brief Operator =(scalar)
     * \param[in] scalar scalar value.
     *
     */
    const Matrix<T,N>& operator=(T& scalar)
    {
      for(unsigned int i = 0; i < N; ++i)
      {
        data[i] = Vector<T, N>(scalar);
      }

      return *this;
    }

    /** \brief Operator =(matrix)
     * \param[in] a matrix.
     *
     */
    Matrix<T, N>& operator=(const Matrix<T,N> &a)
    {
      for(unsigned int i; i < N; ++i)
      {
        data[i] = a[i];
      }

      return *this;
    }

    /** \brief Operator =(matrix)
     * \param[in] a matrix.
     *
     */
    template<class X> Matrix<T, N>& operator=(const Matrix<X,N> &a)
    {
      for(unsigned int i; i < N; ++i)
      {
        for(unsigned int j; j < N; ++j)
        {
          data[i][j] = static_cast<T>(a[i][j]);
        }
      }

      return *this;
    }

    /** \brief Operator ==
     * \param[in] a matrix.
     * \param[in] b matrix.
     *
     */
    friend bool operator==(const Matrix<T, N>& a, const Matrix<T, N>& b)
    {
      for(unsigned int i; i < N; ++i)
      {
        for(unsigned int j; j < N; ++j)
        {
          if (a[i][j] != b[i][j])
          {
            return false;
          }
        }
      }

      return true;
    }

    /** \brief Operator !=
     * \param[in] a matrix.
     * \param[in] b matrix.
     *
     */
    friend bool operator!=(const Matrix<T, N>& a, const Matrix<T, N>& b)
    {
      return !(a == b);
    }

    /** \brief Returns true if the matrix is null.
     *
     */
    const bool isNull() const
    {
      for(unsigned int i; i < N; ++i)
      {
        for(unsigned int j; j < N; ++j)
        {
          if (data[i][j] != 0)
          {
            return false;
          }
        }
      }

      return true;
    }

    /** \brief Subscript operator.
     * \param[in] i vector index.
     *
     */
    inline T& operator()(const unsigned int i, const unsigned int j)
    {
      assert((0 <= i) && (i < N));
      assert((0 <= j) && (j < N));

      return data[i][j];
    }

    /** \brief Const subscript operator.
     * \param[in] i vector index.
     *
     */
    inline const T& operator()(const unsigned int i, const unsigned int j) const
    {
      assert((0 <= i) && (i < N));
      assert((0 <= j) && (j < N));

      return data[i][j];
    }

    /** \brief Subscript operator.
     * \param[in] i vector index.
     *
     */
    inline Vector<T, N>& operator[](const unsigned int i)
    {
      assert((0 <= i) && (i < N));

      return data[i];
    }

    /** \brief Const subscript operator.
     * \param[in] i vector index.
     *
     */
    inline const Vector<T, N>& operator[](const unsigned int i) const
    {
      assert((0 <= i) && (i < N));

      return data[i];
    }

    /** \brief Unary operator +
     *
     */
    inline const Matrix<T, N> operator+()
    {
      Matrix<T, N> result = *this;

      return result;
    }

    /** \brief Unary operator -
     *
     */
    inline const Matrix<T, N> operator-()
    {
      Matrix<T, N> result;
      for(unsigned int i = 0; i < N; ++i)
      {
        result[i] = -data[i];
      }

      return result;
    }

    /** \brief Transpose operator.
     *
     */
    Matrix<T, N> transpose()
    {
      Matrix<T, N> result;

      for(unsigned int i = 0; i < N; ++i)
      {
        for(unsigned int j = 0; j < N; ++j)
        {
          result[i][j] = data[j][i];
        }
      }

      return result;
    }

    Matrix<T, N> inverse()
    {
      // augmenting the square matrix with the identity matrix of the same dimensions.
      std::vector<Vector<T, 2*N>> result;
      result.reserve(N);
      for (unsigned int i = 0; i < N; i++)
      {
        result.push_back(Vector<T,2*N>());
      }

      for (unsigned int i = 0; i < N; i++)
      {
        result[i][i+N] = 1;

        for (unsigned int j = 0; j < N; j++)
        {
          result[i][j]   = data[i][j];
        }
      }

      // first pass
      for (unsigned int i = 0; i < N-1; i++)
      {
        // normalize the first row
        for (int j = (2*N)-1; j >= 0; j--)
        {
          result[i][j] /= result[i][i];
        }
        for (unsigned int k = i + 1; k < N; k++)
        {
          float coeff = result[k][i];
          for (unsigned int j = 0; j < 2*N; j++)
          {
            result[k][j] -= result[i][j] * coeff;
          }
        }
      }

      // normalize the last row
      for (unsigned int j = (2*N) - 1; j >= N-1; j--)
      {
        result[N-1][j] /= result[N-1][N-1];
      }

      // second pass
      for (int i = N-1; i > 0; i--)
      {
        for (int k = i - 1; k >= 0; k--)
        {
          float coeff = result[k][i];
          for (unsigned int j = 0; j < 2*N; j++)
          {
            result[k][j] -= result[i][j] * coeff;
          }
        }
      }

      // cut the identity matrix back
      Matrix<T,N> truncate;
      for (unsigned int i = 0; i < N; i++)
      {
        for (unsigned int j = 0; j < N; j++)
        {
          truncate[i][j] = result[i][j + N];
        }
      }

      return truncate;
    }

    /** \brief Returns the sub-matrix of the position (i,j)
     * \param[in] i row.
     * \param[in] j column.
     *
     */
    Matrix<T, N-1> sub(unsigned int i, unsigned int j)
    {
      Matrix<T, N-1> result;

      unsigned int row = 0;
      for(unsigned int k = 0; k < N; ++k)
      {
        Vector<T, N-1> v;
        if(k == i) continue;
        unsigned int column = 0;
        for(unsigned int l = 0; l < N; ++l)
        {
          if(l == j) continue;
          v[column++] = data[k][l];
        }

        result[row++] = v;
      }

      return result;
    }

    /** \brief Determinant operator.
     *
     */
    float determinant()
    {
      if(N == 2) return (data[0][0]*data[1][1]) - (data[0][1]*data[1][0]);

      float result{0};
      auto sign = [](unsigned int i) { return (i % 2) ? -1 : 1; };

      for(unsigned int i = 0; i < N; ++i)
      {
        result += sign(i) * data[0,i] * sub(0,i).determinant();
      }

      return result;
    }

    /** \brief Identity operator.
     *
     */
    Matrix<T, N>& identity()
    {
      for(unsigned int i = 0; i < N; ++i)
      {
        for(unsigned int j = 0; j < N; ++j)
        {
          data[i][j] = ((i == j) ? T(1) : T(0));
        }
      }

      return *this;
    }

    /** \brief Row access.
     * \param[in] i row index.
     *
     */
    Vector<T, N> row(const unsigned int i) const
    {
      assert((0 <= i) && (i < N));

      return data[i];
    }

    /** \brief Column access.
     * \param[in] i column index.
     *
     */
    Vector<T, N> column(const unsigned int i) const
    {
      assert((0 <= i) && (i < N));

      Vector<T, N> result;
      for(unsigned int j = 0; j < N; ++j)
      {
        result[j] = data[j][i];
      }

      return result;
    }

  private:
    std::vector<Vector<T, N>> data;
};

/** \brief operator <<
 * \param[in] stream iostream.
 * \param[in] m matrix.
 *
 */
template<class T, unsigned int N> std::ostream& operator<<(std::ostream &stream, const Matrix<T, N> &m)
{
  for(unsigned int i = 0; i < N; ++i)
  {
    stream << m[i] << std::endl;
  }
  return stream;
}

/** \brief operator+ (binary, matrix)
 * \param[in] a matrix.
 * \param[in] b matrix.
 *
 */
template<class T, unsigned int N> inline Matrix<T, N> operator+(const Matrix<T, N> &a, const Matrix<T, N> &b)
{
  Matrix<T, N> tmp(0);

  for(unsigned int i = 0; i < N; ++i)
  {
    for(unsigned int j = 0; j < N; ++j)
    {
      tmp(i, j) = a(i, j) + b(i, j);
    }
  }

  return tmp;
}

/** \brief operator+ (binary, value)
 * \param[in] m matrix.
 * \param[in] value T value.
 *
 */
template<class T, unsigned int N> inline Matrix<T, N> operator+(const Matrix<T, N> &m, const T &value)
{
  Matrix<T, N> tmp(0);

  for(unsigned int i = 0; i < N; ++i)
  {
    for(unsigned int j = 0; j < N; ++j)
    {
      tmp(i, j) = m(i, j) + value;
    }
  }

  return tmp;
}

/** \brief operator- (binary, matrix)
 * \param[in] a matrix.
 * \param[in] b matrix.
 *
 */
template<class T, unsigned int N> inline Matrix<T, N> operator-(const Matrix<T, N> &a, const Matrix<T, N> &b)
{
  Matrix<T, N> tmp(0);

  for(unsigned int i = 0; i < N; ++i)
  {
    for(unsigned int j = 0; j < N; ++j)
    {
      tmp(i, j) = a(i, j) - b(i, j);
    }
  }

  return tmp;
}

/** \brief operator- (binary, value)
 * \param[in] m matrix.
 * \param[in] value T value.
 */
template<class T, unsigned int N> inline Matrix<T, N> operator-(const Matrix<T, N> &m, const T &value)
{
  Matrix<T, N> tmp;

  for(unsigned int i = 0; i < N; ++i)
  {
    for(unsigned int j = 0; j < N; ++j)
    {
      tmp(i, j) = m(i, j) - value;
    }
  }

  return tmp;
}

/** \brief operator/ (binary, scalar)
 * \param[in] m matrix.
 * \parma[in] value T value.
 *
 */
template<class T, unsigned int N> inline Matrix<T, N> operator/(const Matrix<T, N> &m, const T &value)
{
  Matrix<T, N> tmp;

  for(unsigned int i = 0; i < N; ++i)
  {
    for(unsigned int j = 0; j < N; ++j)
    {
      tmp(i, j) = m(i, j) / value;
    }
  }

  return tmp;
}

/** \brief operator* (binary, matrix)
 * \param[in] a matrix.
 * \param[in] b matrix.
 *
 */
template<class T, unsigned int N> inline Matrix<T, N> operator*(const Matrix<T, N> &a, const Matrix<T, N> &b)
{
  Matrix<T, N> tmp;
  T sum;

  for(unsigned int i = 0; i < N; ++i)
  {
    for(unsigned int k = 0; k < N; ++k)
    {
      sum = 0;
      for(unsigned int j = 0; j < N; ++j)
      {
        sum = sum + (a(i, j) * b(j, k));
      }

      tmp(i, k) = sum;
    }
  }

  return tmp;
}

/** \brief operator* (binary, value)
 * \param[in] m matrix.
 * \param[in] value T value.
 *
 */
template<class T, unsigned int N> inline Matrix<T, N> operator*(const Matrix<T, N> &m, const T &value)
{
  Matrix<T, N> tmp;

  for(unsigned int i = 0; i < N; ++i)
  {
    tmp[i] = m[i] * value;
  }

  return tmp;
}

/** \brief operator* (binary, scalar)
 * \param[in] a matrix.
 * \param[in] b matrix.
 *
 */
template<class T, unsigned int N> inline Matrix<T, N> operator*(const T scalar, const Matrix<T, N> &a)
{
  Matrix<T, N> tmp;

  for(unsigned int i = 0; i < N; ++i)
  {
    tmp[i] = a[i] * scalar;
  }

  return tmp;
}

/** \brief operator* (binary, vector)
 * \param[in] v vector.
 * \param[in] m matrix.
 *
 */
template<class T, unsigned int N> inline Vector<T, N> operator*(const Vector<T, N> &v, const Matrix<T, N> &m)
{
  Vector<T, N> tmp;
  T sum;

  for(unsigned int i = 0; i < N; ++i)
  {
    sum = 0;

    for(unsigned int j = 0; j < N; ++j)
    {
      sum = sum + (m(j, i) * v[j]);
    }

    tmp[i] = sum;
  }

  return tmp;
}

/** \brief operator* (binary, vector)
 * \param[in] m matrix.
 * \param[in] v vector.
 *
 */
template<class T, unsigned int N> inline Vector<T, N> operator*(const Matrix<T, N> &m, const Vector<T, N> &v)
{
  Vector<T, N> tmp;
  T sum;

  for(unsigned int i = 0; i < N; ++i)
  {
    sum = 0;

    for(unsigned int j = 0; j < N; ++j)
    {
      sum = sum + (m(i, j) * v[j]);
    }

    tmp[i] = sum;
  }

  return tmp;
}

//--------------------------------------------------------------------
// Special cases
//
template<class T> inline Vector<T,3> operator^ (const Vector<T,3> &v, const Vector<T,3> &w)
{
  Vector<T,3> result;
  result[0] = v[1] * w[2] - v[2] * w[1];
  result[1] = v[2] * w[0] - v[0] * w[2];
  result[2] = v[0] * w[1] - v[1] * w[0];

  return result;
}

using Vector4ui  = Vector<unsigned int,4>;
using Vector4i   = Vector<int, 4>;
using Vector4f   = Vector<float, 4>;
using Vector4d   = Vector<double, 4>;
using Vector4ul  = Vector<unsigned long int, 4>;
using Vector4ull = Vector<unsigned long long int, 4>;
using Vector4ll  = Vector<long long int, 4>;

using Vector3ui  = Vector<unsigned int,3>;
using Vector3i   = Vector<int, 3>;
using Vector3f   = Vector<float, 3>;
using Vector3d   = Vector<double, 3>;
using Vector3ul  = Vector<unsigned long int, 3>;
using Vector3ull = Vector<unsigned long long int, 3>;
using Vector3ll  = Vector<long long int, 3>;

using Vector2ui  = Vector<unsigned int, 2>;
using Vector2i   = Vector<int, 2>;
using Vector2f   = Vector<float, 2>;
using Vector2d   = Vector<double, 2>;
using Vector2ul  = Vector<unsigned long int, 2>;
using Vector2ull = Vector<unsigned long long int, 2>;
using Vector2ll  = Vector<long long int, 2>;

using Matrix3ui = Matrix<unsigned int, 3>;
using Matrix3i  = Matrix<int, 3>;
using Matrix3f  = Matrix<float, 3>;
using Matrix3d  = Matrix<double, 3>;

using Matrix4ui = Matrix<unsigned int, 4>;
using Matrix4i  = Matrix<int, 4>;
using Matrix4f  = Matrix<float, 4>;
using Matrix4d  = Matrix<double, 4>;

#endif // ALGEBRA_H_
