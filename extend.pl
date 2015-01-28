#!/usr/bin/perl

open(SIG, $ARGV[0]) || die "open $ARGV[0]: $!";

$n = sysread(SIG, $buf, 1000000);

if($n > 524288){
    print STDERR "ERROR: kernel.img too large: $n bytes (max 521KB)\n";
    exit 1;
}

print STDERR "OK: kernel.img is $n bytes (max 512KB)\n";

$buf .= "\0" x (524288-$n);

open(SIG, ">$ARGV[0]") || die "open >$ARGV[0]: $!";
print SIG $buf;
close SIG;
