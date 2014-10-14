/**
 * @file
 *
 * @author Lawrence Murray <lawrence.murray@csiro.au>
 * $Rev$
 * $Date$
 */
#ifndef BI_RESAMPLER_STRATIFIEDRESAMPLER_HPP
#define BI_RESAMPLER_STRATIFIEDRESAMPLER_HPP

#include "ScanResampler.hpp"
#include "../cuda/cuda.hpp"
#include "../math/vector.hpp"
#include "../random/Random.hpp"
#include "../misc/exception.hpp"

namespace bi {
/**
 * Stratified resampler for particle filter.
 *
 * @ingroup method_resampler
 *
 * Stratified resampler based on the scheme of
 * @ref Kitagawa1996 "Kitagawa (1996)".
 */
class StratifiedResampler: public ScanResampler {
public:
  /**
   * Constructor.
   *
   * @param essRel Minimum ESS, as proportion of total number of particles,
   * to trigger resampling.
   */
  StratifiedResampler(const double essRel = 0.5);

  /**
   * @name Low-level interface
   */
  //@{
  /**
   * Select cumulative offspring.
   *
   * @tparam V1 Vector type.
   * @tparam V2 Integer vector type.
   *
   * @param[in,out] rng Random number generator.
   * @param lws Log-weights.
   * @param[out] Os Cimulative offspring.
   * @param P Total number of offspring to select.
   */
  template<class V1, class V2, Location L>
  void cumulativeOffspring(Random& rng, const V1 lws, V2 Os, const int P,
      ScanResamplerPrecompute<L>& pre)
          throw (ParticleFilterDegeneratedException);

  /**
   * @copydoc MultinomialResampler::ancestors
   */
  template<class V1, class V2, Location L>
  void ancestors(Random& rng, const V1 lws, V2 as,
      ScanResamplerPrecompute<L>& pre)
          throw (ParticleFilterDegeneratedException);

  /**
   * @copydoc MultinomialResampler::ancestorsPermute
   */
  template<class V1, class V2, Location L>
  void ancestorsPermute(Random& rng, const V1 lws, V2 as,
      ScanResamplerPrecompute<L>& pre)
          throw (ParticleFilterDegeneratedException);
//@}
};

/**
 * @internal
 */
template<Location L>
struct precompute_type<StratifiedResampler,L> {
  typedef ScanResamplerPrecompute<L> type;
};
}

#include "../host/resampler/StratifiedResamplerHost.hpp"
#ifdef __CUDACC__
#include "../cuda/resampler/StratifiedResamplerGPU.cuh"
#endif

#include "../primitive/vector_primitive.hpp"
#include "../primitive/matrix_primitive.hpp"
#include "../misc/location.hpp"
#include "../math/temp_vector.hpp"
#include "../math/sim_temp_vector.hpp"

template<class V1, class V2, bi::Location L>
void bi::StratifiedResampler::cumulativeOffspring(Random& rng, const V1 lws,
    V2 Os, const int n, ScanResamplerPrecompute<L>& pre)
        throw (ParticleFilterDegeneratedException) {
  /* pre-condition */
  BI_ASSERT(lws.size() == Os.size());

  if (pre.W > 0) {
#ifdef __CUDACC__
    typedef typename boost::mpl::if_c<V1::on_device,StratifiedResamplerGPU,
        StratifiedResamplerHost>::type impl;
#else
    typedef StratifiedResamplerHost impl;
#endif
    impl::op(rng, pre.Ws, Os, n);

#ifndef NDEBUG
    int m = *(Os.end() - 1);
    BI_ASSERT_MSG(m == n,
        "Stratified resampler gives " << m << " offspring, should give " << n);
#endif
  } else {
    throw ParticleFilterDegeneratedException();
  }
}

template<class V1, class V2, bi::Location L>
void bi::StratifiedResampler::ancestors(Random& rng, const V1 lws, V2 as,
    ScanResamplerPrecompute<L>& pre)
        throw (ParticleFilterDegeneratedException) {
  typename sim_temp_vector<V2>::type Os(lws.size());

  cumulativeOffspring(rng, lws, Os, as.size(), pre);
  cumulativeOffspringToAncestors(Os, as);
}

template<class V1, class V2, bi::Location L>
void bi::StratifiedResampler::ancestorsPermute(Random& rng, const V1 lws,
    V2 as, ScanResamplerPrecompute<L>& pre)
        throw (ParticleFilterDegeneratedException) {
  typename sim_temp_vector<V2>::type Os(lws.size());

  cumulativeOffspring(rng, lws, Os, as.size(), pre);
  cumulativeOffspringToAncestorsPermute(Os, as);
}

#endif
