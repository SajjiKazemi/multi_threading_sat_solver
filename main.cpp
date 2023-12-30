#include <iostream>
#include <sstream>
#include "helpers.h"
#include "MyGraph.h"
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

bool g_cancel;
struct ThreadData
{
    MyGraph *graph;
    pthread_mutex_t mutex_lock;
    pthread_cond_t condition;
};

void *cnf_sat(void *data)
{
    ThreadData *thread_data = static_cast<ThreadData *>(data);
    MyGraph *graph = thread_data->graph;
    std::string beginning = "CNF-SAT-VC: ";

    while (!g_cancel)
    {
        pthread_mutex_lock(&thread_data->mutex_lock);
        if (graph->edges.size() > 0)
        {
            graph->CnfSatVc();
            graph->printVertexCover(beginning, true);
            graph->resetEverything();
            graph->setSize(0);
            
        }
        pthread_cond_wait(&thread_data->condition, &thread_data->mutex_lock);
        pthread_mutex_unlock(&thread_data->mutex_lock);
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
        pthread_mutex_lock(&thread_data->mutex_lock);
        if (graph->edges.size() > 0)
        {
            graph->approxCv1();
            graph->printVertexCover(beginning, true);
            graph->resetEverything();
            graph->setSize(0);
        }
        pthread_cond_wait(&thread_data->condition, &thread_data->mutex_lock);
        pthread_mutex_unlock(&thread_data->mutex_lock);
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
        pthread_mutex_lock(&thread_data->mutex_lock);
        if (graph->edges.size() > 0)
        {
            graph->approxCv2();
            graph->printVertexCover(beginning, true);
            graph->resetEverything();
            graph->setSize(0);
        }
        pthread_cond_wait(&thread_data->condition, &thread_data->mutex_lock);
        pthread_mutex_unlock(&thread_data->mutex_lock);
    }
    return nullptr;
}

int main(int argc, char **argv) 
{
    ThreadData* thread_data1 = new ThreadData;
    ThreadData* thread_data2 = new ThreadData;
    ThreadData* thread_data3 = new ThreadData;

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
            }
            else
            {
                pthread_mutex_lock(&thread_data1->mutex_lock);
                pthread_cond_signal(&thread_data1->condition);
                pthread_mutex_unlock(&thread_data1->mutex_lock);

                pthread_mutex_lock(&thread_data2->mutex_lock);
                pthread_cond_signal(&thread_data2->condition);
                pthread_mutex_unlock(&thread_data2->mutex_lock);

                pthread_mutex_lock(&thread_data3->mutex_lock);
                pthread_cond_signal(&thread_data3->condition);
                pthread_mutex_unlock(&thread_data3->mutex_lock);
            }
        }
        
    }
    g_cancel = true;
    pthread_mutex_lock(&thread_data1->mutex_lock);
    pthread_cond_signal(&thread_data1->condition);
    pthread_mutex_unlock(&thread_data1->mutex_lock);

    pthread_mutex_lock(&thread_data2->mutex_lock);
    pthread_cond_signal(&thread_data2->condition);
    pthread_mutex_unlock(&thread_data2->mutex_lock);

    pthread_mutex_lock(&thread_data3->mutex_lock);
    pthread_cond_signal(&thread_data3->condition);
    pthread_mutex_unlock(&thread_data3->mutex_lock);

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
