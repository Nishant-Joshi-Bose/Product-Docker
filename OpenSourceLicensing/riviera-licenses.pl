#!/usr/bin/perl -w
################################################################################
#
# riviera-licenses.pl
#
# (c) Copyright 2018, Bose Corporation
# Reproduction of this code or any fragment thereof is prohibited and
# and protected by copyright law unless explicit written authorization
# is obtained from Bose Corporation
#
################################################################################
#
# This script generates an HTML table with license information about all the
# Open Source packages in a Riviera product.  Most of the license data is
# extracted from the HSP build output, but additional Open Source packages are
# added manually to this script so they are also included in the table.
#
################################################################################
# Import packages
################################################################################
use Cwd 'abs_path';
use File::Basename;
use File::Copy;
use File::Path;
use File::Slurp;
use Getopt::Long;
use Digest::MD5::File qw(file_md5_hex);
use List::MoreUtils qw(uniq);
use HTML::Entities;
use strict;


################################################################################
# Declare and/or initialize variables
################################################################################
my (%options, %packages, %licenses, %copyrights);
my ($status, $package, $version, $recipe, $license);
my $basename = basename($0);
# This following hashes of arrays contains a list of projects that are not in the
# HSP that need to be added to the Open Source disclosure list.  The projects
# directory contains the license files and its contents get copied into the same
# place that the HSP licenses are extracted to and are all processed together.
my %projects = (
    # OpenSource-XXX projects on github.com:BoseCorp not in HSP
    gssdp               => ["gssdp",            "1.0.2",    "GPLv2+"],
    gupnp               => ["gupnp",            "1.0.2",    "GPLv2+"],
    jsoncpp             => ["jsoncpp",          "1.8.3",    "GPLv2+"],
    libtool             => ["libtool",          "2.4.2",    "GPLv2+"],
    libwebsockets       => ["libwebsockets",    "2.4.1",    "GPLv2+"],
    openavnu            => ["OpenAvnu",         "v1.0",     "Intel"],  # Only using daemons/gptp per Jim Uban
    protobuf            => ["protobuf",         "3.6.1",    "Google"],
    unittestcpp         => ["unittest-cpp",     "1.0.0",    "MIT/X Consortium"],
    unzip               => ["unzip",            "6.0.0",    "Info-ZIP"],
    zip                 => ["zip",              "3.0.0",    "Info-ZIP"],
    # Dinghy licenses extracted from dinghy-licenses.txt (original name: LICENSES.md)
    bitstreamvera       => ["bitstream-vera",   "1.10",     "Bitstream"],
    cairo               => ["cairo",            "1.14.8",   "GPLv2.1 | MPL"],
    pixman              => ["pixman",           "0.34.0",   "MIT"],
    glibnetworking      => ["glib-networking",  "2.50.0",   "GPLv2"],
    libtasn1            => ["libtasn1",         "4.12",     "GPLv3 | LGPLv2.1"],
    libunistring        => ["libunistring",     "0.9.7",    "LGPLv3"],
    libepoxy            => ["libepoxy",         "1.4.1",    "MIT"],
    xutilmacros         => ["xutil_util-macro", "1.19.1",   "RedHat/Oracle"],
    libgcrypt           => ["libgcrypt",        "1.7.9",    "LGPLv2.1"],
    libgpgerror         => ["libgpg-error",     "1.27",     "GPLv2 & LGPLv2.1"],
    libsoup             => ["libsoup",          "2.56.1",   "LGPLv2"],
    webp                => ["webp",             "0.5.2",    "Google"],
    dinghy              => ["dinghy",           "1.0.0",    "Igalia"],
    wpewebkit           => ["wpewebkit",        "0.20.0",   "Igalia & Apple & LGPLv2.1"],
    wpebackend          => ["wpebackend",       "20180305", "Igalia"],
    wpebackendbose      => ["wpebackend-bose",  "20180305", "Igalia"],
    # Monaco licenses extracted from monaco-licenses.txt (output of 'npm run license-aggregation')
    asap                => ["asap",             "2.0.6",    "MIT"],
    corejs              => ["core-js",          "1.2.7",    "MIT"],
    debug               => ["debug",            "2.6.9",    "MIT"],
    decodeuricomponent  => ["decode-uri-component", "0.2.0","MIT"],
    dotprop             => ["dot-prop",         "4.2.0",    "MIT"],
    encoding            => ["encoding",         "0.1.12",   "MIT"],
    fbjs                => ["fbjs",             "0.8.17",   "MIT"],
    iconvlite           => ["iconv-lite",       "0.4.24",   "MIT"],
    isobj               => ["is-obj",           "1.0.1",    "MIT"],
    isomorphicfetch     => ["isomorphic-fetch", "2.2.1",    "MIT"],
    isstream            => ["is-stream",        "1.1.0",    "MIT"],
    istypedarray        => ["is-typedarray",    "1.0.0",    "MIT"],
    jstokens            => ["js-tokens",        "4.0.0",    "MIT"],
    looseenvify         => ["loose-envify",     "1.4.0",    "MIT"],
    ms                  => ["ms",               "2.0.0",    "MIT"],
    nan                 => ["nan",              "2.11.1",   "MIT"],
    nodefetch           => ["node-fetch",       "1.7.3",    "MIT"],
    objectassign        => ["object-assign",    "4.1.1",    "MIT"],
    promise             => ["promise",          "7.3.1",    "MIT"],
    proptypes           => ["prop-types",       "15.6.1",   "MIT"],
    querystring         => ["query-string",     "5.1.0",    "MIT"],
    react               => ["react",            "16.3.2",   "MIT"],
    reactdom            => ["react-dom",        "16.3.2",   "MIT"],
    rxjs                => ["rxjs",             "5.5.10",   "Apache"],
    saferbuffer         => ["safer-buffer",     "2.1.2",    "MIT"],
    sax                 => ["sax",              "1.2.4",    "ISC"],
    schedule            => ["schedule",         "0.4.0",    "MIT"],
    setimmediate        => ["setimmediate",     "1.0.5",    "MIT"],
    stricturiencode     => ["strict-uri-encode","1.1.0",    "MIT"],
    symbolobservable    => ["symbol-observable","1.0.1",    "MIT"],
    typedarraytobuffer  => ["typedarray-to-buffer", "3.1.5","MIT"],
    typescript          => ["typescript",       "3.1.6",    "Apache"],
    uaparserjs          => ["ua-parser-js",     "0.7.19",   "MIT"],
    websocket           => ["websocket",        "1.0.28",   "Apache"],
    whatwgfetch         => ["whatwg-fetch",     "3.0.0",    "MIT"],
    xml2js              => ["xml2js",           "0.4.19",   "MIT"],
    xmlbuilder          => ["xmlbuilder",       "9.0.7",    "MIT"],
    yaeti               => ["yaeti",            "0.0.6",    "MIT"],
    # Other Components (from https://wiki.bose.com/display/WSSW/Software+License)
    freertos            => ["FreeRTOS",         "10.0.1",   "MIT"],
    id3lib              => ["id3lib",           "3.8.3",    "GPL"],
    jsmn                => ["jsmn",             "n/a",      "MIT"],
    go                  => ["go",               "1.4.3",    "BSD"],
    errors              => ["go-errors",        "n/a",      "BSD"],
    jwx                 => ["go-jwx",           "n/a",      "MIT"],
    pdebug              => ["go-pdebug",        "n/a",      "MIT"],
);

