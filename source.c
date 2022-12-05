#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
//#include <vector> 
#include <string.h>

int main()
{
    int counter = 0;
    struct timespec mt;
    int hours = 0;
    int minutes = 0;
    int seconds = 0;
    char value[100];
    char out[100];
    char *myFifo = "data_path";
    char *myFifo2 = "data_out";
    mkfifo(myFifo, 0666);
    mkfifo(myFifo2, 0666);
    int fd = open(myFifo, O_RDONLY | O_NONBLOCK);
    int fdB = open(myFifo2, O_WRONLY);
    while(1) {
        read(fd, value, 100);
        sleep(0.01);
        counter += 1;    
        if (counter == 10000) {
            clock_gettime(CLOCK_REALTIME, &mt);
            mt.tv_sec += atoi(value)/18;
            hours = mt.tv_sec % 86400 / 3600;
            minutes = mt.tv_sec % 3600 / 60;
            seconds = mt.tv_sec % 60;
            printf("Time: %d:%d:%d\n", hours, minutes, seconds);
            sprintf(out, "Time: %d:%d:%d\n", hours, minutes, seconds);
            printf("%d\n", atoi(value)/18);
            write(fdB, out, strlen(out));
            counter = 0;
        }
    }
    close(fd);
    return 0;
}