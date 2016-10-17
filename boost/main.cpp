#include "header.h"

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
void tp2_ex1();
void tp2_ex2();
void tp2_ex3();
void tp3();
//void tp4();
void tpmaxflow();

int main(size_t argn, char** args)
{

    
    //tp2_ex2();
    //tp3();
	tpmaxflow();

    //testForeach();
    //testGraph();
    //testVisitor();
    //testTopo();
    //testGen();
    return 0;
}
void log(const char * msg){ cout << msg; }
void log(any msg){ cout << any_cast<string>(msg); }




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
	//randomize_property<edge_weight_t>(g, gen);
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