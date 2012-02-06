// Copyright (c) 2006,2007,2009,2010,2011 Tel-Aviv University (Israel).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
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
// Author(s)     : Baruch Zukerman <baruchzu@post.tau.ac.il>
//                 Ron Wein <wein@post.tau.ac.il>

#ifndef CGAL_ARR_BATCHED_PL_SL_VISITOR_H
#define CGAL_ARR_BATCHED_PL_SL_VISITOR_H

/*!
 * Definition of the Arr_batched_pl_sl_visitor class-template.
 */

#include <CGAL/Arr_point_location/Arr_point_location.h>
#include <CGAL/Object.h>

#include <boost/variant.hpp>
#include <boost/optional.hpp>

namespace CGAL {

/*! \class Arr_batched_pl_sl_visitor
 * A sweep-line visitor for performing batched point-location queries on an
 * arrangement embedded on a surface.
 */
template <class Helper_, class OutputIterator_>
class Arr_batched_pl_sl_visitor : public Helper_::Base_visitor {
public:
  typedef Helper_                                       Helper;
  typedef OutputIterator_                               OutputIterator;

  typedef typename Helper::Traits_2                     Traits_2;
  typedef typename Helper::Arrangement_2                Arrangement_2;
  typedef typename Helper::Event                        Event;
  typedef typename Helper::Subcurve                     Subcurve;

  typedef typename Arrangement_2::Vertex_const_handle   Vertex_const_handle;
  typedef typename Arrangement_2::Halfedge_const_handle Halfedge_const_handle;
  typedef typename Arrangement_2::Face_const_handle     Face_const_handle;

#if CGAL_POINT_LOCATION_VERSION < 2
  typedef CGAL::Object                                  result_type;
#else
  typedef typename boost::variant<Vertex_const_handle,
                                  Halfedge_const_handle,
                                  Face_const_handle>    variant_type;
  typedef typename boost::optional<variant_type>        result_type;
#endif

protected:
  typedef typename Helper::Base_visitor                 Base;
  typedef typename Base::Status_line_iterator           Status_line_iterator;

  // This function returns either make_object() or a result_type constructor
  // to generate return values. The Object version takes a dummy template
  // argument, which is needed for the return of the other option, e.g.,
  // boost::optional<boost::variant> >.
  // In theory a one parameter variant could be returned, but this _could_
  // lead to conversion overhead, and so we rather go for the real type.
  // Overloads for empty returns are also provided.
#if CGAL_POINT_LOCATION_VERSION < 2
  template<typename T>
  inline CGAL::Object result_return(T t) const { return CGAL::make_object(t); }

  inline CGAL::Object result_return() const { return CGAL::Object(); }
#else
  template<typename T>
  inline result_type result_return(T t) const { return result_type(t); }

  inline result_type result_return() const { return result_type(); }
#endif // CGAL_POINT_LOCATION_VERSION < 2
  
  // Data members:
  Helper          m_helper;    // The helper class.
  OutputIterator& m_out;       // An output iterator for the result.

public:
  /*!
   * Constructor.
   * \param arr The arrangement.
   * \param oi A pointer to the output iterator that will store the result.
   */
  Arr_batched_pl_sl_visitor(const Arrangement_2* arr, OutputIterator& oi) :
    m_helper(arr),
    m_out(oi)
  {}
  
  /* A notification issued before the sweep process starts. */
  void before_sweep();

  /*!
   * A notification invoked after the sweep-line finishes handling the given
   * event.
   * \param event The event.
   * \param above An iterator to the sweep-line subcurves lying right above
   *              (or on) the event point.
   * \param on_above Whether the event is locates on the subcurve above it.
   */
  bool after_handle_event(Event* event,
                          Status_line_iterator above,
                          bool on_above);
};

//-----------------------------------------------------------------------------
// Memeber-function definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// A notification issued before the sweep process starts.
//
template <class Hlpr, class OutIt>
void Arr_batched_pl_sl_visitor<Hlpr, OutIt>::before_sweep()
{
  // We just have to notify the helper that the sweep process now starts.
  m_helper.before_sweep();
}

//-----------------------------------------------------------------------------
// A notification invoked after the sweep-line finishes handling the given
// event.
//
template <class Hlpr, class OutIt>
bool Arr_batched_pl_sl_visitor<Hlpr, OutIt>::
after_handle_event(Event* event, Status_line_iterator above, bool on_above)
{
  // Notify the helper on the event.
  m_helper.after_handle_event(event);

  // We are only interested in events associated with query points:
  if (! event->is_query())
    return true;

  // Check on what kind of feature does the current query point lie.
  if (event->is_action()) {
    // The query point coincides with an isolated arrangement vertex:
    Vertex_const_handle  vh = event->point().vertex_handle();
    *m_out++ = std::make_pair(event->point().base(), result_return(vh));
    return true;
  }

  if (event->has_right_curves() || event->has_left_curves()) {
    // The point is located on an arrangement vertex:
    Vertex_const_handle  vh;

    if (event->has_right_curves()) {
      // Take the target vertex of one of the halfedges to the right.
      Subcurve* sc = *(event->right_curves_begin());
      Halfedge_const_handle he = sc->last_curve().halfedge_handle();

      vh = he->target();
    }
    else {
      // Take the source vertex of one of the halfedges to the right.
      Subcurve* sc = *(event->left_curves_begin());
      Halfedge_const_handle he = sc->last_curve().halfedge_handle();

      vh = he->source();
    }

    *m_out++ = std::make_pair(event->point().base(), result_return(vh));
    return true;
  }

  if (above == this->status_line_end()) {
    // There are no valid edges above the query point, so we use the helper
    // class to obtain the current top face.
    *m_out++ =
      std::make_pair(event->point().base(), result_return(m_helper.top_face()));
    return true;
  }

  if (on_above) {
    // The query point lies on the halfedge associated with the subcurve
    // that the status-line iterator refers to.
    Halfedge_const_handle  he = (*above)->last_curve().halfedge_handle();
    *m_out++ = std::make_pair(event->point().base(), result_return(he));
    return true;
  }

  // If we reached here, the status-line iterator refers to a halfedge above
  // the query point, such that the query point is located in the incident
  // face of this halfedge.
  Halfedge_const_handle  he = (*above)->last_curve().halfedge_handle();
  *m_out++ = std::make_pair(event->point().base(), result_return(he->face()));
  return true;
}

} //namespace CGAL

#endif
