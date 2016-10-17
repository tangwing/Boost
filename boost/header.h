#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/range/adaptor/adjacent_filtered.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graph_utility.hpp>    // For print_graph
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/topological_sort.hpp>

#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/graph/floyd_warshall_shortest.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>
#include <boost/graph/astar_search.hpp>


#include <boost/graph/random.hpp>
#include <boost/random.hpp>
#include <boost/variant.hpp>
#include <boost/any.hpp>
#include <boost/concept_check.hpp>
#include <boost/concept/assert.hpp>

#pragma once
#include <functional>
#include <vector>
#include <iostream>
#include <fstream>
#include <typeinfo>
#include <type_traits>
#include <chrono>
#include <stdlib.h>

using namespace  std;
using namespace std::chrono;
using namespace boost;
using namespace boost::graph;
using namespace boost::adaptors;

#pragma warning( disable : 4996 )

#define foreach_ BOOST_FOREACH 
#define MESURE_TIME(func) {    auto before = steady_clock::now();\
	func; \
	cout << "Time used :" << chrono::duration_cast<chrono::seconds>(steady_clock::now() - before).count() << "s\n";\
}

#include<random>
template <typename Graph>
void generateCompleteGraph(Graph & g, int N, int edgeWeightMin = 2, int edgeWeightMax = 10)
{
	BOOST_CONCEPT_ASSERT((MutableGraphConcept<Graph>));
	auto rand = bind(uniform_int_distribution<int>(edgeWeightMin, edgeWeightMax), default_random_engine(time(nullptr)));
	g.clear();
	for (int i = 0; i < N; ++i) add_vertex(g);
	property_map<Graph, edge_weight_t>::type edgeWeightMap = get(edge_weight, g);
	for (auto itv = vertices(g); itv.first != itv.second; ++itv.first)
	{
		for (auto itv2 = std::next(itv.first); itv2 != itv.second; ++itv2)
		{
			add_edge(*(itv.first), *itv2, rand(), g);
		}
	}
}



template<typename T_WEIGHT_t, typename Graph>
void generateDirectedGraph(Graph & g, T_WEIGHT_t edge_w, int N, int N_edge = -1, int edgeWMin = 0, int edgeWMax = 20){
	BOOST_CONCEPT_ASSERT((MutableGraphConcept<Graph>));
	auto rando = bind(uniform_int_distribution<int>(edgeWMin, edgeWMax), default_random_engine(time(nullptr)));
	g.clear();
	generate_random_graph(g, N, N_edge == 0 ? uniform_int_distribution<int>(N + 1, N*N)(default_random_engine()) : N_edge, default_random_engine(), false);
	property_map<Graph, T_WEIGHT_t>::type edgeWeightMap = get(edge_w, g);
	foreach_(auto e, make_iterator_range(edges(g)))
		edgeWeightMap[e] = rando();
}



template <typename T>
void print_predecessor(const vector<T> & pred)
{
	for (int i = 0; i < pred.size(); ++i)
	{
		cout << "Parent(" << i << ")=" << pred[i] << "\n";
	}
}

template<typename Graph>
void print_edges_w(Graph g)
{
	auto wp = get(edge_weight, g);
	for (auto itv = edges(g); itv.first != itv.second; ++itv.first)
	{
		cout << source(*itv.first, g) << " -- " << target(*itv.first, g) << " : " << wp[*itv.first] << "\n";
	}
}

template<typename T>
vector<vector<T> > & readAllFromFile(string fileName, int nbFieldPerLine)
{
	static vector<vector<T> > dataGrid;
	ifstream inf(fileName);
	if (!inf) cerr << "Error reading file.\n";
	else
	{
		istream_iterator<T> eof;
		istream_iterator<T> it(inf);
		int counter = 0;
		while (it != eof)
		{
			++counter;
			vector<T> line;
			for (int i = 0; i < nbFieldPerLine; ++i)
			{
				if (it == eof) cerr << "Error at line " << counter << "\n";
				line.push_back(*it++);
			}
			dataGrid.push_back(line);
		}
	}
	return dataGrid;
}