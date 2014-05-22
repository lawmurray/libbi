LibBi VERSION
=============

v1.1.0
------

New features:

* Added bridge particle filter (from Del Moral & Murray, 2014).
* Added built-in variables `t_now`, `t_last_input` and `t_next_obs`.
* Added `transpose` and `cholesky` actions.
* Added `log` argument to `pdf` action.
* Added matrix-matrix multiply.
* Added range syntax, e.g. `x[0:4]`.
* Added checks for `*.sh`, `data/*.nc` and `oct/*.m` files to
  `libbi package --validate` checks.

Changes:

* Removed ill-defined `uninformative` action.
* Action syntax made stricter: an action that returns a scalar cannot be
  applied to a vector on the left.

Performance:

* Minor performance improvements in GPU random number generation and
  resampling.
* Minor I/O performance improvements.

Fixes:

* Restored NetCDF 4.1 support.
* Fixed initialisation of parameters from init file when
  `--with-transform-initial-to-param` used.
* Fixed `C_` and `U2_` variables in Kalman filter output files.
* Fixed reporting of log-likelihood in PMCMC output when particle filter
  degenerates.
* Fixed build error when model name does not begin with an uppercase letter.
* Fixed runtime error when empty `--output-file` given.
* Fixed race condition in locking the build directory under some
  circumstances.
* Fixed unnecessary recompiles triggered by new hash implementation in newer
  versions of Perl.

v1.0.2
------

* Removed dependency on NetCDF C++ interface, the C interface is now used
  directly.
* Added `'extended'` boundary condition for dimensions.
* Added `--enable-openmp`/`--disable-openmp` command-line options.
* Added `--enable-gpu-cache/--disable-gpu-cach`e command-line options for better
  control of GPU memory usage.
* Added `--adapter-ess-rel`  command-line option to avoid adaptation of proposal
  in SMC$^2$ when ESS too low.
* Several bug and compatibility fixes.

v1.0.1
------

* Added additional material to manual, including new section with guidance on
  tuning the proposal distribution and number of particles when using PMMH.
* Fixed sampling of joint distribution (`--target joint` now implies
  `--with-param-to-state`, just as `--target prior` and `--target prediction`
  do).
* Fixed reordering of actions and blocks when the same variable appears on
  the left more than once.
* Fixed bug in GPU implementation of multinomial resampler.
* Added `--dry-parse` option to remove parsing overhead when binaries have
  already been compiled.

v1.0.0
------

* First public release.