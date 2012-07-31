=head1 NAME

ode - block for control of ordinary differential equation actions.

=head1 SYNOPSIS

    do ode(alg = '...', h = 1.0, atoler = 1.0e-3, rtoler = 1.0e-3) {
      X1 <- ode(...)
      X2 <- ode(...)
      ...
    }

    do ode(I<alg>, I<h>, I<atoler>, I<rtoler>) {
      ...
    }

=head1 DESCRIPTION

An C<ode> block is used to group multiple L<ode> actions and configure the
numerical integrator used to apply them.

An ode block may only be used within a L<transition> block, may not contain
nested blocks, and may only contain L<ode> actions.

=cut

package Bi::Block::ode;

use base 'Bi::Model::Block';
use warnings;
use strict;

=head1 PARAMETERS

=over 4

=item * C<alg> (position 0, default C<'rk43'>)

The numerical integration algorithm to be used. Valid values are:

=over 8

=item * C<'rk4'>

The classic order 4 Runge-Kutta with fixed step size.

=item * C<'dopri5'>

An order 5(4) Dormand-Prince with adaptive step size control.

=item * C<'rk43'>

An order 4(3) low-storage Runge-Kutta with adaptive step size control.

=back

=item * C<h> (position 1, default 1.0)

For a fixed step size, the step size to use. For an adaptive step size, the
suggested initial step size to use.

=item * C<atoler> (position 2, default 1.0e-3)

The absolute error tolerance for adaptive step size control.

=item * C<rtoler> (position 3, default 1.0e-3)

The relative error tolerance for adaptive step size control.

=back

=cut
our $BLOCK_ARGS = [
  {
    name => 'alg',
    positional => 1,
    default => "'rk43'"
  },
  {
    name => 'h',
    positional => 1,
    default => 1.0
  },
  {
    name => 'atoler',
    positional => 1,
    default => 0.001
  },
  {
    name => 'rtoler',
    positional => 1,
    default => 0.001
  }
];

sub validate {
    my $self = shift;
    
    $self->process_args($BLOCK_ARGS);
    
    my $alg = $self->get_named_arg('alg')->eval_const;
    if ($alg ne 'rk4' && $alg ne 'dopri5' && $alg ne 'rk43') {
        die("unrecognised value '$alg' for argument 'alg' of block 'ode'\n");
    }
}

1;

=head1 AUTHOR

Lawrence Murray <lawrence.murray@csiro.au>

=head1 VERSION

$Rev$ $Date$