1 sudo apt-get install dh-make<br>
2 sudo apt-get install dkms<br>
3 sudo cp -a fde_binder_linux /usr/src/fde_binder_linux6.6-1.0<br>
&emsp;&emsp;3.1 sudo dkms add -m fde_binder_linux6.6 -v 1.0<br>
4 sudo dkms build -m fde_binder_linux6.6 -v 1.0<br>
5 sudo dkms mkdeb -m fde_binder_linux6.6 -v 1.0<br>
