#include <assert.h>

#include <mlpack.hpp>

#include "ants.h"
#include "thop.h"
#include "trees.h"

Tree_Edge::Tree_Edge(const std::size_t &num_city, const std::size_t &current_city, Building_Node *building_root_ptr)
{
    // Top-down
    _root_ptr = new Node();
    _leaf_ptrs.resize(num_city);
    _leaf_ptrs[0] = nullptr; // Not allowed to revisit the starting city
    _build_childs(_root_ptr, building_root_ptr, current_city);
}

void Tree_Edge::_build_childs(Node *&parent_ptr, Building_Node *building_parent_ptr, const std::size_t &current_city)
{
    std::size_t i;
    bool is_leaf;
    Building_Leaf *building_leaf_ptr;
    Building_Node *building_child_ptr;
    double heuristic;

    for (i = 0; i < 2; i++)
    {
        building_child_ptr = building_parent_ptr->child_ptrs[i];
        heuristic = building_child_ptr->make_heuristic(current_city);
        is_leaf = building_child_ptr->child_ptrs[0] == nullptr;

        if (is_leaf)
        {
            const std::size_t city_index = building_leaf_ptr->get_city_index();

            building_leaf_ptr = (Building_Leaf *)(building_child_ptr, city_index);
            parent_ptr->child_ptrs[i] = new Leaf(parent_ptr, heuristic, city_index);
            _leaf_ptrs[city_index] = (Leaf *)(parent_ptr->child_ptrs[i]);
            continue;
        }

        parent_ptr->child_ptrs[i] = new Node(parent_ptr, heuristic);
        _build_childs(parent_ptr->child_ptrs[i], building_child_ptr, current_city);
    }
}

Tree_Edge::Tree_Edge(const std::size_t &num_city, const std::size_t &current_city)
{
    // Bottom-up
    std::size_t i;
    std::vector<Node *> node_ptrs;

    _leaf_ptrs.resize(num_city);
    _leaf_ptrs[0] = nullptr; // Not allowed to revisit the starting city
    // _leaf_ptrs[current_city] = nullptr; // Not allowed to stand still
    for (i = 1; i < num_city; i++)
    {
        _leaf_ptrs[i] = new Leaf(i, HEURISTIC(current_city, i));
        node_ptrs.push_back(_leaf_ptrs[i]);
    }
    _bottom_up_build_tree(node_ptrs);
}

void Tree_Edge::_bottom_up_build_tree(std::vector<Node *> &node_ptrs)
{
    if (node_ptrs.size() == 2)
    {
        _root_ptr = new Node(node_ptrs[0], node_ptrs[1], true);
        return;
    }

    std::size_t i;
    std::vector<Node *> parent_ptrs;

    for (i = 0; i + 1 < node_ptrs.size(); i += 2)
        parent_ptrs.push_back(new Node(node_ptrs[i], node_ptrs[i + 1]));
    if (i == node_ptrs.size() - 1)
        parent_ptrs.push_back(node_ptrs[i]);

    node_ptrs.resize(0);
    node_ptrs.shrink_to_fit();
    _bottom_up_build_tree(parent_ptrs);
}

std::size_t Tree_Edge::choose_next_city(
    Wont_Visit_Node *wont_visit_root_ptr,
    const double &one_minus_q_0,
    const double &alpha,
    const double &beta,
    const double &one_minus_rho,
    const double &past_trail_restart,
    const double &past_trail_min,
    const std::size_t &global_restart_times,
    const std::size_t &global_evap_times,
    const std::size_t &global_wont_visit_restart_times)
{
    Node *current_ptr = _root_ptr;
    Wont_Visit_Node *current_wont_visit_ptr = wont_visit_root_ptr;
    std::size_t next_child_index;
    bool left_wont_visit, right_wont_visit;

    while (current_ptr->child_ptrs[0] != nullptr)
    {
        left_wont_visit = current_wont_visit_ptr->child_ptrs[0]->get_wont_visit(global_wont_visit_restart_times);
        right_wont_visit = current_wont_visit_ptr->child_ptrs[1]->get_wont_visit(global_wont_visit_restart_times);

        if (left_wont_visit && !right_wont_visit)
            next_child_index = 1;
        else if (!left_wont_visit && right_wont_visit)
            next_child_index = 0;
        else if (!left_wont_visit && !right_wont_visit)
            next_child_index = current_ptr->choose_child_with_prob(
                one_minus_q_0,
                alpha, beta, one_minus_rho, past_trail_restart, past_trail_min,
                global_restart_times, global_evap_times);
        else
            assert(false);

        current_ptr = current_ptr->child_ptrs[next_child_index];
        current_wont_visit_ptr = current_wont_visit_ptr->child_ptrs[next_child_index];
    }

    return ((Leaf *)current_ptr)->get_city_index();
}

