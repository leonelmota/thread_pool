#include <pthread.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include "safe_queue.h"
#include "task.h"

using namespace std;

int const LIMIT_THREADS = 40;
int count_threads = 0;
int count_wait_threads = 0;

pthread_mutex_t m_count;
pthread_cond_t finished;
SafeQueue<task_descr_t> q(LIMIT_THREADS);

int min_threads, max_threads, current_tid = 1;

task_descr_t eow = {0, 0};

void* trab(void *args){
    int tid = *(int*)args;

    while(true){
        task_descr_t task = q.pop();
        if (task.pid > 0){
            pthread_mutex_lock(&m_count);
            count_wait_threads--;
            pthread_mutex_unlock(&m_count);

            processa(&task);

            pthread_mutex_lock(&m_count);
            if (++count_wait_threads > min_threads){
                count_threads--;
                count_wait_threads--;
                if (count_threads == 0)
                    pthread_cond_signal(&finished);
                pthread_mutex_unlock(&m_count);
                cout << "TE " << tid << endl;
                free(args);
                pthread_exit((void*)EXIT_SUCCESS);
            }
            pthread_mutex_unlock(&m_count);
        }
        else{
            pthread_mutex_lock(&m_count);
            if (--count_threads == 0)
                pthread_cond_broadcast(&finished);
            pthread_mutex_unlock(&m_count);

            cout << "TE " << tid << endl;
            free(args);
            pthread_exit((void*)EXIT_SUCCESS);
        }

    }
}

void create(){
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    int *tid = new int;
    *tid = current_tid;
    pthread_t thread;

    int rc = pthread_create(&thread, &attr, trab, tid);
    if (rc == 0){
        cout << "TB " << *tid << endl;
        count_threads++;
        current_tid++;
        count_wait_threads++;
    }
}


int main(int argc, char* argv[]){
    
    min_threads = atoi(argv[1]);
    max_threads = atoi(argv[2]);

    pthread_mutex_init(&m_count, NULL);
    pthread_cond_init(&finished, NULL);
    

    pthread_mutex_lock(&m_count);
    for (int i=0; i<min_threads; i++) create();
    pthread_mutex_unlock(&m_count);
    
    while(true){
        task_descr_t task;
        if ((cin >> task.pid >> task.ms).eof()) break;
 
        q.push(task);
        pthread_mutex_lock(&m_count);
        if (count_wait_threads < q.size() and count_threads < max_threads){
            create();
        }
        pthread_mutex_unlock(&m_count);
    }

    for (int i=0; i<LIMIT_THREADS; i++) q.push(eow);

    pthread_mutex_lock(&m_count);
    while(count_threads > 0) pthread_cond_wait(&finished, &m_count);
    pthread_mutex_unlock(&m_count);

    return 0;

}