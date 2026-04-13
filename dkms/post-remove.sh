sudo sed -i "/binder_linux/d" /etc/modules 1>/dev/null 2>&1
sudo lsmod |grep binder_linux
if [ $? = 0 ];then
	sudo rmmod binder_linux
fi
