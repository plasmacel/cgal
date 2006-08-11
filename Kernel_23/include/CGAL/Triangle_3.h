// Copyright (c) 1999  Utrecht University (The Netherlands),
// ETH Zurich (Switzerland), Freie Universitaet Berlin (Germany),
// INRIA Sophia-Antipolis (France), Martin-Luther-University Halle-Wittenberg
// (Germany), Max-Planck-Institute Saarbruecken (Germany), RISC Linz (Austria),
// and Tel-Aviv University (Israel).  All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; version 2.1 of the License.
// See the file LICENSE.LGPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
// 
//
// Author(s)     : Andreas Fabri, Stefan Schirra

#ifndef CGAL_TRIANGLE_3_H
#define CGAL_TRIANGLE_3_H

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

CGAL_BEGIN_NAMESPACE

template <class R_>
class Triangle_3 : public R_::Kernel_base::Triangle_3
{
  typedef typename R_::RT                    RT;
  typedef typename R_::FT                    FT;
  typedef typename R_::Point_3               Point_3;
  typedef typename R_::Plane_3               Plane_3;
  typedef typename R_::Aff_transformation_3  Aff_transformation_3;

  typedef Triangle_3                         Self;
  BOOST_STATIC_ASSERT((boost::is_same<Self, typename R_::Triangle_3>::value));

public:

  typedef typename R_::Kernel_base::Triangle_3  Rep;

  const Rep& rep() const
  {
    return *this;
  }

  Rep& rep()
  {
    return *this;
  }

  typedef          R_                       R;

  Triangle_3() {}

  Triangle_3(const Rep& t)
      : Rep(t) {}

  Triangle_3(const Point_3& p, const Point_3& q, const Point_3& r)
    : Rep(typename R::Construct_triangle_3()(p, q, r).rep()) {}

  Triangle_3 transform(const Aff_transformation_3 &t) const
  {
    return Triangle_3(t.transform(this->vertex(0)),
                      t.transform(this->vertex(1)),
                      t.transform(this->vertex(2)));
  }

  Plane_3 supporting_plane() const
  {
    return R().construct_supporting_plane_3_object()(*this);
  }

  bool has_on(const Point_3 &p) const
  {
    return R().has_on_3_object()(*this, p);
  }

  Point_3 vertex(int i) const // TODO : use Qrt
  {
    return R().construct_vertex_3_object()(*this, i);
  }

  Point_3 operator[](int i) const // TODO : use Qrt
  {
    return vertex(i);
  }

  bool is_degenerate() const
  {
    return R().is_degenerate_3_object()(*this);
  }

  FT squared_area() const // TODO : use Qrt
  {
    return R().compute_squared_area_3_object()(*this);
  }

};


template < class R >
std::ostream &
operator<<(std::ostream &os, const Triangle_3<R> &t)
{
    switch(os.iword(IO::mode)) {
    case IO::ASCII :
        return os << t[0] << ' ' << t[1] << ' ' << t[2];
    case IO::BINARY :
        return os << t[0]  << t[1]  << t[2];
    default:
        os << "Triangle_3(" << t[0] <<  ", " << t[1] <<   ", " << t[2] <<")";
        return os;
    }
}

template < class R >
std::istream &
operator>>(std::istream &is, Triangle_3<R> &t)
{
    typename R::Point_3 p, q, r;

    is >> p >> q >> r;

    if (is)
        t = Triangle_3<R>(p, q, r);
    return is;
}

CGAL_END_NAMESPACE

#endif // CGAL_TRIANGLE_3_H
