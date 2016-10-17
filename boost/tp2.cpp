#include "header.h"
#include<chrono>

void tp2_ex1()
{
	typedef adjacency_list<vecS, vecS, undirectedS, no_property, property<edge_weight_t, int>> GraphTp2;
	typedef property_map<GraphTp2, edge_weight_t>::type WeightProperty;
	GraphTp2 g;
	generateCompleteGraph(g, 6, 2, 10);
	WeightProperty wp = get(edge_weight, g);
	print_graph(g);
	print_edges_w(g);

	// Apply prim
	vector<GraphTp2::vertex_descriptor> pred(num_vertices(g));
	//GraphTp2::vertex_descriptor arr[6];
	prim_minimum_spanning_tree(g, &(pred[0])); //must pass a T*
	print_predecessor(pred);

	vector<GraphTp2::edge_descriptor> res;
	kruskal_minimum_spanning_tree(g, back_inserter(res));
	foreach_(auto e, res)
	{
		cout << source(e, g) << " - " << target(e, g) << "\n";
	}
	res.clear();

	//5000 nodes
	generateCompleteGraph(g, 5000);
	pred.resize(num_vertices(g));
	chrono::system_clock sc;
	auto before = sc.now();
	prim_minimum_spanning_tree(g, &(pred[0]));
	cout << "Time used by prim :" << chrono::duration_cast<chrono::seconds>(sc.now() - before).count() << "s\n";

	res.clear();
	//ostream_iterator<GraphTp2::edge_descriptor> it(cout, "\n");
	before = sc.now();
	kruskal_minimum_spanning_tree(g, back_inserter(res));
	cout << "Time used by kruskal :" << chrono::duration_cast<chrono::seconds>(sc.now() - before).count() << "s\n";
}


struct EdgeProperties{
	int weight;
	//int distance;
};
struct VertexProperties{
	int dist;
};

void tp2_ex2(){
	// Ex2
	typedef adjacency_list<vecS, vecS, directedS, VertexProperties, EdgeProperties > GraphTp2Ex2;
	//auto gen = bind(uniform_int_distribution<int>(0, numeric_limits<int>::max()), default_random_engine(time(NULL)));
	//auto gen = uniform_int_distribution<int>() ;
	GraphTp2Ex2 g;
	generateDirectedGraph(g, &EdgeProperties::weight, 5000, 10000);
	cout << num_vertices(g) << endl;
	//print_graph(g);
	// Dij
	vector<GraphTp2Ex2::vertex_descriptor> pred(5000);
	pred[0] = 0;
	//dijkstra_shortest_paths(g, 0, predecessor_map(&(pred[0])));
	//MESURE_TIME(
	//!!!!!!!!!!!!! Distance_map is about a property of VERTEX not EDGE!!
	dijkstra_shortest_paths(g, 0, predecessor_map(make_iterator_property_map(pred.begin(), get(vertex_index, g)))
		.weight_map(get(&EdgeProperties::weight, g))
		.distance_map(get(&VertexProperties::dist,g))
		);
	//);
	property_map<GraphTp2Ex2, int VertexProperties::*>::type dist = get(&VertexProperties::dist, g);
	cout << "Dijkstra Distance 0-250 : " << dist[vertex(250, g)]<<endl;

	GraphTp2Ex2::vertex_descriptor root = vertex(0, g);
	//TODO
	bellman_ford_shortest_paths(g, root_vertex(root)
		.predecessor_map(&pred[0])
		.weight_map(get(&EdgeProperties::weight, g))
		.distance_map(get(&VertexProperties::dist, g)));

	cout << "Bellman-Ford Distance 0-250 : " << dist[vertex(250, g)] << endl;

	vector<vector<int> > distMatrix((num_vertices(g)), vector<int>(num_vertices(g)));
	floyd_warshall_all_pairs_shortest_paths(g, distMatrix,
		weight_map(get(&EdgeProperties::weight, g))
	);

	cout << "Floyd-Warshall Distance 0-250 : " << distMatrix[0][250] << endl;

}
