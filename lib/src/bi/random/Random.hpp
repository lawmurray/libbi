/**
 * @file
 *
 * @author Lawrence Murray <lawrence.murray@csiro.au>
 * $Rev$
 * $Date$
 *
 * Imported from dysii 1.4.0, originally indii/ml/aux/Random.hpp
 */
#ifndef BI_RANDOM_RANDOM_HPP
#define BI_RANDOM_RANDOM_HPP

#include "../cuda/cuda.hpp"
#include "../cuda/random/curand.hpp"
#include "../misc/assert.hpp"

#include "boost/random/mersenne_twister.hpp"

#include <vector>

namespace bi {
/**
 * Pseudorandom number generator.
 *
 * @ingroup math_rng
 *
 * Uses the Mersenne Twister algorithm for generating pseudorandom variates,
 * as implemented by Boost.
 *
 * The functions of this class are thread-safe (using OpenMP), utilising one
 * random number generator per thread. Each random number generator is
 * seeded differently from the one seed given. Experimental results suggest
 * that this is sufficient with the Mersenne Twister given its large period.
 * Parallel generation of random numbers is used in the plural methods,
 * with static scheduling to ensure reproducibility with the same seed.
 *
 * @section Random_references References
 *
 * @anchor Matsumoto1998 Matsumoto, M. and Nishimura,
 * T. Mersenne Twister: A 623-dimensionally equidistributed
 * uniform pseudorandom number generator. <i>ACM Transactions on
 * Modeling and Computer Simulation</i>, <b>1998</b>, 8, 3-30.
 */
class Random {
public:
  /**
   * Constructor. Initialise but do not seed random number generator.
   */
  Random();

  /**
   * Constructor. Initialise and seed random number generator.
   *
   * @param seed Seed value.
   */
  Random(const unsigned seed);

  /**
   * Destructor.
   */
  ~Random();

  /**
   * Seed random number generator.
   *
   * @param seed Seed value.
   */
  void seed(const unsigned seed);

  /**
   * Reset random number generator with last used seed.
   */
  void reset();

  /**
   * Generate a boolean value from a Bernoulli distribution.
   *
   * @param p Probability of true, between 0 and 1 inclusive.
   *
   * @return The random boolean value, 1 for true, 0 for false.
   */
  template<class T1>
  unsigned bernoulli(const T1 p = 0.5);

  /**
   * Generate a random number from a multinomial distribution with given
   * probabilities.
   *
   * @tparam V1 Vector type.
   *
   * @param ps Probabilities. Need not be normalised to 1.
   *
   * @return Random index between @c 0 and <tt>ps.size() - 1</tt>, selected
   * according to the non-normalised probabilities given in @c ps.
   */
  template<class V1>
  typename V1::difference_type multinomial(const V1& ps);

  /**
   * Generate a random integer from a uniform distribution over a
   * given interval.
   *
   * @tparam T1 Scalar type.
   *
   * @param lower Lower bound on the interval.
   * @param upper Upper bound on the interval.
   *
   * @return The random integer, >= @p lower and <= @p upper.
   */
  template<class T1>
  T1 uniformInt(const T1 lower = 0, const T1 upper = 1);

  /**
   * Generate a random number from a uniform distribution over a
   * given interval.
   *
   * @tparam T1 Scalar type.
   *
   * @param lower Lower bound on the interval.
   * @param upper Upper bound on the interval.
   *
   * @return The random number.
   */
  template<class T1>
  T1 uniform(const T1 lower = 0.0, const T1 upper = 1.0);

  /**
   * Generate a random number from a Gaussian distribution with a
   * given mean and standard deviation.
   *
   * @tparam T1 Scalar type.
   *
   * @param mu Mean of the distribution.
   * @param sigma Standard deviation of the distribution.
   *
   * @return The random number. If the standard deviation is zero, returns
   * the mean.
   */
  template<class T1>
  T1 gaussian(const T1 mu = 0.0, const T1 sigma = 1.0);

  /**
   * Fill vector with random numbers from a uniform distribution over a
   * given interval.
   *
   * @tparam V1 Vector type.
   *
   * @param lower Lower bound on the interval.
   * @param upper Upper bound on the interval.
   * @param[out] x Vector.
   */
  template<class V1>
  void uniforms(V1 x, const typename V1::value_type lower = 0.0,
      const typename V1::value_type upper = 1.0);

  /**
   * Fill vector with random numbers from a Gaussian distribution with a
   * given mean and standard deviation.
   *
   * @tparam V1 Vector type.
   *
   * @param mu Mean of the distribution.
   * @param sigma Standard deviation of the distribution.
   * @param[out] x Vector.
   */
  template<class V1>
  void gaussians(V1 x, const typename V1::value_type mu = 0.0,
      const typename V1::value_type sigma = 1.0);

private:
  /**
   * Type of random number generator on host.
   */
  typedef boost::mt19937 rng_t;