void Tree_Edge::reinforce(
    const std::size_t &city_index,
    const double &invert_fitness,
    const double &one_minus_rho,
    const double &past_trail_restart,
    const double &past_trail_min,
    const double &trail_max,
    const std::size_t &global_restart_times,
    const std::size_t &global_evap_times)
{
    Node *node_ptr = _leaf_ptrs[city_index];
    while (node_ptr->parent_ptr != nullptr)
    {
        node_ptr->reinforce(
            invert_fitness,
            one_minus_rho,
            past_trail_restart,
            past_trail_min,
            trail_max,
            global_restart_times,
            global_evap_times);
        node_ptr = node_ptr->parent_ptr;
    }
}

double Tree_Edge::leaf_pheromone(
    const std::size_t &city_index,
    const double &one_minus_rho,
    const double &past_trail_restart,
    const double &past_trail_min,
    const std::size_t &global_restart_times,
    const std::size_t &global_evap_times)
{
    return _leaf_ptrs[city_index]->get_pheromone(
        one_minus_rho,
        past_trail_restart,
        past_trail_min,
        global_restart_times,
        global_evap_times);
}

Wont_Visit_Tree::Wont_Visit_Tree(const std::size_t &num_city, Building_Node *building_root_ptr)
{
    // Top-down
    _root_ptr = new Wont_Visit_Node(nullptr);
    _leaf_ptrs.resize(num_city);
    _leaf_ptrs[0] = nullptr; // Not allowed to revisit the starting city
    _build_childs(_root_ptr, building_root_ptr);
}

void Wont_Visit_Tree::_build_childs(Wont_Visit_Node *&parent_ptr, Building_Node *building_parent_ptr)
{
    std::size_t i;
    bool is_leaf;
    Building_Leaf *building_leaf_ptr;
    Building_Node *building_child_ptr;

    for (i = 0; i < 2; i++)
    {
        building_child_ptr = building_parent_ptr->child_ptrs[i];
        is_leaf = building_child_ptr->child_ptrs[0] == nullptr;

        if (is_leaf)
        {
            building_leaf_ptr = (Building_Leaf *)(building_child_ptr);
            parent_ptr->child_ptrs[i] = new Wont_Visit_Node(parent_ptr, true);
            _leaf_ptrs[building_leaf_ptr->get_city_index()] = parent_ptr->child_ptrs[i];
            continue;
        }

        parent_ptr->child_ptrs[i] = new Wont_Visit_Node(parent_ptr);
        _build_childs(parent_ptr->child_ptrs[i], building_child_ptr);
    }
}

Wont_Visit_Tree::Wont_Visit_Tree(const std::size_t &num_city)
{
    // Bottom-up
    std::size_t i;
    std::vector<Wont_Visit_Node *> node_ptrs;

    _leaf_ptrs.resize(num_city);
    _leaf_ptrs[0] = nullptr; // Not allowed to revisit the starting city
    for (i = 1; i < num_city; i++)
    {
        _leaf_ptrs[i] = new Wont_Visit_Node(nullptr, nullptr);
        node_ptrs.push_back(_leaf_ptrs[i]);
    }
    _bottom_up_build_tree(node_ptrs);
}

