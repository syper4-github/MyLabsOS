#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int state = 1;
int button0 = 1;
int button1 = 1;
int button2_interrupt = 0;
int button2_interrupt_past = 0;
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
int fdB;

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

int GPIO26Read()
{
    int button2 = 0;
    FILE* gpio26_value;
    gpio26_value = fopen("/sys/module/driver/parameters/button_state", "r");
    fscanf(gpio26_value, "%d", &button2);
    fclose(gpio26_value);
    return button2;
}

void *Thread1()
{
    mkfifo(myFifo, 0666);
    int fd = open(myFifo, O_RDONLY | O_NONBLOCK);
    while(1) {
        pthread_mutex_lock(&mutex);
        read(fd, value, 100);
        sleep(0.01);
        counter += 1;
        if (counter == 10001)
            counter = 0;
        pthread_mutex_unlock(&mutex);
    }
    close(fd);
}

void *Thread2()
{
    mkfifo(myFifo2, 0666);
    fdB = open(myFifo2, O_WRONLY);
    while(1) {
        if (counter == 10000) {
            pthread_mutex_lock(&mutex);
            sprintf(out, "Time: %d:%d:%d\n", hours, minutes, seconds);
            write(fdB, out, strlen(out));
            pthread_mutex_unlock(&mutex);
        }
    }
    close(fdB);
}

void *Thread3()
{
    while(1) {
        if (counter == 10000) {
            clock_gettime(CLOCK_REALTIME, &mt);
            //mt.tv_sec += atoi(value) / 18;
            hours = (mt.tv_sec + hours_offset) % 86400 / 3600;
            minutes = (mt.tv_sec + minutes_offset) % 3600 / 60;
            seconds = (mt.tv_sec + seconds_offset) % 60;
            button0 = GPIO22Read();
            button1 = GPIO23Read();
            button2_interrupt = GPIO26Read();
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
            //printf("%d\n", atoi(value)/18);
            if(button2_interrupt != button2_interrupt_past)
                printf("Button 26 Pressed");
            button0_past = button0;
            button1_past = button1;
            button2_interrupt_past = button2_interrupt;
        }
    }
}

void *Thread4()
{
    while(1)
    {
        char str[80];
        printf("Enter a string: \n");
        fgets(str, 80, stdin);
        if (strstr(str, "start encoder") != 0) 
        {
            printf("Encoder Started!\n");
            system("sudo ./encoder/encoder -q > data_path &");
            /*char *binaryPath = "encoder/encoder";
            char *args[] = {binaryPath, "-q", ">", "data_path", NULL};
            int fd_proc = fork();
            if (fd_proc == 0){
            int x = execv(binaryPath, args);
            if (x == -1)
                printf("Sth wrong 1\n");
            }
            */
        }
        if (strstr(str, "stop encoder") != 0) 
        {
            printf("Encoder Stopped!\n");
            system("pgrep encoder | xargs -I {} sudo kill {}");
        }
        if (strstr(str, "start display") != 0) 
        {
            printf("Display Started!\n");
            system("sudo cat data_out | xargs -I {} sudo ./display_text/lcd {} &");
        }
        if (strstr(str, "stop display") != 0) 
        {
            printf("Display Stopped!\n");
            //system("pgrep xargs | xargs -I {} sudo kill {}");
            system("killall cat");
        }
        printf ("Here's your string: %s\n", str);
        strcpy(str, "");
    }
}

int main()
{
    pthread_t t1, t2, t3, t4;
    pthread_create(&t1, NULL, Thread1, NULL);
    pthread_create(&t2, NULL, Thread2, NULL);
    pthread_create(&t3, NULL, Thread3, NULL);
    pthread_create(&t4, NULL, Thread4, NULL);
    //system("echo 22 > /sys/class/gpio/export");
   // system("echo 23 > /sys/class/gpio/export");
  //  sleep(1);
    /*char *binaryPath = "encoder/encoder";
    char *args[] = {binaryPath, "-q", ">", "data_path", NULL};
    int fd_proc = fork();
    if (fd_proc == 0){
    int x = execv(binaryPath, args);
        if (x == -1)
          printf("Sth wrong 1\n");   
    }*/
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
    return 0;
}