my %sbprojects = (
    # Soundbar specific packages (from https://wiki.bose.com/display/WSSW/Software+License)
    adv7533driver       => ["adv7533-driver*",  "n/a",      "GPLv2"],
    sii9437driver       => ["sii9437-driver*",  "n/a",      "GPLv2"],
    microecc            => ["micro-ecc*",       "1.0.0",    "BSD"],
    heatshrink          => ["heatshrink*",      "0.4.1",    "ISC"],
);


################################################################################
# Declare subroutines
################################################################################
sub getRecipeFiles($);
sub printHeader();
sub toHTML($);
sub usage($$);


################################################################################
# Process and validate command line options
################################################################################
$status = GetOptions(\%options,
                        'soundbar', # Include packages exclusive to Soundbar 500/700
                        'help',     # Print usage summary and exit
                   );

usage(0, "") if (defined($options{help}));
usage(1, "") unless($status);
if ($options{soundbar})
{
    # Include Soundbar specific packages
    $projects{$_} = $sbprojects{$_} for (keys %sbprojects);
}


################################################################################
# Main program
################################################################################
my $mydir = abs_path(dirname($0));
my $file = shift || die usage(1, "Please specify a path to the Riviera-HSP licenses file!");
my $licdir = shift || "$mydir/licenses";
chdir($mydir);
my $preamble = read_file('eula.html');
die("$basename: The file $file does not exist or is not readable!\n") unless -r $file;
my $tmpdir = "/tmp/licenses-$$";
mkdir($tmpdir);
system("tar zxf $file -C $tmpdir");
system("cp -r projects/* $tmpdir");
for my $manifest (glob($tmpdir . "/machine-*-image-apq8017-*/license.manifest"))
{
    open(MANIFEST, $manifest) || die "$basename: Cannot open $manifest for reading ($!)\n";
    while(<MANIFEST>)
    {
        $package = $1 if (/PACKAGE NAME: (.*)/);
        $version = $1 if (/PACKAGE VERSION: (.*)/);
        $recipe  = $1 if (/RECIPE NAME: (.*)/);
        $license = $1 if (/LICENSE: (.*)/);
        if  (/LICENSE:/)
        {
            next if ($license =~ m/Proprietary/i);  # Skip any packages that have proprietary licenses
            my $licenseFiles = getRecipeFiles($tmpdir . "/$recipe");
            my $entry = sprintf "%s\t%s\t%s\t%s\t%s", "$package", "$version", "$license", "$licenseFiles", join("||", uniq(@{$copyrights{$recipe}})) || "";
            $packages{$entry}++;
        }
    }
    close(MANIFEST);
}
for (keys %projects)
{
    $package = @{$projects{$_}}[0];
    $version = @{$projects{$_}}[1];
    $recipe  = $_;
    $license = @{$projects{$_}}[2];

    my $licenseFiles = getRecipeFiles($tmpdir . "/$recipe");
    my $entry = sprintf "%s\t%s\t%s\t%s\t%s", "$package", "$version", "$license", "$licenseFiles", join("||", uniq(@{$copyrights{$recipe}})) || "";
    $packages{$entry}++;
}

