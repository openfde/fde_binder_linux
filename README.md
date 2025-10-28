sudo apt-get install dh-make
sudo apt-get install dkms
sudo cp -a fde_binder_linux /usr/src/fde_binder_linux_1.0
3.1 sudo dkms add -m fde_binder_linux -v 1.0
sudo dkms build -m fde_binder_linux -v 1.0
sudo dkms mkdeb -m fde_binder_linux -v 1.0
