/*********************************************
 * file:	~\code4b\graph.h                  *
 * remark:implementation of undirected graphs *
 **********************************************/

#include <iostream>
#include <algorithm>
#include <format>
#include <cassert>     // assert
#include <limits>      // std::numeric_limits
#include <algorithm>   // std::make_heap(), std::pop_heap(), std::push_heap()

#include "graph.h"
#include "dsets.h"

// Note: graph vertices are numbered from 1 -- i.e. there is no vertex zero

// -- CONSTRUCTORS

// Create a graph with n vertices and no vertices
Graph::Graph(int n) : table(n + 1), size{n}, n_edges{0} {
    assert(n >= 1);
}

Graph::Graph(const std::vector<Edge> &V, int n) : Graph{n} {
    for (auto e : V) {
        insertEdge(e);
    }
}

// -- MEMBER FUNCTIONS

// insert undirected edge e
// update weight if edge e is present
void Graph::insertEdge(const Edge &e) {
    assert(e.from >= 1 && e.from <= size);
    assert(e.to >= 1 && e.to <= size);

    auto edge_insertion = [&T = this->table, &n = this->n_edges](const Edge &e1) {
        if (auto it = std::find_if(begin(T[e1.from]), end(T[e1.from]),
                                   [e1](const Edge &ed) { return e1.links_same_nodes(ed); });
            it == end(T[e1.from])) {
            T[e1.from].push_back(e1);  // insert new edge e1
            ++n;                       // increment the counter of edges
        } else {
            it->weight = e1.weight;  // update the weight
        }
    };

    edge_insertion(e);
    edge_insertion(e.reverse());
}

// remove undirected edge e
void Graph::removeEdge(const Edge &e) {
    assert(e.from >= 1 && e.from <= size);
    assert(e.to >= 1 && e.to <= size);

    auto edgeRemoval = [&T = this->table, &n = this->n_edges](const Edge &e1) {
        auto it = std::find_if(begin(T[e1.from]), end(T[e1.from]),
                               [e1](const Edge &ed) { return e1.links_same_nodes(ed); });

        assert(it != end(T[e1.from]));
        T[e1.from].erase(it);  // remove edge e1
        --n;                   // decrement the counter of edges
    };

    edgeRemoval(e);
    edgeRemoval(e.reverse());
}

// Prim's minimum spanning tree algorithm
void Graph::mstPrim() const {
    std::vector<int> dist(size + 1, std::numeric_limits<int>::max());
    std::vector<int> path(size + 1, 0);
    std::vector<bool> done(size + 1, false);

    int v = 4;
    dist[v] = 0;
    done[v] = true;
    int total_weight = 0;

    while(true)
    {
        for(auto& e : table[v])
        {
            int u = e.to;

            if(done[u] == false && dist[u] > e.weight)
            {
                path[u] = v;
                dist[u] = e.weight;
            }
        }

        v = -1;
        
        for(int i = 0;i <= size;i++)
        {
            if(i != 0 && done[i] == false && (v == -1 || dist[v] > dist[i]))
            {
                v = i;
            }
        }
        
        if(v == -1)
            break;

        done[v] = true;
        std::cout << "( " << path[v] << ", " << v << ", " << dist[v] << ")\n";
        total_weight += dist[v];
    }
    
    std::cout << "\nTotal weight = " << total_weight << "\n";
}

// Kruskal's minimum spanning tree algorithm
void Graph::mstKruskal() const {
    std::vector<Edge> heap;

    for(int v = 0;v <= size;v++)
    {
        for(auto& e : table[v])
        {
            int u = e.to;
            
            if(v < u)
                heap.push_back(e);
        }
    }

    std::make_heap(heap.begin(), heap.end(), std::greater<Edge>{});

    std::sort_heap(heap.begin(), heap.end(), std::greater<Edge>{});
    
    DSets D = DSets(size);
    
    int counter = 0;
    int total_weight = 0;

    while(counter < size - 1)
    {
        auto top = heap.back();
        heap.pop_back();

        int u = top.to;
        int v = top.from;
        int w = top.weight;

        int Du = D.find(u);
        int Dv = D.find(v);
        
        if(Du != Dv)
        {
            std::cout << "( " << v << ", " << u << ", " << w << ")\n";
            total_weight += w;
            D.join(Du, Dv);
            counter++;
        }
    }
    std::cout << "\nTotal Weight = " << total_weight << "\n";
}

// print graph
void Graph::printGraph() const {
    std::cout << std::format("{:-<66}\n", '-');
    std::cout << "Vertex  adjacency lists\n";
    std::cout << std::format("{:-<66}\n", '-');

    for (int v = 1; v <= size; v++) {
        std::cout << std::format("{:4} : ", v);
        for (auto const &e : table[v]) {
            std::cout << std::format("({:2}, {:2}) ", e.to, e.weight);
        }
        std::cout << "\n";
    }
    std::cout << std::format("{:-<66}\n", '-');
}