rmtree($licdir);
mkpath($licdir);
open(LIST, ">$licdir/licenses.html") || die "$basename: Can't open $licdir/licenses.html for writing ($!)\n";
printf LIST "%s", printHeader();
printf LIST "%s", toHTML($_) for (sort keys %packages);
printf LIST "  </tr>\n</table>\n</html>\n";
close(LIST);

for $license (keys %licenses)
{
    my $md5 = file_md5_hex($licenses{$license});
    copy($licenses{$license}, "$licdir/$md5.txt") || die "Can't copy $licenses{$license} to $licdir/$md5.txt ($!)\n";
}
rmtree("$tmpdir");
exit(0);


################################################################################
# End of main program, all subroutine definitions follow
################################################################################
sub getRecipeFiles($)
{
    my $recipedir = shift;
    my $files = "";
    foreach my $file (glob("$recipedir/*"))
    {
        # Skip files that are not license files
        next if ($file =~ m/configure|Makefile/i);

        # Extract Copyright notices from all files (including source files)
        open(COPYRIGHT, $file) || next;
        while(<COPYRIGHT>)
        {
            if (/(Copyright[^\*\\]*)|\(c\)/i)
            {
                if (/\d\d\d\d/)
                {
                    # Clean up copyright notice string to eliminate duplicates
                    chomp;
                    s/^.*Copyright/Copyright/i; # Remove leading cruft
                    s/\*\///;                   # Remove C style comments
                    s/\/\*//;                   # Remove C style comments
                    s/\s*\*\s+//;               # Remove C style comments
                    s/\*\s+$//;                 # Remove C style comments
                    s/\t/  /g;                  # Convert tab to spaces
                    s/^\s+//;                   # Remove leading spaces
                    s/\s+$//;                   # Remove trailing spaces
                    s/\s+\*$//;                 # Remove dangling asteriks
                    s/,$//;                     # Remove trailing comma
                    push(@{$copyrights{basename($recipedir)}}, encode_entities($_));
                }
            }
        }
        close(COPYRIGHT);

        # Create a list of license files with embedded hyperlinks excluding source files
        unless ($file =~ m/\.c|\.h/i)
        {
            my $md5 = file_md5_hex($file);
            $licenses{$md5} = $file unless defined ($licenses{$md5});
            $files .=  sprintf("<a href=\"%s\">%s<a>, ", "$md5.txt", basename($file));
        }
    }
    $files =~ s/, $//;
    return($files);
}

sub printHeader()
{
    my $header = <<__EOF__;
<!DOCTYPE html>
<html lang="en">
<meta charset="utf-8">
<title>EULA</title>
<style type="text/css">
.tg  {border-collapse:collapse;border-spacing:1;}
.tg td{font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;}
.tg th{font-family:Arial, sans-serif;font-size:14px;font-weight:normal;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;}
.tg .tg-0lax{text-align:left;vertical-align:top}
ol { counter-reset: item; padding:10px ; margin: 0; padding-left: 10px; }
ol > li > ol { padding-left: 10px; padding: 8px; }
li { display: block; padding-left: 10px; padding-right: 10px; padding: 8px; }
li:before { content: counters(item, ".") " "; counter-increment: item }
</style>
$preamble
<table class="tg">
  <tr>
    <th class="tg-0lax"><span style="font-weight:bold">Package Name</span></th>
    <th class="tg-0lax"><span style="font-weight:bold">Package Version</span></th>
    <th class="tg-0lax"><span style="font-weight:bold">License(s)</span></th>
    <th class="tg-0lax"><span style="font-weight:bold">License File(s) </span></th>
    <th class="tg-0lax"><span style="font-weight:bold">Copyright (if available)</span></th>
  </tr>
__EOF__

    return($header);
}

sub toHTML($)
{
    my $entry = shift;
    $entry =~ s/\|\|/<br>/g;
    $entry =~ s/\t/<\/td>\n    <td class=\"tg-0lax\">/g;
    return(sprintf "  <tr>\n    <td class=\"tg-0lax\">%s<\/td>\n  </tr>\n", $entry);
}

sub usage($$)
{
    my ($status, $message) = @_;
    print "\n$message\n" if($message ne "");
    print <<__EOF__;

This script generates a CSV file with information about all the Open Source
packages in a Riviera HSP build.

    Usage: $basename [options] /path/to/HSP-license-file

    -soundbar        Include packages exclusive to Soundbar 500/700
    -help            Print this usage summary

__EOF__

exit($status);
}
