/**
 * @file
 *
 * @author Lawrence Murray <lawrence.murray@csiro.au>
 * $Rev$
 * $Date$
 */
#ifndef BI_MATH_TEMP_MATRIX_HPP
#define BI_MATH_TEMP_MATRIX_HPP

#include "host_matrix.hpp"
#include "../misc/pooled_allocator.hpp"
#include "../misc/pinned_allocator.hpp"

#include "boost/typeof/typeof.hpp"
#include "boost/mpl/if.hpp"

namespace bi {
/**
 * @internal
 *
 * @group math_host
 *
 * @tparam M1 Matrix type.
 */
template<class M1>
struct host_matrix_map_type {
  /**
   * Allocator type.
   */
  typedef pooled_allocator<pinned_allocator<typename M1::value_type> >
      allocator_type;

  /**
   * Equivalent host type.
   */
  typedef host_matrix<typename M1::value_type, allocator_type> host_type;

  /**
   * Type map. Host types map to same, device types map to equivalent %host
   * type.
   */
  typedef typename boost::mpl::if_c<M1::on_device,host_type,M1>::type type;

};

/**
 * @internal
 *
 * @group math_host
 */
template<class M1, bool on_device>
struct host_matrix_map {
  static typename host_matrix_map_type<M1>::type* map(const M1 X);
};

/**
 * @internal
 *
 * @group math_host
 */
template<class M1>
struct host_matrix_map<M1,true> {
  static typename host_matrix_map_type<M1>::type* map(const M1 X) {
    BOOST_AUTO(result, new typename host_matrix_map_type<M1>::type(X.size1(),
        X.size2()));
    *result = X;
    return result;
  }
};

/**
 * @internal
 *
 * @group math_host
 */
template<class M1>
struct host_matrix_map<M1,false> {
  static typename host_matrix_map_type<M1>::type* map(const M1 X) {
    return new typename host_matrix_map_type<M1>::type(X);
  }
};

/**
 * @internal
 *
 * @group math_host
 */
template<class T1>
struct host_matrix_temp_type {
  /**
   * Allocator type.
   */
  typedef pooled_allocator<pinned_allocator<T1> > allocator_type;

  /**
   * Temp type.
   */
  typedef host_matrix<T1,allocator_type> type;
};

/**
 * Construct temporary matrix on %host.
 *
 * @ingroup math_host
 *
 * @tparam T1 Scalar type.
 *
 * @param rows Number of rows.
 * @param cols Number of columns.
 */
template<class T1>
typename host_matrix_temp_type<T1>::type* host_temp_matrix(
    const int rows, const int cols);

/**
 * Construct temporary matrix on %host as copy of arbitrary matrix.
 *
 * @ingroup math_host
 *
 * @tparam M1 Matrix type.
 *
 * @param X
 *
 * @return If @p X is a %host matrix, returns a reference to the same.
 * If @p X is a device matrix, constructs a suitable %host matrix using
 * pooled memory, asynchronously copies contents to this, and returns a
 * reference to it.
 */
template<class M1>
typename host_matrix_map_type<M1>::type* host_map_matrix(const M1 X);

}

template<class T1>
inline typename bi::host_matrix_temp_type<T1>::type* bi::host_temp_matrix(
    const int rows, const int cols) {
  return new typename bi::host_matrix_temp_type<T1>::type(rows, cols);
}

template<class M1>
inline typename bi::host_matrix_map_type<M1>::type* bi::host_map_matrix(
    const M1 X) {
  return host_matrix_map<M1,M1::on_device>::map(X);
}

#endif