void Wont_Visit_Tree::_bottom_up_build_tree(std::vector<Wont_Visit_Node *> &node_ptrs)
{
    if (node_ptrs.size() == 2)
    {
        _root_ptr = new Wont_Visit_Node(node_ptrs[0], node_ptrs[1], true);
        return;
    }

    std::size_t i;
    std::vector<Wont_Visit_Node *> parent_ptrs;

    for (i = 0; i + 1 < node_ptrs.size(); i += 2)
        parent_ptrs.push_back(new Wont_Visit_Node(node_ptrs[i], node_ptrs[i + 1]));
    if (i == node_ptrs.size() - 1)
        parent_ptrs.push_back(node_ptrs[i]);

    node_ptrs.resize(0);
    node_ptrs.shrink_to_fit();
    _bottom_up_build_tree(parent_ptrs);
}

void Wont_Visit_Tree::set_wont_visit(const std::size_t &city_index, const std::size_t &num_city, const std::size_t &global_wont_visit_restart_times)
{
    if (city_index != 0 and city_index != num_city - 1)
        _leaf_ptrs[city_index]->set_wont_visit(global_wont_visit_restart_times);
    assert(_root_ptr->get_wont_visit(global_wont_visit_restart_times) == false);
}

Building_Tree::Building_Tree(const problem &instance)
{
    cluster_struct cluster;
    const std::size_t num_city = instance.n - 1;

    _make_first_cluster(instance, cluster);

    _root_ptr = new Building_Node();
    _root_ptr->child_ptrs[0] = new Building_Node(_root_ptr, cluster.centroid_x, cluster.centroid_y);
    _root_ptr->child_ptrs[1] = new Building_Leaf(
        _root_ptr,
        instance.nodeptr[num_city - 1].x,
        instance.nodeptr[num_city - 1].y,
        num_city - 1);

    // No use
    // _leaf_ptrs[0] = nullptr; // Not allowed to revisit the starting city
    // _leaf_ptrs[num_city - 1] = (Building_Leaf *)(_root_ptr->child_ptrs[1]);

    _build_childs(_root_ptr->child_ptrs[0], cluster.indexes, cluster.features);
}

void Building_Tree::_build_childs(Building_Node *parent_ptr, const unknown_classB &city_indexes, const unknown_classA &city_features)
{
    std::vector<cluster_struct> clusters;
    std::size_t i;

    _cluster_cities(
        city_indexes,
        city_features,
        clusters);

    for (i = 0; i < 2; i++)
    {
        if (?? clusters[i]_indexes.len() == 1)
        {
            parent_ptr->child_ptrs[i] = new Building_Leaf(parent_ptr, clusters[i].centroid_x, clusters[i].centroid_y, clusters[i].indexes[0]);
            // No use // _leaf_ptrs[clusters[i].indexes[0]] = (Building_Leaf *)(parent_ptr->child_ptrs[i]);
            continue;
        }

        parent_ptr->child_ptrs[i] = new Building_Node(parent_ptr, clusters[i].centroid_x, clusters[i].centroid_y);
        _build_childs(parent_ptr->child_ptrs[i], clusters[i].indexes, clusters[i].features);
    }
}

void Building_Tree::_make_first_cluster(const problem &instance, cluster_struct &cluster)
{
    const std::size_t num_city = instance.n - 1;
    const std::size_t cluster_size = num_city - 2; // No starting or ending cites
    std::size_t i;

    cluster.centroid_x = 0;
    cluster.centroid_y = 0;
    ? ? cluster.indexes.resize(cluster_size);
    ? ? cluster.features.resize(cluster_size);

    for (i = 1; i < cluster_size + 1; i++)
    {
        cluster.indexes[i - 1] = i;
        cluster.centroid_x += instance.nodeptr[i].x;
        cluster.centroid_y += instance.nodeptr[i].y;
        ? ? cluster.features[i] = (instance.nodeptr[i].x, instance.nodeptr[i].y);
    }

    cluster.centroid_x /= cluster_size;
    cluster.centroid_y /= cluster_size;
}