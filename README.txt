
sudo aptitude update
sudo aptitude safe-upgrade
sudo aptitude install build-essential
sudo aptitude install kdebase-dev

Starting now with 0.10 code
./configure
make   ( got sporadic compiler errors ; repeated make until successful )
sudo make install
/usr/local/kde/bin/knowit   ( runs knowit )


On to the 0.11 code:

# uncomment the two "backports" lines in /etc/apt/sources.list and install git
sudo aptitude install git-core

# use git to fetch patch and knowit code with patch applied :
git clone git://github.com/dc25/knowit_0.11.git  

cd knowit_0.11/knowit
sudo aptitude install automake
make -f Makefile.cvs
./configure
make
sudo make install
# knowit should now be installed in /usr/bin/knowit and ready to run.


