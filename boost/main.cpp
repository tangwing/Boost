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



#include <functional>
#include <vector>
#include <iostream>
#include <fstream>
#include <typeinfo>
#include <type_traits>
using namespace  std;
using namespace boost;
using namespace boost::graph;
using namespace boost::adaptors;

#pragma warning( disable : 4996 )

#define foreach_ BOOST_FOREACH 
typedef pair<int, int> Edge_raw;
typedef adjacency_list<vecS, vecS, directedS, property<vertex_name_t, string>, property<edge_weight_t, int> > Graph;
typedef Graph::vertex_descriptor Vertex;
typedef Graph::edge_descriptor Edge;
Graph::vertex_iterator vb, ve;
Graph::edge_iterator eb, ee;
enum { A , B, C, D, E, N };
vector<Edge_raw> edge_array = { Edge_raw(A, B), Edge_raw(A, C), Edge_raw(A, D), Edge_raw(A, E),
Edge_raw(C, A), Edge_raw(C, E), Edge_raw(D, A), Edge_raw(D, B), Edge_raw(D, E), Edge_raw(E, A) };
int m = sizeof(edge_array) / sizeof(Edge_raw);


void testForeach();
void testGraph();
void testVisitor();
void testTopo();
void testGen();
void tp2();
void tp3();
void tp4();

int main(size_t argn, char** args)
{

    
    //tp2();
    tp3();
    //testForeach();
    //testGraph();
    //testVisitor();
    //testTopo();
    //testGen();
    return 0;
}
void log(const char * msg){ cout << msg; }
void log(any msg){ cout << any_cast<string>(msg); }

#include<random>
template <typename Graph>
void generateCompleteGraph(Graph & g ,int N, int edgeWeightMin = 2, int edgeWeightMax = 10)
{
    BOOST_CONCEPT_ASSERT((MutableGraphConcept<Graph>));
    auto rand = bind(uniform_int_distribution<int>(edgeWeightMin, edgeWeightMax), default_random_engine(time(nullptr)));
    g.clear();
    for (int i = 0; i < N; ++i) add_vertex(g);
    property_map<Graph, edge_weight_t>::type edgeWeightMap = get(edge_weight, g);
    for (auto itv = vertices(g); itv.first != itv.second; ++itv.first)
    {
        for (auto itv2 =std::next(itv.first); itv2 != itv.second; ++itv2)
        {
            add_edge(*(itv.first), *itv2, rand(), g);
        }
    }
}
template <typename T>
void print_predecessor(const vector<T> & pred)
{
    for (int i = 0; i < pred.size(); ++i)
    {
        cout << "Parent(" << i << ")=" << pred[i]<<"\n";
    }
}
template<typename Graph>
void print_edges_w(Graph g)
{
    auto wp = get(edge_weight, g);
    for (auto itv = edges(g); itv.first != itv.second; ++itv.first)
    {
        cout << source(*itv.first,g) << " -- " << target(*itv.first,g) << " : " << wp[*itv.first] << "\n";
    }
}

#include<chrono>
void tp2()
{
    typedef adjacency_list<vecS, vecS, undirectedS, no_property, property<edge_weight_t, int> > GraphTp2;
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

    before = sc.now();
    kruskal_minimum_spanning_tree(g, back_inserter(res));
    cout << "Time used by kruskal :" << chrono::duration_cast<chrono::seconds>(sc.now() - before).count() << "s\n";
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
        int counter=0;
        while (it != eof)
        {
            ++counter;
            vector<T> line;
            for (int i = 0; i < nbFieldPerLine; ++i)
            {
                if (it == eof) cerr << "Error at line " << counter<<"\n";
                line.push_back(*it++);
            }
            dataGrid.push_back(line);
        }
    }
    return dataGrid;
}

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
        weight_map(get(&EdgeProperty::danger, g)).
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
    auto curr = idIndexMap[t];
    vector<GraphTp3::vertex_descriptor> path;
    while ( true)
    {
        path.push_back(curr);
        if (pred[curr] == curr) break;
        curr = pred[curr];
    }
    ofstream o("pcc.txt");
    if (!o)cerr << "Error writing file!\n";
    transform(path.rbegin(), path.rend()-1, ostream_iterator<int>(o,"\n"), [&g](GraphTp3::vertex_descriptor v)
    {
        return g[v].id;
    });
    o << g[path.front()].id;

}

void tp4()
{
    
}

void testForeach()
{
    std::vector<int> vec = { 1, 2, 3, 3, 4, 5, 6, 6 };
    BOOST_FOREACH(auto i, (vec | adjacent_filtered(not_equal_to<int>()))) cout << i;
}

template <typename NameMap>
class MyVisitor : public default_bfs_visitor
{
    NameMap nameMap;
public:
    MyVisitor(NameMap names):nameMap(names){}
    template<typename Vertex, typename Graph>
    void discover_vertex(Vertex v, const Graph & g) const
    {
        cout << "Discovering " << nameMap[v] << endl;
    }
};

void testVisitor()
{
    // Create graph
    Graph g;
    //typedef property_map<Graph, vertex_name_t>::type NameMap;
    foreach_(auto & e, edge_array){
        add_edge(e.first, e.second, g);
    }

    for (tie(vb, ve) = vertices(g); vb != ve; ++vb)
        put(vertex_name,g,*vb, to_string(char('A' + *vb)));

    //add_edges(edge_array.begin(), edge_array.end(), g);
    print_graph(g, get(vertex_name, g));
    // Create visitor
    //auto nameMap = get(vertex_name, g);
    //MyVisitor< decltype(nameMap) > vis(nameMap);
    //breadth_first_search(g, visitor(vis));
    // Pass visitor to traversing function (dfs)
}

