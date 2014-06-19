#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pty.h>
#include <time.h>
#include <sys/wait.h>
#include <stdarg.h>
#include "stringconv.h"

#define _PATH_BSHELL "/bin/sh"

void finish(int);
void done(void);
void fail(void);
void resize(int);
void fixtty(void);
void getmaster(void);
void getslave(void);
void doinput(void);
void dooutput(void);
void doshell(void);
void resize_clients(void);

static char *shell;
static int  master;
static int  slave;
static int  child;
static int  subchild;

static struct   termios tt;
static struct   winsize win;
static char *cflg = NULL;

static char *progname;
static int serialport; // by JSI
static time_t lastwrite = 0;

// ha ennyi másodperc üresjárat után íródik valami a terminálra,
// akkor a kiírást megelőzi egy BELL karakter kiküldése
static int beeplast = 181;

static char clear[] = {27, 91, 72, 27, 91, 74, 0};

void dumpf (const char *fmt, ...) {
    va_list ap;
    FILE *f = fopen("dump", "a");
    va_start(ap, fmt);
    vfprintf(f, fmt, ap);
    va_end(ap);
    fclose(f);
}

void sendport (char *data, int len) {
    int i;
    char data2[BUFSIZ]; // A BUFSIZ az stdio.h -ban van generálva és 4096 az értéke (Dellina 2.6.28)
    stringconv(data, len, data2, BUFSIZ);

    //~ memcpy(data2, data, len);

    char *tmp;
    char buf[8];
    for (i = 0; i < len; i++) {
        tmp = data2 + i;
        snprintf(buf, 2, "%c", (int)*tmp);
        write(serialport, buf, 1);
//~ dumpf("%d (%d)\n", (int)*tmp, len);
    }

    // csipogunk, ha már rég jártunk erre :)
    if (time(NULL) > lastwrite + beeplast) {
        int bell = 7;
        write(serialport, (char*)&bell, 1);
    }
    lastwrite = time(NULL);
}

void banner () {
    sendport(clear, sizeof(clear));
    int i;
    char *msg;

    for (i = 0; i < 480; i++)
        sendport(".", 1);
    msg = "..............                                                      ............";
    sendport(msg, strlen(msg));
    msg = ".............      J S S    T E L E T Y P E    T E R M I N A L       ...........";
    sendport(msg, strlen(msg));
    msg = "..............                                                      ............";
    sendport(msg, strlen(msg));
    for (i = 0; i < 480; i++)
        sendport(".", 1);
    sendport("\n\n", 2);
}

static int getspeed (int speed) {
    switch (speed) {
        case 300: return B300;
        case 600: return B600;
        case 1200: return B1200;
        case 1800: return B1800;
        case 2400: return B2400;
        case 4800: return B4800;
        case 9600: return B9600;
        case 19200: return B19200;
        case 38400: return B38400;
        case 57600: return B57600;
        case 115200: return B115200;
        default:
            fprintf(stderr, "Invalid speed!\n");
            exit(255);
    }
}

static void init_serialport (char *device, int speed) {
    serialport = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (serialport < 0) {
        perror(device);
        exit(-1);
    }

    fcntl(serialport, F_SETFL, 0);
    struct termios options;
    tcgetattr(serialport, &options); // serialport aktuális beállításainak lekérdezése
    cfsetispeed(&options, getspeed(speed)); // bejövő baud beállítása
    cfsetospeed(&options, getspeed(speed)); // kimenő baud beállítása
    options.c_cflag |= (CLOCAL | CREAD); // vevő engedélyezése, lokális mód beállítása
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    tcsetattr(serialport, TCSANOW, &options); // beállítások mentése
}

int main( int argc, char **argv) {
    extern int optind;

    char *p;
    progname = argv[0];
    if ((p = strrchr(progname, '/')) != NULL)
        progname = p+1;

    if (argc != 3 ) {
        fprintf(stderr, "JSS TeleType Connector (c) 2010 Andrew JSI\nusage: %s <serialport> <baud>\n", progname);
        return 255;
    }

    init_serialport(argv[1], atoi(argv[2]));
    printf("Connected to %s, forking...\n", argv[1]);
    banner();

    shell = getenv("SHELL");
    if (shell == NULL)
        shell = _PATH_BSHELL;

    getmaster();
    fixtty();

    (void) signal(SIGCHLD, finish);
    child = fork();
    if (child < 0) {
        perror("fork");
        fail();
    }
    if (child == 0) {
        subchild = child = fork();
        if (child < 0) {
            perror("fork");
            fail();
        }
        if (child)
            dooutput();
        else
            doshell();
    } else {
        (void) signal(SIGWINCH, resize);
    }
    doinput();

    return 0;
}

