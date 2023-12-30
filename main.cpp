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

bool g_cancel;
pthread_mutex_t g_mutex;
struct ThreadData
{
    MyGraph *graph;
    pthread_mutex_t mutex_lock;
    pthread_cond_t condition;
    clockid_t clock_id;
};

void *cnf_sat(void *data)
{
    ThreadData *thread_data = static_cast<ThreadData *>(data);
    MyGraph *graph = thread_data->graph;
    std::string beginning = "CNF-SAT-VC: ";
    pthread_getcpuclockid(pthread_self(), &thread_data->clock_id);
    while (!g_cancel)
    {
        //pthread_cond_wait(&thread_data->condition, &g_mutex);
        if (graph->edges.size() > 0)
        {
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
            
        }


    }
    return nullptr;
}

void *approxCv1(void *data)
{
    ThreadData *thread_data = static_cast<ThreadData *>(data);
    MyGraph *graph = thread_data->graph;
    std::string beginning = "APPROX-VC-1: ";
    while (!g_cancel)
    {
        pthread_mutex_lock(&g_mutex);
        if (graph->edges.size() > 0)
        {
            graph->approxCv1();
            graph->printVertexCover(beginning, true);
            graph->resetEverything();
            graph->setSize(0);
        }
        pthread_cond_wait(&thread_data->condition, &g_mutex);
        pthread_mutex_unlock(&g_mutex);
    }
    return nullptr;
}

void *approxCv2(void *data)
{
    ThreadData *thread_data = static_cast<ThreadData *>(data);
    MyGraph *graph = thread_data->graph;
    std::string beginning = "APPROX-VC-2: ";
    while (!g_cancel)
    {
        pthread_mutex_lock(&g_mutex);
        if (graph->edges.size() > 0)
        {
            graph->approxCv2();
            graph->printVertexCover(beginning, true);
            graph->resetEverything();
            graph->setSize(0);
        }
        pthread_cond_wait(&thread_data->condition, &g_mutex);
        pthread_mutex_unlock(&g_mutex);
    }
    return nullptr;
}

int main(int argc, char **argv) 
{
    ThreadData* thread_data1 = new ThreadData;
    ThreadData* thread_data2 = new ThreadData;
    ThreadData* thread_data3 = new ThreadData;
    pthread_mutex_init(&g_mutex, nullptr);
    pthread_mutex_init(&thread_data1->mutex_lock, nullptr);
    pthread_cond_init(&thread_data1->condition, nullptr);

    pthread_mutex_init(&thread_data2->mutex_lock, nullptr);
    pthread_cond_init(&thread_data2->condition, nullptr);

    pthread_mutex_init(&thread_data3->mutex_lock, nullptr);
    pthread_cond_init(&thread_data3->condition, nullptr);

    g_cancel = false;
    int ret = 0;
    pthread_t thread1, thread2, thread3;

    MyGraph graph;
    MyGraph graph_t1;
    MyGraph graph_t2;
    MyGraph graph_t3;
    bool first = true;
    while (!std::cin.eof())
    {
        std::string line;
        std::getline(std::cin, line);
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
                graph.edges.clear();
            }
        }
        
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
