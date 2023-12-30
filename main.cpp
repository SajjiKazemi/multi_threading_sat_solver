#include <iostream>
#include <sstream>
#include "helpers.h"
#include "MyGraph.h"
#include <pthread.h>
#include <ctime>
#include <ratio>
#include <chrono>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <atomic>
#include <thread>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <sys/types.h>

bool g_cancel;
bool check_timeout = false;
bool stop_flag = false;
bool thread_deleted = false;
pthread_mutex_t g_mutex;
struct ThreadData
{
    MyGraph *graph;
    pthread_mutex_t mutex_lock;
    pthread_cond_t condition;
    clockid_t clock_id;
    bool ready_for_task;
};

void *cnf_sat(void *data)
{
    ThreadData *thread_data = static_cast<ThreadData *>(data);
    MyGraph *graph = thread_data->graph;
    std::string beginning = "CNF-SAT-VC: ";
    pthread_getcpuclockid(pthread_self(), &thread_data->clock_id);
    std::chrono::high_resolution_clock::time_point timeout1;
    std::chrono::high_resolution_clock::time_point timeout2;
    int child_to_parent_pipe[2];
    pipe(child_to_parent_pipe);
    std::cout << "pid is" << syscall(SYS_gettid) << std::endl;
    pid_t child_pid = fork();
    timeout1 = std::chrono::high_resolution_clock::now();
    if (child_pid == 0)
    {
        dup2(child_to_parent_pipe[1], STDIN_FILENO);
        close(child_to_parent_pipe[0]);
        close(child_to_parent_pipe[1]);

        // while (!g_cancel && !stop_flag)
        // {
            // std::cout << "pid is" << getpid() << std::endl << std::flush;
            // std::cout << "This is child" << std::endl << std::flush;
            //pthread_cond_wait(&thread_data->condition, &g_mutex);
            if (graph->edges.size() > 0)
            {
                // std::cout << "pid is" << syscall(SYS_gettid) << std::endl;
                // pid_t pid = syscall(SYS_gettid);
                // kill(pid, SIGTERM);
                std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
                graph->CnfSatVc();
                std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
                pthread_mutex_lock(&g_mutex);
                graph->printVertexCover(beginning, true);
                std::cout << "CNF-SAT-VC time: " << duration << " microseconds\n" << std::flush;
                pthread_mutex_unlock(&g_mutex);
                graph->resetEverything();
                graph->setSize(0);
                //thread_data->ready_for_task = false;
                
            }
        // }
        //thread_data->ready_for_task = false;
        return nullptr;
    }
    else if (child_pid < 0)
    {
        std::cout << "Error: fork() failed\n";
        exit(EXIT_FAILURE);
    }
    //else if (child_pid > 0)
    //{
        timeout2 = std::chrono::high_resolution_clock::now();
        auto timeout = std::chrono::duration_cast<std::chrono::microseconds>(timeout2 - timeout1).count();
        while (timeout < 14000000 && thread_data->ready_for_task)
        {
            timeout2 = std::chrono::high_resolution_clock::now();
            timeout = std::chrono::duration_cast<std::chrono::microseconds>(timeout2 - timeout1).count();
            int status;
            if (waitpid(child_pid, &status, WNOHANG))
            {
                thread_data->ready_for_task = false;
                timeout = 0;
            }
        }
        std::cout << "Timeout" << std::endl << std::flush;
        if (timeout >= 14000000 && thread_data->ready_for_task)
        {
            // g_cancel = true;
            // stop_flag = true;
            // MyGraph graph;
            // graph_t1 = graph;
            // thread_data1->graph = &graph_t1;
            pthread_mutex_lock(&g_mutex);
            std::cout << "Timeout" << std::endl << std::flush;
            thread_data->ready_for_task = false;
            std::cout << "Killing" << std::endl << std::flush;
            usleep(1000000);  // 100 milliseconds
            kill(child_pid, SIGTERM);
            int status;
            waitpid(child_pid, &status, 0);
            if (WIFEXITED(status)) 
                {
                    std::cout << "Child process exited with status: " << WEXITSTATUS(status) << std::endl << std::flush;
                } 
            else if (WIFSIGNALED(status)) 
                {
                    std::cout << "Child process terminated by signal: " << WTERMSIG(status) << std::endl << std::flush;
                } 
            else 
                {
                    std::cout << "Child process terminated abnormally." << std::endl << std::flush;
                }
            pthread_mutex_unlock(&g_mutex);
        }
        
        // int status;
        // std::cout << "it's running!" << std::endl << std::flush;
        //waitpid(child_pid, &status, 0);
        // if (WIFEXITED(status))
        // {
        //     std::cout << "Child process exited with status " << WEXITSTATUS(status) << std::endl << std::flush;

        // }
        // usleep(10000);
        // std::cout << "Killing" << std::endl << std::flush;
        // kill(child_pid, SIGTERM);

        // waitpid(child_pid, &status, 0);
        // if (WIFEXITED(status)) 
        //     {
        //         std::cout << "Child process exited with status: " << WEXITSTATUS(status) << std::endl;
        //     } 
        // else if (WIFSIGNALED(status)) 
        //     {
        //         std::cout << "Child process terminated by signal: " << WTERMSIG(status) << std::endl;
        //     } 
        // else 
        //     {
        //         std::cout << "Child process terminated abnormally." << std::endl;
        //     }
        
    //}

    return nullptr;
}

