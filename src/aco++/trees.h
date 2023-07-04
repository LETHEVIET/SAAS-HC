#ifndef _TREES_H_
#define _TREES_H_

#include <vector>

#include "nodes.h"

template <class RootT, class LeafT>
class Tree_Base
{
public:
    Tree_Base() {}
    ~Tree_Base() { delete _root_ptr; }

    RootT *get_root_ptr() { return _root_ptr; }

protected:
    RootT *_root_ptr;
    std::vector<LeafT *> _leaf_ptrs;
};

class Tree_Edge : public Tree_Base<Node, Leaf>
{
public:
    Tree_Edge(const std::size_t &num_city, const std::size_t &current_city);                                   // Bottom-up
    Tree_Edge(const std::size_t &num_city, const std::size_t &current_city, Building_Node *building_root_ptr); // Top-down
    ~Tree_Edge(){};

    std::size_t choose_next_city(
        Wont_Visit_Node *wont_visit_root_ptr,
        const double &one_minus_q_0,
        const double &alpha,
        const double &beta,
        const double &one_minus_rho,
        const double &past_trail_restart,
        const double &past_trail_min,
        const std::size_t &global_restart_times,
        const std::size_t &global_evap_times,
        const std::size_t &global_wont_visit_restart_times);
    void reinforce(
        const std::size_t &city_index,
        const double &invert_fitness,
        const double &one_minus_rho,
        const double &past_trail_restart,
        const double &past_trail_min,
        const double &trail_max,
        const std::size_t &global_restart_times,
        const std::size_t &global_evap_times);
    double leaf_pheromone(
        const std::size_t &city_index,
        const double &one_minus_rho,
        const double &past_trail_restart,
        const double &past_trail_min,
        const std::size_t &global_restart_times,
        const std::size_t &global_evap_times);

protected:
    void _bottom_up_build_tree(std::vector<Node *> &node_ptrs);
    void _build_childs(Node *&parent_ptr, Building_Node *building_parent_ptr, const std::size_t &current_city);
};

class Wont_Visit_Tree : public Tree_Base<Wont_Visit_Node, Wont_Visit_Node>
{
public:
    Wont_Visit_Tree(const std::size_t &num_city);                                   // Bottom-up
    Wont_Visit_Tree(const std::size_t &num_city, Building_Node *building_root_ptr); // Top-down
    ~Wont_Visit_Tree(){};

    void set_wont_visit(const std::size_t &city_index, const std::size_t &num_city, const std::size_t &global_wont_visit_restart_times);
    void _build_childs(Wont_Visit_Node *&parent_ptr, Building_Node *building_parent_ptr);

protected:
    void _bottom_up_build_tree(std::vector<Wont_Visit_Node *> &node_ptrs);
};

typedef struct
{
    unknown_classB indexes;
    unknown_classA features;
    double centroid_x;
    double centroid_y;
} cluster_struct;

class Building_Tree : public Tree_Base<Building_Node, Building_Leaf>
{
public:
    Building_Tree(const problem &instance);
    ~Building_Tree(){};

protected:
    void _make_first_cluster(
        const problem &instance,
        // returning values
        cluster_struct &cluster);
    void _cluster_cities(
        const unknown_classB &city_indexes,
        const unknown_classA &city_features,
        // returning values
        std::vector<cluster_struct> &clusters);
    void _build_childs(Building_Node *parent_ptr, const unknown_classB &city_indexes, const unknown_classA &city_features);
};

#endif