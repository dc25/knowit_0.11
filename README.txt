

sudo aptitude update
sudo aptitude safe-upgrade

sudo aptitude install build-essential
sudo aptitude install kdebase-dev
sudo aptitude install automake

# uncomment the two "backports" lines in /etc/apt/sources.list 
sudo aptitude install git-core

# use git to fetch patch and knowit code with patch applied :
git clone git://github.com/dc25/knowit_0.11.git  

cd knowit_0.11/knowit
make -f Makefile.cvs
./configure
make
sudo make install
# knowit should now be installed in /usr/bin/knowit and ready to run.


