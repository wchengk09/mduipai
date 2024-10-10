// DO NOT EDIT
// DO NOT EDIT

#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include <semaphore.h>
using namespace std;
const int MAX_THREADS = 1000005;
int timelim,memlim;
pthread_mutex_t end_mutex;
sem_t cr_sem;
volatile long long cnt[MAX_THREADS];

long long tim(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

inline void err(string msg,int tid = 0){
    pthread_mutex_lock(&end_mutex);
    printf("%s",msg.c_str());
    if (tid){
        system(("cp a-" + to_string(tid) + ".txt a.txt > /dev/null 2>&1").c_str());
        system(("cp b-" + to_string(tid) + ".txt b.txt > /dev/null 2>&1").c_str());
        system(("cp in-" + to_string(tid) + ".txt in.txt > /dev/null 2>&1").c_str());
        system(("cp diff-" + to_string(tid) + ".txt diff.txt > /dev/null 2>&1").c_str());
    }
    pid_t pid = getpid();
    if (fork() == 0){
        int stat;
        waitpid(pid,&stat,0);
        system("rm in-*.txt a-*.txt b-*.txt diff-*.txt> /dev/null 2>&1");
    }
    exit(0);
}

void sigint(int s){
    s = s;
    err("\e[31mInterrupted\e[0m");
}


void run(int id,const char *name,const char *in,const char *out){
    pid_t pid = fork();
    if (pid == 0){
        if (strlen(in))freopen(in,"r",stdin);
        if (strlen(out))freopen(out,"w",stdout);
        nice(19);
        execl(name,name,NULL);
        abort();
    }else{
        long long st = tim();
        int run_time = 0;
        char *buf = new char[4096];
        memset(buf,0,sizeof(char) * 4096);
        while (1){
            run_time = tim() - st;
            // printf("run_time %d\n",run_time);
            if (run_time > (long long)timelim){
                kill(pid,SIGKILL);
                err("\t\e[33m" + (string)name + " \e[31mTLE\e[0m\n",id);
            }
            int mem_use = 0;
            FILE *fp = fopen(("/proc/" + to_string(pid) + "/status").c_str(),"r");
            while (fgets(buf,4096,fp)){
                // printf("%s\n",buf);
                if (memmem(buf,4096,"VmRSS:",6)){
                    // printf("%s\n",buf);
                    sscanf(buf,"%*s%d",&mem_use);
                }
            }
            fclose(fp);
            // printf("mem_use %d %s\n",mem_use,mem_use > memlim * 1000 ? "TLE" : "");
            if (mem_use > memlim * 1000){
                kill(pid,SIGKILL);
                err("\t\e[33m" + (string)name + " \e[31mMLE\e[0m\n",id);
            }
            int stat;
            if (waitpid(pid,&stat,WNOHANG)){
                if (!WIFEXITED(stat)){
                    err("\t\e[33m" + (string)name + " \e[31mRE\e[0m\n",id);
                    exit(0);
                }
                else break;
            }
            usleep(1000);
        }
        delete[] buf;
    }
}

void *judge_main(void *ptr){
    int tid = *(int*)ptr;
    sem_post(&cr_sem);
    while (1){
        string in = "in-" + to_string(tid) + ".txt";
        string a = "a-" + to_string(tid) + ".txt";
        string b = "b-" + to_string(tid) + ".txt";
        run(tid,"./rand","",in.c_str());
        run(tid,"./a",in.c_str(),a.c_str());
        run(tid,"./b",in.c_str(),b.c_str());
        int val = system(("diff -Z " + a + ' ' + b + " > diff-" + to_string(tid) + ".txt 2>&1").c_str());
        if (val == 130)
            sigint(SIGINT);
        else if (val)
            err("\t\e[31mWrong Answer\e[0m\n",tid);
        cnt[tid] ++;
    }
}

int main(int argc,char **argv){
    int threads = (argc >= 2 ? atoi(argv[1]) : 4);
    timelim = (argc >= 3 ? atoi(argv[2]) : 1000);
    memlim = (argc >= 4 ? atoi(argv[3]) : 256);

    nice(19);    
    pthread_mutex_init(&end_mutex,nullptr);
    sem_init(&cr_sem,0,0);
    signal(SIGINT,sigint);
    
    for (int i = 1;i <= threads;i ++){
        pthread_t tmp;
        pthread_create(&tmp,nullptr,judge_main,(void*)&i);
        sem_wait(&cr_sem);
    }

    long long st = tim();

    while (1){
        long long sum = 0;
        for (int i = 1;i <= threads;i ++)
            sum += cnt[i];
        pthread_mutex_lock(&end_mutex);
        printf("\x1b[H\x1b[2J"); 
        long long dt = tim() - st;
        printf("Testcase \033[36m#%lld\033[0m\nAverage speed: \e[35m%lld testcases/min\e[0m\n",sum,dt == 0 ? 0 : (sum * 60000 / dt));
        pthread_mutex_unlock(&end_mutex);
        usleep(50000);
    }

    return 0;
}