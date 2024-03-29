/*
 *  This file is part of libcxxsupport.
 *
 *  libcxxsupport is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  libcxxsupport is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with libcxxsupport; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/*
 *  libcxxsupport is being developed at the Max-Planck-Institut fuer Astrophysik
 *  and financially supported by the Deutsches Zentrum fuer Luft- und Raumfahrt
 *  (DLR).
 */

/*! \file xcomplex.h
 *  Class for representing complex numbers, strongly inspired by C++'s
 *  std::complex
 *
 *  Copyright (C) 2003-2011 Max-Planck-Society
 *  \author Martin Reinecke
 */

#ifndef PLANCK_XCOMPLEX_H
#define PLANCK_XCOMPLEX_H

#include <iostream>
#include <complex>

/*! \defgroup complexgroup Complex number support */
/*! \{ */

/*! A class for representing complex numbers.

    This template is intended as an (under-encapsulated) replacement for
    the (over-encapsulated) std::complex<>. The goal is to include the
    whole functionality of std::complex<>, with some additional methods
    that allow higher performance.

    The (known and intentional) differences between xcomplex<> and
    std::complex<> are:
     - the default constructor of xcomplex<> does nothing, in contrast to
       std::complex<>, which initialises its members to zero.
     - xcomplex<> implements the methods real() and imag() according
       to defect report DR387
*/
template<typename T> class xcomplex
  {
  public:
    T re, /*!< real part */
      im; /*!< imaginary part */

    /*! Default constructor. \a re and \a im are not initialised. */
    xcomplex () {}
    /*! Creates the complex number (\a re_, \a im_). */
    xcomplex (const T &re_, const T &im_)
      : re(re_), im(im_) {}
    /*! Creates the complex number (\a re_, 0). */
    xcomplex (const T &re_)
      : re(re_), im(0) {}
    /*! Creates an xcomplex from a std::complex of identical precision. */
    xcomplex (const std::complex<T> &orig)
      : re(orig.real()), im(orig.imag()) {}
    /*! Creates a complex number as a copy of \a orig. */
    template<typename U> explicit xcomplex (const xcomplex<U> &orig)
      : re(T(orig.re)), im(T(orig.im)) {}

    /*! Conversion operator to std::complex<T> */
    operator std::complex<T> () const
      { return std::complex<T>(re,im); }

    /*! Returns the real part as lvalue. */
    T &real() { return re; }
    /*! Returns the real part. */
    const T &real() const { return re; }
    /*! Returns the imaginary part as lvalue. */
    T &imag() { return im; }
    /*! Returns the imaginary part. */
    const T &imag() const { return im; }

    /*! Sets the number to (\a re_, \a im_). */
    void Set (const T &re_, const T &im_)
      { re = re_; im = im_; }

    /*! Sets the number to \a orig. */
    xcomplex &operator= (const xcomplex &orig)
      { re=orig.re; im=orig.im; return *this; }
    /*! Sets the number to \a orig. */
    xcomplex &operator= (const std::complex<T> &orig)
      { re=orig.real(); im=orig.imag(); return *this; }
    /*! Sets the number to (\a orig, 0). */
    xcomplex &operator= (const T &orig)
      { re=orig; im=0; return *this; }
    /*! Adds \a b to \a *this. */
    xcomplex &operator+= (const xcomplex &b)
      { re+=b.re; im+=b.im; return *this; }
    /*! Subtracts \a b from \a *this. */
    xcomplex &operator-= (const xcomplex &b)
      { re-=b.re; im-=b.im; return *this; }
    /*! Multiplies \a *this by \a b. */
    xcomplex &operator*= (const xcomplex &b)
      {
      T tmp=re;
      re=tmp*b.re-im*b.im; im=tmp*b.im+im*b.re;
      return *this;
      }
    /*! Divides \a *this by \a b. */
    xcomplex &operator/= (const xcomplex &b)
      {
      std::complex<T> tmp=*this;
      tmp /= std::complex<T>(b);
      *this=tmp;
      return *this;
      }
    /*! Multiplies \a *this by \a fact. */
    xcomplex &operator*= (const T &fact)
      { re*=fact; im*=fact; return *this; }
    /*! Divides \a *this by \a div. */
    xcomplex &operator/= (const T &div)
      { re/=div; im/=div; return *this; }
    /*! Returns \a *this * \a fact. */
    xcomplex operator* (const T &fact) const
      { return xcomplex (re*fact,im*fact); }
    /*! Returns \a *this * \a b. */
    xcomplex operator* (const xcomplex &b) const
      { return xcomplex (re*b.re-im*b.im, re*b.im+im*b.re); }
    /*! Returns \a *this / \a b. */
    xcomplex operator/ (const xcomplex &b) const
      { return xcomplex(std::complex<T>(*this)/std::complex<T>(b)); }
    /*! Returns \a *this / \a div. */
    xcomplex operator/ (const T &div) const
      { return xcomplex (re/div,im/div); }
    /*! Returns \a *this + \a b. */
    xcomplex operator+ (const xcomplex &b) const
      { return xcomplex (re+b.re, im+b.im); }
    /*! Returns \a *this - \a b. */
    xcomplex operator- (const xcomplex &b) const
      { return xcomplex (re-b.re, im-b.im); }
    /*! Returns \a -(*this) */
    xcomplex operator- () const
      { return xcomplex (-re,-im); }

    /*! Flips the signs of both components. */
    void Negate()
      { re=-re; im=-im; }
    /*! Flips the signs of the imaginary component. */
    void Conjugate()
      { im=-im; }
    /*! Multiplies the number by exp(i*\a angle) */
    void Rotate(T angle)
      {
      T ca=cos(angle), sa=sin(angle);
      T tmp=re;
      re=tmp*ca-im*sa; im=tmp*sa+im*ca;
      }
    /*! Returns the complex conjugate of \a *this. */
    xcomplex conj() const
      { return xcomplex (re,-im); }

    /*! Returns the norm of \a *this. */
    T norm() const
      { return re*re + im*im; }
  };

/*! Returns the complex conjugate of \a num.
    \relates xcomplex */
template <typename T> inline xcomplex<T> conj (const xcomplex<T> &num)
  { return xcomplex<T> (num.re, -num.im); }
/*! Returns the norm of \a num.
    \relates xcomplex */
template <typename T> inline T norm (const xcomplex<T> &num)
  { return num.re*num.re + num.im*num.im; }
/*! Returns the absolute value of \a num.
    \relates xcomplex */
template <typename T> inline T abs (const xcomplex<T> &num)
  {
  using namespace std;
  return abs(complex<T>(num));
  }
/*! Returns \a f1*f2.
    \relates xcomplex */
template <typename T> inline xcomplex<T> operator*
  (const T &f1, const xcomplex<T> &f2)
  { return xcomplex<T> (f1*f2.re, f1*f2.im); }
/*! Returns \a f1/f2.
    \relates xcomplex */
template <typename T> inline xcomplex<T> operator/
  (const T &f1, const xcomplex<T> &f2)
  { return xcomplex<T>(f1)/f2; }
/*! Writes \a val to \a os.
    \relates xcomplex */
template<typename T>
  inline std::ostream &operator<< (std::ostream &os, const xcomplex<T> &val)
  { os << "(" << val.re << "," << val.im << ")"; return os; }

/*! \} */

#endif