void *approxCv1(void *data)
{
    ThreadData *thread_data = static_cast<ThreadData *>(data);
    MyGraph *graph = thread_data->graph;
    std::string beginning = "APPROX-VC-1: ";
    pthread_getcpuclockid(pthread_self(), &thread_data->clock_id);
    while (!g_cancel)
    {
        if (graph->edges.size() > 0)
        {
            std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
            graph->approxCv1();
            std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
            pthread_mutex_lock(&g_mutex);
            graph->printVertexCover(beginning, true);
            std::cout << "APPROX-VC-1 time: " << duration << " microseconds\n" << std::flush;
            pthread_mutex_unlock(&g_mutex);
            graph->resetEverything();
            graph->setSize(0);
            thread_data->ready_for_task = false;
        }
        //pthread_cond_wait(&thread_data->condition, &g_mutex);
    }
    return nullptr;
}

void *approxCv2(void *data)
{
    ThreadData *thread_data = static_cast<ThreadData *>(data);
    MyGraph *graph = thread_data->graph;
    std::string beginning = "APPROX-VC-2: ";
    pthread_getcpuclockid(pthread_self(), &thread_data->clock_id);
    while (!g_cancel)
    {
        if (graph->edges.size() > 0)
        {
            std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
            graph->approxCv2();
            std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
            pthread_mutex_lock(&g_mutex);
            graph->printVertexCover(beginning, true);
            std::cout << "APPROX-VC-2 time: " << duration << " microseconds\n" << std::flush;
            pthread_mutex_unlock(&g_mutex);
            graph->resetEverything();
            graph->setSize(0);
            thread_data->ready_for_task = false;
        }
        //pthread_cond_wait(&thread_data->condition, &g_mutex);
    }
    return nullptr;
}

