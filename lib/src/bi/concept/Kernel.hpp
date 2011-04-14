/**
 * @file
 *
 * @author Lawrence Murray <lawrence.murray@csiro.au>
 * $Rev: 1254 $
 * $Date: 2011-02-02 17:36:34 +0800 (Wed, 02 Feb 2011) $
 */
#error "Concept documentation only, should not be #included"

namespace concept {
/**
 * %Kernel.
 *
 * @ingroup concept
 *
 * @note This is a phony class, representing a concept, for documentation
 * purposes only.
 */
struct Kernel {
  /**
   * Evaluate the kernel.
   *
   * @param x \f$x\f$; point at which to evaluate the kernel.
   *
   * @return \f$\mathcal{K}(x)\f$; density of the kernel at the given point.
   */
  template<class T1>
  T1 operator()(const T1 x) const = 0;
};
}