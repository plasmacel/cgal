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
 
#ifndef CGAL_DIRECTION_3_H
#define CGAL_DIRECTION_3_H

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

CGAL_BEGIN_NAMESPACE

template <class R_>
class Direction_3 : public R_::Kernel_base::Direction_3
{
  typedef typename R_::RT                    RT;
  typedef typename R_::Vector_3              Vector_3;
  typedef typename R_::Line_3                Line_3;
  typedef typename R_::Ray_3                 Ray_3;
  typedef typename R_::Segment_3             Segment_3;
  typedef typename R_::Aff_transformation_3  Aff_transformation_3;

  typedef Direction_3                        Self;
  BOOST_STATIC_ASSERT((boost::is_same<Self, typename R_::Direction_3>::value));

public:

  typedef typename R_::Kernel_base::Direction_3 Rep;

  const Rep& rep() const
  {
    return *this;
  }

  Rep& rep()
  {
    return *this;
  }

  typedef          R_                       R;

  Direction_3() {}

  Direction_3(const Rep& d)
    : Rep(d) {}

  Direction_3(const Vector_3& v)
    : Rep(typename R::Construct_direction_3()(v).rep()) {}

  Direction_3(const Line_3& l)
    : Rep(typename R::Construct_direction_3()(l).rep()) {}

  Direction_3(const Ray_3& r)
    : Rep(typename R::Construct_direction_3()(r).rep()) {}

  Direction_3(const Segment_3& s)
    : Rep(typename R::Construct_direction_3()(s).rep()) {}

  Direction_3(const RT& hx, const RT& hy, const RT& hz)
    : Rep(typename R::Construct_direction_3()(hx, hy, hz).rep()) {}

  Direction_3 transform(const Aff_transformation_3 &t) const
  {
    return t.transform(*this);
  }
 
  Direction_3
  operator-() const
  {
    return R().construct_opposite_direction_3_object()(*this);
  } 
  
  Vector_3 to_vector() const
  {
    return R().construct_vector_3_object()(*this);
  }

  Vector_3 vector() const { return to_vector(); }


  typename Qualified_result_of<typename R::Compute_dx_3, Direction_3>::type
  dx() const
  {
    return R().compute_dx_3_object()(*this);
  }

  typename Qualified_result_of<typename R::Compute_dy_3, Direction_3>::type
  dy() const
  {
    return R().compute_dy_3_object()(*this);
  }

  typename Qualified_result_of<typename R::Compute_dz_3, Direction_3>::type
  dz() const
  {
    return R().compute_dz_3_object()(*this);
  }

  typename Qualified_result_of<typename R::Compute_dx_3, Direction_3>::type
  delta(int i) const
  {
    CGAL_kernel_precondition( i >= 0 && i <= 2 );
    if (i==0) return dx();
    if (i==1) return dy();
    return dz();
  }

};


template <class R >
std::ostream&
insert(std::ostream& os, const Direction_3<R>& d, const Cartesian_tag&) 
{
  typename R::Vector_3 v = d.to_vector();
  switch(os.iword(IO::mode)) {
    case IO::ASCII :
      return os << v.x() << ' ' << v.y()  << ' ' << v.z();
    case IO::BINARY :
      write(os, v.x());
      write(os, v.y());
      write(os, v.z());
      return os;
    default:
      os << "DirectionC3(" << v.x() << ", " << v.y() << ", " << v.z() << ")";
      return os;
  }
}

template <class R >
std::ostream&
insert(std::ostream& os, const Direction_3<R>& d, const Homogeneous_tag&)
{
  switch(os.iword(IO::mode))
  {
    case IO::ASCII :
        return os << d.dx() << ' ' << d.dy() << ' ' << d.dz();
    case IO::BINARY :
        write(os, d.dx());
        write(os, d.dy());
        write(os, d.dz());
        return os;
    default:
        return os << "DirectionH3(" << d.dx() << ", "
                                    << d.dy() << ", "
                                    << d.dz() << ')';
  }
}

template < class R >
std::ostream&
operator<<(std::ostream& os, const Direction_3<R>& d)
{
  return insert(os, d, typename R::Kernel_tag() );
}


template <class R >
std::istream&
extract(std::istream& is, Direction_3<R>& d, const Cartesian_tag&) 
{
  typename R::FT x, y, z;
  switch(is.iword(IO::mode)) {
    case IO::ASCII :
      is >> x >> y >> z;
      break;
    case IO::BINARY :
      read(is, x);
      read(is, y);
      read(is, z);
      break;
    default:
      std::cerr << "" << std::endl;
      std::cerr << "Stream must be in ascii or binary mode" << std::endl;
      break;
  }
  if (is)
      d = Direction_3<R>(x, y, z);
  return is;
}

template <class R >
std::istream&
extract(std::istream& is, Direction_3<R>& d, const Homogeneous_tag&) 
{
  typename R::RT x, y, z;
  switch(is.iword(IO::mode))
  {
    case IO::ASCII :
        is >> x >> y >> z;
        break;
    case IO::BINARY :
        read(is, x);
        read(is, y);
        read(is, z);
        break;
    default:
        std::cerr << "" << std::endl;
        std::cerr << "Stream must be in ascii or binary mode" << std::endl;
        break;
  }
  if (is)
    d = Direction_3<R>(x, y, z);
  return is;
}

template < class R >
std::istream&
operator>>(std::istream& is, Direction_3<R>& d)
{
  return extract(is, d, typename R::Kernel_tag() );
}

CGAL_END_NAMESPACE

#endif // CGAL_DIRECTION_3_H
