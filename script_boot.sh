echo 22 > /sys/class/gpio/export
echo 23 > /sys/class/gpio/export
sleep 1
sudo insmod /home/pi/IVT33_Bolshakov/Course/driver.ko
sleep 1
sudo ./IVT33_Bolshakov/Course/combiner