#include "header.h"
#include "boost/graph/push_relabel_max_flow.hpp"

void tpmaxflow(){
	typedef adjacency_list < vecS, vecS, directedS, no_property, property < edge_capacity_t, int,
		property < edge_residual_capacity_t, int,
		property < edge_reverse_t, adjacency_list<>::edge_descriptor >> >> Graph;
	typedef Graph::vertex_descriptor Vertex;
	typedef Graph::edge_descriptor Edge;
	Graph g;

	property_map<Graph, edge_capacity_t>::type
		capacity = get(edge_capacity, g);
	property_map<Graph, edge_reverse_t>::type
		rev = get(edge_reverse, g);
	property_map<Graph, edge_residual_capacity_t>::type
		residual_capacity = get(edge_residual_capacity, g);

	 
	// init
	Vertex v1 = add_vertex(g);
	Vertex v2 = add_vertex(g);
	Vertex v3 = add_vertex(g);
	Vertex v4 = add_vertex(g);
	Vertex v5 = add_vertex(g);
	Vertex v6 = add_vertex(g);
	Edge e12 = add_edge(v1, v2, g).first;
	Edge e15 = add_edge(v1, v5, g).first;
	Edge e23 = add_edge(v2, v3, g).first;
	Edge e24 = add_edge(v2, v4, g).first;
	Edge e36 = add_edge(v3, v6, g).first;
	Edge e45 = add_edge(v4, v5, g).first;
	Edge e46 = add_edge(v4, v6, g).first;
	Edge e56 = add_edge(v5, v6, g).first;
	capacity[e12] = 15;
	capacity[e15] = 18;
	capacity[e23] = 17;
	capacity[e24] = 13;
	capacity[e36] = 20;
	capacity[e45] = 5;
	capacity[e46] = 15;
	capacity[e56] = 10;
	rev[e12] = add_edge(v2, v1, g).first;
	rev[e15] = add_edge(v5, v1, g).first;
	rev[e23] = add_edge(v3, v2, g).first;
	rev[e24] = add_edge(v4, v2, g).first;
	rev[e36] = add_edge(v6, v3, g).first;
	rev[e45] = add_edge(v5, v4, g).first;
	rev[e46] = add_edge(v6, v4, g).first;
	rev[e56] = add_edge(v6, v5, g).first;
	rev[rev[e12]] = e12;
	rev[rev[e15]] = e15;
	rev[rev[e23]] = e23;
	rev[rev[e24]] = e24;
	rev[rev[e36]] = e36;
	rev[rev[e45]] = e45;
	rev[rev[e46]] = e46;
	rev[rev[e56]] = e56;

	capacity[rev[e12]] = capacity[rev[e15]] = capacity[rev[e23]] = capacity[rev[e24]] 
		= capacity[rev[e36]] = capacity[rev[e45]] = capacity[rev[e46]] = capacity[rev[e56]] = 0;

	ofstream outFile("graph.viz");
	write_graphviz(outFile, g);
	system("dot -Tpng -ograph.png graph.gv | explorer graph.png");

	//Vertex s, t;
	long flow;
	flow = push_relabel_max_flow(g, v1, v6);


	std::cout << "c  The total flow:" << std::endl;
	std::cout << "s " << flow << std::endl << std::endl;

	std::cout << "c flow values:" << std::endl;
	graph_traits<Graph>::vertex_iterator u_iter, u_end;
	graph_traits<Graph>::out_edge_iterator ei, e_end;
	for (boost::tie(u_iter, u_end) = vertices(g); u_iter != u_end; ++u_iter)
		for (boost::tie(ei, e_end) = out_edges(*u_iter, g); ei != e_end; ++ei)
			if (capacity[*ei] > 0)
				std::cout << "f " << *u_iter << " " << target(*ei, g) << " "
				<< (capacity[*ei] - residual_capacity[*ei]) << std::endl;

}