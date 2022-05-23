#ifndef SAFE_QUEUE
#define SAFE_QUEUE

#include <queue>
#include <pthread.h>

template <class T>
class SafeQueue
{
public:
  SafeQueue(int max_size){
      this->max_size = max_size;
      pthread_mutex_init(&m, NULL);
      pthread_cond_init(&c_not_empty, NULL);
      pthread_cond_init(&c_not_full, NULL);
  }

  ~SafeQueue(void){}

  void push(T t){
    pthread_mutex_lock(&m);
    while(q.size() > max_size){
       pthread_cond_wait(&c_not_full, &m);
    }
    q.push(t); 
    pthread_cond_signal(&c_not_empty);
    pthread_mutex_unlock(&m);
  }

  T pop(void){
    pthread_mutex_lock(&m);
    while(q.empty()){
      pthread_cond_wait(&c_not_empty, &m);
    }
    T val = q.front();
    q.pop();
    pthread_cond_signal(&c_not_full);
    pthread_mutex_unlock(&m);
    return val;
  }

  int size(){
    pthread_mutex_lock(&m);
    int s = q.size();
    pthread_mutex_unlock(&m);
    return s; 
  }

private:
  int max_size;  
  std::queue<T> q;
  pthread_mutex_t m;
  pthread_cond_t c_not_empty;
  pthread_cond_t c_not_full;
};
#endif
