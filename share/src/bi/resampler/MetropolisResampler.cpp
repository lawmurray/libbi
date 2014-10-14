/**
 * @file
 *
 * @author Lawrence Murray <lawrence.murray@csiro.au>
 * $Rev$
 * $Date$
 */
#include "MetropolisResampler.hpp"

bi::MetropolisResampler::MetropolisResampler(const int B, const double essRel) :
    Resampler(essRel), B(B) {
  //
}

void bi::MetropolisResampler::setSteps(const int B) {
  this->B = B;
}
