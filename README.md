# Finding the Vertex Cover of a Randomly Generated Graph

## Overview

This project is designed and implemented in multithread architecture to find the vertex cover of a randomly generated graph using the MiniSat SAT solver and the other two methods. For a brief understanding of the vertex cover concept, refer to [this link](https://en.wikipedia.org/wiki/Vertex_cover#:~:text=In%20graph%20theory%2C%20a%20vertex,every%20edge%20of%20the%20graph).

For the MiniSat SAT solver, the [vertex_cover](https://github.com/SajjiKazemi/vertex_cover.git) project will be used. The other two methods have been implemented in this project. In summary, the second method involves selecting a vertex with the highest degree (most incident edges) and adding it to the vertex cover. Subsequently, all edges incident on that vertex are discarded, and the process is repeated until no edges remain. In the third method, an edge ⟨u, v⟩ is chosen, and both u and v are included in the vertex cover. Following this, all edges connected to u and v are eliminated, and the process is reiterated until no edges remain.

## Getting Started

### Clone the Repository

```bash
git clone https://github.com/SajjiKazemi/multi_threading_sat_solver.git
```

## Dependencies
Create a virtual environment using the provided my_env.yml file:

```bash
conda env create -f my_env.yml
```
Activate the environment:

```bash
conda activate my_env
```

## Usage

To run the project, inside the project directory, create the necessary directory:

```bash
mkdir build
```

and execute the following command line:

```bash
cmake ../
```

and then:

```bash
make
```

Run the executable file for main.cpp:


```bash
 ./main
```

Given a graph as input and assuming your executable is called main, the program will output the vertex cover computed by each approach in sorted order. That is, give the following input:

```bash
$ ./main

V 5

E {<3,2>,<3,1>,<3,4>,<2,5>,<5,4>}
```

The output from your program should be:

```bash
CNF-SAT-VC: 3,5
APPROX-VC-1: 3,5
APPROX-VC-2: 1,3,4,5
```

## Contact
For any questions or feedback, feel free to reach out:

Email: sajjikazemi@uwaterloo.ca

LinkedIn: www.linkedin.com/in/sajjadkazemi