void doinput () {
    register int cc;
    char ibuf[BUFSIZ];

    while ((cc = read(0, ibuf, BUFSIZ)) > 0)
        (void) write(master, ibuf, cc);
    done();
}

void finish (int dummy) {
    int status;
    register int pid;
    register int die = 0;

    while ((pid = wait3(&status, WNOHANG, 0)) > 0)
        if (pid == child)
            die = 1;

    if (die) {
        done();
    }
}

void
resize(int dummy) {
    /* transmit window change information to the child */
    (void) ioctl(0, TIOCGWINSZ, (char *)&win);
    (void) ioctl(slave, TIOCSWINSZ, (char *)&win);

    kill(child, SIGWINCH);
}

void fatal() {
    done();
    exit(-1);
}

void sig_resize_clients (int x) {
    resize_clients();
}

void resize_clients () {
  int rows, cols;
  char tmp[100];
  int len;

  /* Get rows and cols from our connection to the terminal: fd 1 */
    (void) ioctl(0, TIOCGWINSZ, (char *)&win);
  rows = win.ws_row;
  cols = win.ws_col;

  /* Prepare the xterm resize string */
  snprintf(tmp, 100, "\033[8;%i;%it\n", rows, cols);
  len = strlen(tmp);
}

void send_noecho(int fd) {
  char seq[] = { 255 /*IAC*/, 251 /*WILL*/, 1 /*ECHO*/ };
  send(fd, seq, sizeof seq, 0);
}

void remove_element(int index, int *array, int total) {
    int i;
    for (i = index; i < (total - 1); ++i)
        array[i] = array[i+1];
}

void dooutput() {
    int cc;
    char obuf[BUFSIZ];
    int i;

/*  (void) close(0); lets use it in resize*/
    close(slave);
    fd_set read_set;
    fd_set should_read_set;

    FD_ZERO(&should_read_set);
    FD_SET(master, &should_read_set);
    signal(SIGWINCH, sig_resize_clients);
    for (;;) {
        int max;
        read_set = should_read_set;

        if (master > max)
            max = master;
        
        i = select(max + 1, &read_set, 0, 0, 0);
        if (i == -1)
          continue;

        if (FD_ISSET(master, &read_set)) {
            cc = read(master, obuf, sizeof (obuf));
            if (cc <= 0)
                break;
            /* Write to the terminal output */
            (void) write(1, obuf, cc);
            sendport(obuf, cc); // JSI
        }
    }
    done();
}

void doshell () {
    char *shname;
    printf("TERM is now \"vt100\"\r\n");
    setenv("TERM", "vt100", 1);

#if 0
    int t;
    t = open(_PATH_TTY, O_RDWR);
    if (t >= 0) {
        (void) ioctl(t, TIOCNOTTY, (char *)0);
        (void) close(t);
    }
#endif

    getslave();
    (void) close(master);
    (void) dup2(slave, 0);
    (void) dup2(slave, 1);
    (void) dup2(slave, 2);
    (void) close(slave);

    shname = strrchr(shell, '/');
    if (shname)
        shname++;
    else
        shname = shell;

    if (cflg)
        execl(shell, shname, "-c", cflg, NULL);
    else
        execl(shell, shname, "-i", NULL);

    perror(shell);
    fail();
}

void
fixtty() {
    struct termios rtt;

    rtt = tt;
    cfmakeraw(&rtt);
    rtt.c_lflag &= ~ECHO;
    (void) tcsetattr(0, TCSAFLUSH, &rtt);
}

void
fail() {

    (void) kill(0, SIGTERM);
    done();
}

void done() {
    if (subchild) {
        (void) close(master);
    } else {
        (void) tcsetattr(0, TCSAFLUSH, &tt);
        printf("\nLeaving terminal\n");
        sendport(clear, sizeof(clear));
    }
    exit(0);
}

void getmaster () {
    tcgetattr(0, &tt);
    ioctl(0, TIOCGWINSZ, (char *)&win);
    if (openpty(&master, &slave, NULL, &tt, &win) < 0) {
        fprintf(stderr, "openpty failed\n");
        fail();
    }
}

void getslave () {
    setsid();
    ioctl(slave, TIOCSCTTY, 0);
}


