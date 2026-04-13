sudo depmod
sudo modprobe binder_linux
sudo sed -i "/binder_linux/d" /etc/modules 1>/dev/null 2>&1
sudo sed -i '$a \binder_linux' /etc/modules
