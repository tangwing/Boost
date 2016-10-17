#include "header.h"
struct NodeProperty
{
	NodeProperty(){}
	NodeProperty(int i, double lat, double lon) :id(i), latitude(lat), longitude(lon){}
	int id;
	double latitude, longitude;
};

struct EdgeProperty
{
	EdgeProperty(){}
	EdgeProperty(int d, int dan) :dist(d), danger(dan){}
	int dist, danger;
};

void tp3()
{
	typedef adjacency_list<vecS, vecS, directedS, NodeProperty, EdgeProperty> GraphTp3;
	map<int, GraphTp3::vertex_descriptor> idIndexMap;
	GraphTp3 g;

	// Add nodes
	auto nodeData = readAllFromFile<double>("Noeuds.csv", 3);
	cout << "Total nb of entries : " << nodeData.size() << "\n";
	int count = 1;
	for (auto &line : nodeData)
	{
		//++count;        if (count % 15 == 0)break;
		assert(line.size() == 3);
		idIndexMap[int(line[0])] = add_vertex(NodeProperty(int(line[0]), line[1], line[2]), g);
		//cout << "\n";
	}

	// Add edges
	auto edgeData = readAllFromFile<int>("Arcs.csv", 4);
	cout << "Total nb of entries : " << edgeData.size() << "\n";
	for (auto &line : edgeData)
	{
		assert(line.size() == 4);
		add_edge(idIndexMap[int(line[0])], idIndexMap[int(line[1])], EdgeProperty(line[2], line[3]), g);
	}

	cout << "\nPlease enter source and target point id:";
	int s, t;
	cin >> s >> t;
	vector<GraphTp3::vertex_descriptor> pred(num_vertices(g), GraphTp3::null_vertex());
	vector<int>  dist(num_vertices(g));
	dijkstra_shortest_paths(g, idIndexMap[s],
		weight_map(get(&EdgeProperty::danger, g)).		// Here we can change easily the distance metric
		predecessor_map(make_iterator_property_map(pred.begin(), get(vertex_index, g))).
		distance_map(make_iterator_property_map(dist.begin(), get(vertex_index, g))));

	//dijkstra_shortest_paths(g, idIndexMap[s],
	//    weight_map(get(&EdgeProperty::dist, g)).
	//    predecessor_map(make_iterator_property_map(pred.begin(), get(vertex_index, g))).
	//    distance_map(make_iterator_property_map(dist.begin(), get(vertex_index, g))));

	// Print path to file
	if (dist[idIndexMap[t]] == numeric_limits<int>::max())
	{
		cout << "No path found!\n";
		return;
	}
	cout << "Distance from " << s << " to " << t << " is " << dist[idIndexMap[t]] << endl;
	
	//construct path
	auto curr = idIndexMap[t];
	vector<GraphTp3::vertex_descriptor> path;
	while (true)
	{
		path.push_back(curr);
		if (pred[curr] == curr) break;
		curr = pred[curr];
	}
	ofstream o("pcc.txt");
	if (!o)cerr << "Error writing file!\n";
	transform(path.rbegin(), path.rend() - 1, ostream_iterator<int>(o, "\n"), [&g](GraphTp3::vertex_descriptor v)
	{
		return g[v].id;
	});
	o << g[path.front()].id;

}