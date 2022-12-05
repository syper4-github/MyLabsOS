#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <string.h>


int GPIO22Read()
{
    int button0 = 0;
    FILE* gpio22_value;
    gpio22_value = fopen("/sys/class/gpio/gpio22/value", "r");
    fscanf(gpio22_value, "%d", &button0);
    fclose(gpio22_value);
    return button0;
}

int GPIO23Read()
{
    int button1 = 0;
    FILE* gpio23_value;
    gpio23_value = fopen("/sys/class/gpio/gpio23/value", "r");
    fscanf(gpio23_value, "%d", &button1);
    fclose(gpio23_value);
    return button1;
}

int main()
{
    int state = 1;
    int button0 = 1;
    int button1 = 1;
    int button0_past = 0;
    int button1_past = 0;
    int counter = 0;
    struct timespec mt;
    int hours = 0;
    int hours_offset = 0;
    int minutes = 0;
    int minutes_offset = 0;
    int seconds = 0;
    int seconds_offset = 0;
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
            //mt.tv_sec += atoi(value) / 18;
            hours = (mt.tv_sec + hours_offset) % 86400 / 3600;
            minutes = (mt.tv_sec + minutes_offset) % 3600 / 60;
            seconds = (mt.tv_sec + seconds_offset) % 60;
            button0 = GPIO22Read();
            button1 = GPIO23Read();
            switch(state)
            {
                case 1:
                    if ( ( button0 == 0 ) && ( button0_past == 1 ) )
                        state = 3;
                    if ( ( button1 == 0 ) && ( button1_past == 1 ) )
                        state = 2;
                    seconds_offset = atoi(value) / 18;
                    break;

                case 2:
                    if ( ( button0 == 0 ) && ( button0_past == 1 ) )
                        state = 1;
                    if ( ( button1 == 0 ) && ( button1_past == 1 ) )
                        state = 3;
                    minutes_offset = atoi(value) / 18 * 60;
                    break;

                case 3:
                    if ( ( button0 == 0 ) && ( button0_past == 1 ) )
                        state = 2;
                    if ( ( button1 == 0 ) && ( button1_past == 1 ) )
                        state = 1;
                    hours_offset = atoi(value) / 18 * 3600;
                    break;

                default:
                    mt.tv_sec += atoi(value) / 18;
            }
            printf("Time: %d:%d:%d\n", hours, minutes, seconds);
            sprintf(out, "Time: %d:%d:%d\n", hours, minutes, seconds);
            printf("%d\n", atoi(value)/18);
            write(fdB, out, strlen(out));
            counter = 0;
            button0_past = button0;
            button1_past = button1;
        }
    }
    close(fd);
    return 0;
}