sudo ./encoder/encoder -q > data_path &
sudo cat data_out | xargs -I {} sudo ./display_text/lcd "{}" &
sudo ./source &
ps -a | grep source | xargs -I {} sudo kill -9 "{}"
gcc combiner.c -o combiner -lpthread


sudo nano /home/pi/.bashrc