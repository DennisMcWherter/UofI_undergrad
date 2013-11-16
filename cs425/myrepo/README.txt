Build Instructions

cd libs
Extract the appropriate poco lib (i.e. .tar.gz for *nix, .zip for windows)
cd into the extracted directory
Run: ./configure --prefix=`pwd` --no-samples --no-tests; make; make install
Return to the root MyRepo directory
Run: make

