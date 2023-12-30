#include <iostream>
#include <sstream>
#include "helpers.h"
#include "MyGraph.h"
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

bool g_cancel;
pthread_mutex_t g_lock;
pthread_cond_t g_cond;

typedef struct MyGraph threadGraph;

void *cnf_sat(void *data)
{
    //threadGraph *new_graph = static_cast<threadGraph *>(data);
    MyGraph *new_graph = (MyGraph *)data;
    while (!g_cancel)
    {
        pthread_mutex_lock(&g_lock);
        if (new_graph->edges.size() > 0)
        {
            new_graph->getVertexCover();
            new_graph->printVertexCover(true);
            new_graph->resetEverything();
            new_graph->setSize(0);
            
        }
        //std::cout << "I am here" << std::endl << std::flush;
        pthread_cond_wait(&g_cond, &g_lock);
        pthread_mutex_unlock(&g_lock);
        //usleep(1000);
    }
    return nullptr;
}

int main(int argc, char **argv) 
{
    pthread_mutex_init(&g_lock, nullptr);
    pthread_cond_init(&g_cond, nullptr);

    g_cancel = false;
    int ret = 0;
    pthread_t thread1;

    MyGraph graph;
    bool first = true;
    while (!std::cin.eof())
    {
        std::string line;
        std::getline(std::cin, line);
        std::string command = line_parser::get_command(line, graph);
        line_parser::analyze_command(command, line, graph);
        if (graph.edges.size() > 0)
        {
            if (first)
            {
                ret = pthread_create(&thread1, nullptr, cnf_sat, (void *)&graph);
                if (ret != 0)
                {
                    std::cout << "Error: pthread_create() failed\n";
                    exit(EXIT_FAILURE);
                }
                first = false;
            }
            else
            {
                pthread_mutex_lock(&g_lock);
                pthread_cond_signal(&g_cond);
                pthread_mutex_unlock(&g_lock);
            }
        }
        
    }
    g_cancel = true;
    pthread_mutex_lock(&g_lock);
    pthread_cond_signal(&g_cond);
    pthread_mutex_unlock(&g_lock);
    ret = pthread_join(thread1, nullptr);
    if (ret != 0)
    {
        std::cout << "Error: pthread_join() failed\n";
        exit(EXIT_FAILURE);
    }
    
 return 0;
}