int main(int argc, char **argv) 
{
    std::atomic<bool> stop_flag(false);

    ThreadData* thread_data1 = new ThreadData;
    ThreadData* thread_data2 = new ThreadData;
    ThreadData* thread_data3 = new ThreadData;

    thread_data1->ready_for_task = true;
    thread_data2->ready_for_task = true;
    thread_data3->ready_for_task = true;

    pthread_mutex_init(&g_mutex, nullptr);
    pthread_mutex_init(&thread_data1->mutex_lock, nullptr);
    pthread_cond_init(&thread_data1->condition, nullptr);

    pthread_mutex_init(&thread_data2->mutex_lock, nullptr);
    pthread_cond_init(&thread_data2->condition, nullptr);

    pthread_mutex_init(&thread_data3->mutex_lock, nullptr);
    pthread_cond_init(&thread_data3->condition, nullptr);

    std::cout << "pid is" << getpid() << std::endl;

    g_cancel = false;
    int ret = 0;
    pthread_t thread1, thread2, thread3;

    MyGraph graph;
    MyGraph graph_t1;
    MyGraph graph_t2;
    MyGraph graph_t3;
    bool first = true;
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point next;

    while (!std::cin.eof())
    {
        if (thread_data1->ready_for_task && thread_data2->ready_for_task && thread_data3->ready_for_task)
        {
            // thread_data1->ready_for_task = false;
            // thread_data2->ready_for_task = false;
            // thread_data3->ready_for_task = false;

            std::string line;
            if (!check_timeout)
            {
                std::getline(std::cin, line);
            }
            std::string command = line_parser::get_command(line, graph);
            line_parser::analyze_command(command, line, graph);
            if (graph.edges.size() > 0)
            {
                graph_t1 = graph;
                graph_t2 = graph;
                graph_t3 = graph;
                thread_data1->graph = &graph_t1;
                thread_data2->graph = &graph_t2;
                thread_data3->graph = &graph_t3;

                if (first)
                {
                    check_timeout = true;
                    start = std::chrono::high_resolution_clock::now();
                    std::cout << "pid is" << getpid() << std::endl;

                    ret = pthread_create(&thread1, nullptr, cnf_sat, static_cast<void *>(thread_data1));
                    if (ret != 0)
                    {
                        std::cout << "Error: pthread_create() failed\n";
                        exit(EXIT_FAILURE);
                    }
                    ret = pthread_create(&thread2, nullptr, approxCv1, static_cast<void *>(thread_data2));
                    if (ret != 0)
                    {
                        std::cout << "Error: pthread_create() failed\n";
                        exit(EXIT_FAILURE);
                    }
                    first = false;
                    ret = pthread_create(&thread3, nullptr, approxCv2, static_cast<void *>(thread_data3));
                    if (ret != 0)
                    {
                        std::cout << "Error: pthread_create() failed\n";
                        exit(EXIT_FAILURE);
                    }
                    graph.edges.clear();
                }
                else
                {
                    check_timeout = true;
                    start = std::chrono::high_resolution_clock::now();
                    //pthread_mutex_lock(&thread_data1->mutex_lock);
                    if (thread_deleted)
                    {
                        ret = pthread_create(&thread1, nullptr, cnf_sat, static_cast<void *>(thread_data1));
                        if (ret != 0)
                        {
                            std::cout << "Error: pthread_create() failed\n";
                            exit(EXIT_FAILURE);
                        }
                        thread_deleted = false;
                    }
                    else
                    {
                        pthread_mutex_lock(&g_mutex);
                        pthread_cond_signal(&thread_data1->condition);
                        pthread_mutex_unlock(&g_mutex);
                    }

                    pthread_mutex_lock(&g_mutex);
                    //pthread_mutex_lock(&thread_data2->mutex_lock);
                    pthread_cond_signal(&thread_data2->condition);
                    //pthread_mutex_unlock(&thread_data2->mutex_lock);
                    pthread_mutex_unlock(&g_mutex);

                    pthread_mutex_lock(&g_mutex);
                    //pthread_mutex_lock(&thread_data3->mutex_lock);
                    pthread_cond_signal(&thread_data3->condition);
                    //pthread_mutex_unlock(&thread_data3->mutex_lock);
                    pthread_mutex_unlock(&g_mutex);
                    graph.edges.clear();
                }


            }
        }
        
        else if (!thread_data1->ready_for_task && !thread_data2->ready_for_task && !thread_data3->ready_for_task)
        {
            check_timeout = false;
            thread_deleted = true;
            thread_data1->ready_for_task = true;
            thread_data2->ready_for_task = true;
            thread_data3->ready_for_task = true;
            std::cout << std::endl << std::endl << std::flush;
        }
        
        next = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(next - start).count();
        // struct timespec timeout;
        // clock_gettime(CLOCK_REALTIME, &timeout);
        // timeout.tv_sec += 10;
        // pthread_timedjoin_np(thread1, nullptr, &timeout);

        // if (duration > 5 && check_timeout)
        //     {
        //         std::cout << "Timeout\n";
        //         // g_cancel = true;
        //         stop_flag = true;
        //         // MyGraph graph;
        //         // graph_t1 = graph;
        //         // thread_data1->graph = &graph_t1;
        //         pthread_mutex_lock(&g_mutex);
        //         pthread_cond_signal(&thread_data1->condition);
        //         pthread_mutex_unlock(&g_mutex);

        //         ret = pthread_join(thread1, nullptr);
        //         if (ret != 0)
        //         {
        //             std::cout << "Error: pthread_join() failed\n";
        //             exit(EXIT_FAILURE);
        //         }

        //         check_timeout = false;
        //         stop_flag = false;
        //         thread_data1->ready_for_task = true;
        //         thread_data2->ready_for_task = true;
        //         thread_data3->ready_for_task = true;
        //         std::cout << std::endl << std::endl << std::flush;
        //     }
        
        
    }
    g_cancel = true;
    //pthread_mutex_lock(&thread_data1->mutex_lock);
    pthread_mutex_lock(&g_mutex);
    pthread_cond_signal(&thread_data1->condition);
    //pthread_mutex_unlock(&thread_data1->mutex_lock);
    pthread_mutex_unlock(&g_mutex);

    pthread_mutex_lock(&g_mutex);
    //pthread_mutex_lock(&thread_data2->mutex_lock);
    pthread_cond_signal(&thread_data2->condition);
    //pthread_mutex_unlock(&thread_data2->mutex_lock);
    pthread_mutex_unlock(&g_mutex);

    pthread_mutex_lock(&g_mutex);
    //pthread_mutex_lock(&thread_data3->mutex_lock);
    pthread_cond_signal(&thread_data3->condition);
    //pthread_mutex_unlock(&thread_data3->mutex_lock);
    pthread_mutex_unlock(&g_mutex);

    ret = pthread_join(thread1, nullptr);
    if (ret != 0)
    {
        std::cout << "Error: pthread_join() failed\n";
        exit(EXIT_FAILURE);
    }

    ret = pthread_join(thread2, nullptr);
    if (ret != 0)
    {
        std::cout << "Error: pthread_join() failed\n";
        exit(EXIT_FAILURE);
    }

    ret = pthread_join(thread3, nullptr);
    if (ret != 0)
    {
        std::cout << "Error: pthread_join() failed\n";
        exit(EXIT_FAILURE);
    }
    
 return 0;
}
