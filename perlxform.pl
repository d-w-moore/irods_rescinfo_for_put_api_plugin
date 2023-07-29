#!/usr/bin/env perl
# -- Transform file names
#
# -- Bash example usage --
# newname="get_rescinfo_for_put"
# x=$(./perlxform.pl  $newname < <(yes) )
# shopt -s globstar 
# rm **/*.orig
# perl -i.orig -pe "s/project_template/$newname/g" $x
#
use POSIX;

-d ".git" or die "please run from repo root.\n";

# Get names of files registered in Git for current commit.
#
(our $pid = open (FILE, "git ls-files|")) or die "error retrieving repo filenames.\n";
chomp(@fnames = <FILE>);
waitpid $pid,0;

$orig = "project_template";

(our $newname = "$ARGV[0]")
    or die "need a non-zero-length substitute string for rename.\n";

@newnames = ();

# Transform and rename.
#
for (@fnames) {
    my $oldname=$_;
    my $rename=0;
    if (s/$orig/$newname/) {
        print STDERR $_, " -> " if isatty *STDIN;
        my $ans=<STDIN>;
        my $cmd =  "git mv '$oldname' '$_'";
        do {(0 == system $cmd) and $rename=1;} if $ans=~/^\s*[yY]/;
    }
    push @newnames, ($rename?$_:$oldname);
}
print join"\n",(grep !/perlxform|\.git/,@newnames);
print"\n";