void testGraph()
{
    log( "Creating a star graph 0->1,2,3,4,5\n");
    Graph g;
    add_edge(0, 1, g);
    add_edge(0, 2, g);
    add_edge(0, 3, g);
    add_edge(0, 4, g);
    add_edge(0, 5, g);
    
    property_map<Graph, vertex_index_t>::type indexMap = get(vertex_index, g);
    
    log("Print incidence edges of 0\n");
    auto v0 = *vertices(g).first;
    foreach_(auto e, out_edges(v0, g))
        cout << source(e, g) << "->" << target(e, g) << endl;

    log("Print adjacent vertices of 0:\n");
    //log("Adjacent vertices are ");
    foreach_(auto v, adjacent_vertices(v0, g))
        log(to_string(indexMap[v])+"\n");

    log("Get vertex by index\n");
    // The type of the map structure which stores vertex_index for Graph class.
    Graph::vertex_descriptor ver = vertex(0, g);
    log("The index of ver is ");
    log(int(indexMap[ver]));
    log("\n");

    //BOOST_CONCEPT_ASSERT((concepts::MutableIncidenceGraph<Graph>));



    //property_map<Graph, edge_attribute_t>::type attrEdgeMap = get(edge_, g);
    // Access vertice/edges
    //for (auto v = vertices(g); v.first != v.second; ++v.first)
    {
        //cout << get(indexMap, *v.first) << "->";
        //graph_traits<Graph>::adjacency_iterator b, e;
        //for (tie(b, e) = adjacent_vertices(*v.first, g); b != e; ++b)
        //{
        //    cout << get(indexMap, *b)<<" ";
        //}
        //Graph::out_edge_iterator b, e;
        //for (std::tie(b, e) = out_edges(*v.first, g); b != e; ++b)
        //{
        //    cout << target(*b, g) <<"\t";
        //}
        //cout << endl;
    }
}

template<typename NameMap>
class graphviz_vertex_name_writer : default_writer
{
    NameMap names_;
    Graph & g_;
public:
    graphviz_vertex_name_writer(NameMap names, Graph & g) :names_(names), g_(g){}
    template <class VorE>
    void operator()(std::ostream& o, const VorE& v) const {
        o << "[label=" << names_[get(vertex_index, g_)[v]]<<"]";
    }
};

void testTopo()
{
    // ItA figure 22.7 p613
    enum{ socks, undershorts, pants, shoes, watch, shirt, belt, tie, jacket ,N};
    string names[] = { "socks", "undershorts", "pants", "shoes", "watch", "shirt", "belt", "tie","jacket "};
    Edge_raw edges[] = {Edge_raw(socks, shoes), Edge_raw(undershorts, shoes), Edge_raw(undershorts, pants), Edge_raw(pants, shoes), Edge_raw(pants, belt), Edge_raw(shirt, belt), Edge_raw(shirt, tie), Edge_raw(belt, jacket), Edge_raw(tie, jacket)};
    //typedef adjacency_list<vecS, vecS, directedS> Graph;
    typedef Graph::vertex_descriptor Vertex;
    typedef Graph::edge_descriptor Edge;

    Graph g(&edges[0], &edges[0] + N, N);
    vector<Vertex> res(N);
    topological_sort(g, res.rbegin());
    copy(res.begin(), res.end(), ostream_iterator<Vertex>(cout, " "));
    print_graph(g, names);

    ofstream graph("graph.gv", ios::out);
    write_graphviz(graph, g, graphviz_vertex_name_writer<string*>(names, g));
    system("dot -Tpng -ograph.png graph.gv | explorer graph.png");
}

void testGen()
{
    Graph g;
    boost::random::mt19937 gen(time(NULL));
    
    generate_random_graph(g, 10, 20, gen);
    randomize_property<vertex_name_t, Graph>(g, gen);
    print_graph(g, get(vertex_name, g));
    ofstream graph("graph.gv", ios::out);
    //write_graphviz(graph, g, graphviz_vertex_name_writer<property_map<Graph, vertex_name_t>::type >(get(vertex_name, g), g));
    write_graphviz(graph, g, default_writer());
    system("dot -Tpng -ograph.png graph.gv | explorer graph.png");
}

void exampleSTL()
{
    // Iterator
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);

    // begin()->iterator qui pointe sur le premier element
    // end()  ->iterator qui pointe sur APRES le dernier element
    // ++it   ->faire avance l'iterator
    // *it    ->accéder à l'élément pointé
    for (vector<int>::iterator it = vec.begin(); it != vec.end(); ++it)
        cout << *it;


    {
        // #include <algorithm>
        // #include <functor>
        vector<int> vec1 = { 1, 2, 3, 4 };
        vector<int> vec2 = { 5, 6, 7, 8 };
        list<int> lis(4);                   //size=4
        // Appliquer l'opération d'addition sur les pairs d'éléments de vec1 et vec2 
        // puis enregistrer les résultats dans lis
        transform(vec1.begin(), vec1.end(), 
                  vec2.begin(), lis.begin(), plus<int>());
        // Appliquer une fonction quelconque sur chacun des éléments dans lis
        for_each(lis.begin(), lis.end(), [](int i)
        {
            cout << i;  // 6 8 10 12
        });
    }
}