  /**
   * Random number generators on host.
   */
  std::vector<rng_t> rng;

  /**
   * Random number generator on device.
   */
  #ifndef USE_CPU
  curandGenerator_t devRng;
  #endif

  /**
   * Original seed.
   */
  unsigned originalSeed;
};

}

#include "../misc/omp.hpp"
#include "../math/temp_vector.hpp"

#include "boost/random/uniform_int.hpp"
#include "boost/random/uniform_real.hpp"
#include "boost/random/normal_distribution.hpp"
#include "boost/random/bernoulli_distribution.hpp"
#include "boost/random/variate_generator.hpp"

template<class T1>
inline unsigned bi::Random::bernoulli(const T1 p) {
  /* pre-condition */
  assert (p >= 0.0 && p <= 1.0);

  typedef boost::bernoulli_distribution<unsigned> dist_t;

  dist_t dist(p);
  boost::variate_generator<rng_t&, dist_t> gen(rng[bi_omp_tid], dist);

  return gen();
}

template<class V1>
inline typename V1::difference_type bi::Random::multinomial(const V1& ps) {
  /* pre-condition */
  assert (ps.size() > 0);

  typedef boost::uniform_real<typename V1::value_type> dist_t;

  BOOST_AUTO(Ps, host_temp_vector<typename V1::value_type>(ps.size()));
  exclusive_scan_sum_exp(ps.begin(), ps.end(), Ps->begin());

  dist_t dist(0, *(Ps->end() - 1));
  boost::variate_generator<rng_t&, dist_t> gen(rng[bi_omp_tid], dist);

  int p = std::lower_bound(Ps->begin(), Ps->end(), gen()) - Ps->begin();
  delete Ps;

  return p;
}

template<class T1>
inline T1 bi::Random::uniformInt(const T1 lower, const T1 upper) {
  /* pre-condition */
  assert (upper >= lower);

  typedef boost::uniform_int<T1> dist_t;

  dist_t dist(lower, upper);
  boost::variate_generator<rng_t&, dist_t> gen(rng[bi_omp_tid], dist);

  return gen();
}

template<class T1>
inline T1 bi::Random::uniform(const T1 lower, const T1 upper) {
  /* pre-condition */
  assert (upper >= lower);

  typedef boost::uniform_real<T1> dist_t;

  dist_t dist(lower, upper);
  boost::variate_generator<rng_t&, dist_t> gen(rng[bi_omp_tid], dist);

  return gen();
}

template<class T1>
inline T1 bi::Random::gaussian(const T1 mu, const T1 sigma) {
  /* pre-condition */
  assert (sigma >= 0.0);

  typedef boost::normal_distribution<T1> dist_t;

  dist_t dist(mu, sigma);
  boost::variate_generator<rng_t&, dist_t> gen(rng[bi_omp_tid], dist);

  return gen();
}

template<class V1>
void bi::Random::uniforms(V1 x, const typename V1::value_type lower,
    const typename V1::value_type  upper) {
  /* pre-condition */
  assert (upper >= lower);

  typedef typename V1::value_type T1;
  typedef boost::uniform_real<T1> dist_t;

  if (V1::on_device) {
    assert (lower == 0.0 && upper == 1.0);
    #ifndef USE_CPU
    CURAND_CHECKED_CALL(curand_generate_uniform<T1>::func(devRng, x.buf(),
        x.size()));
    #else
    BI_ASSERT(false, "GPU random number generation not enabled");
    #endif
  } else {
    #pragma omp parallel
    {
      int j;
      dist_t dist(lower, upper);
      boost::variate_generator<rng_t&,dist_t> gen(rng[bi_omp_tid], dist);

      #pragma omp for schedule(static)
      for (j = 0; j < x.size(); ++j) {
        x(j) = gen();
      }
    }
  }
}

template<class V1>
void bi::Random::gaussians(V1 x, const typename V1::value_type mu,
    const typename V1::value_type sigma) {
  /* pre-condition */
  assert (sigma >= 0.0);

  typedef typename V1::value_type T1;
  typedef boost::normal_distribution<T1> dist_t;

  if (V1::on_device) {
    #ifndef USE_CPU
    CURAND_CHECKED_CALL(curand_generate_normal<T1>::func(devRng, x.buf(),
        x.size(), static_cast<T1>(0.0), static_cast<T1>(1.0)));
    #else
    BI_ASSERT(false, "GPU random number generation not enabled");
    #endif
  } else {
    #pragma omp parallel
    {
      int j;
      dist_t dist(mu, sigma);
      boost::variate_generator<rng_t&,dist_t> gen(rng[bi_omp_tid], dist);

      #pragma omp for schedule(static)
      for (j = 0; j < x.size(); ++j) {
        x(j) = gen();
      }
    }
  }
}

#endif