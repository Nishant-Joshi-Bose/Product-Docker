#!/usr/bin/perl
if (@ARGV < 2) {
    print "Usage: updatepsoc.pl cyacdfile lpmserialdev\n";
    exit 0;
}

my ($cyacd, $serialdev) = @ARGV;
open(CFD, "< $cyacd") || die "Can't open $cyacd, $!\n";
open(SFD, "> $serialdev") || die "Can't open $serialdev, $!\n";
# Start update
print SFD "lb us\r\n";
print "lb us\r\n";
#select(undef,undef,undef,0.250);
sleep(1);
# Send the header
my $header = <CFD>;
print SFD "lb uh,$header\r";
print "lb uh,$header\r";
sleep(1);
#select(undef,undef,undef,0.250);
# Send the data
while (my $data = <CFD>) {
    print SFD "lb ud,$data\r";
    print "lb ud,$data\r";
    select(undef,undef,undef,0.100);
#    sleep(1);
}
# End update
#select(undef,undef,undef,0.250);
sleep(1);
print SFD "lb ue\r\n";
print "lb ue\r\n";
close(SFD);
close(CFD);
