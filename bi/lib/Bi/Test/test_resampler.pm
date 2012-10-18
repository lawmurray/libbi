=head1 NAME

test_resampler - frontend to test client programs.

=head1 SYNOPSIS

    bi test_resampler ...

=head1 INHERITS

L<Bi::Client>

=cut
our @CLIENT_OPTIONS = ();

=head1 METHODS

=over 4

=cut

package Bi::Test::test_resampler;

use base 'Bi::Client';
use warnings;
use strict;

sub init {
    my $self = shift;

    $self->{_binary} = 'test_resampler';
    push(@{$self->{_params}}, @CLIENT_OPTIONS);
}

sub needs_model {
    return 0;
}

1;

=back

=head1 AUTHOR

Lawrence Murray <lawrence.murray@csiro.au>

=head1 VERSION

$Rev$ $Date$
