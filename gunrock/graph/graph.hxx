#pragma once

#include <gunrock/formats/formats.hxx>

namespace gunrock {
namespace graph {

struct graph_properties_t {
    bool directed {false};
    bool weighted {false};
    graph_properties_t() = default;
};

template <typename vertex_t, typename edge_t, typename weight_t>
struct graph_base_t {

    typedef vertex_t            vertex_type;
    typedef edge_t              edge_type;
    typedef weight_t            weight_type;
    typedef graph_properties_t  properties_type;

    typedef graph_base_t<vertex_type, edge_type, weight_type> graph_base_type;

    graph_base_t() : 
        _number_of_vertices(0), 
        _number_of_edges(0), 
        _properties() {}

    graph_base_t(vertex_type number_of_vertices, edge_type number_of_edges) :
        _number_of_vertices(number_of_vertices),
        _number_of_edges(number_of_edges),
        _properties() {}

    graph_base_t(vertex_type number_of_vertices, edge_type number_of_edges, properties_type properties) :
        _number_of_vertices(number_of_vertices),
        _number_of_edges(number_of_edges),
        _properties(properties) {}

    vertex_type number_of_vertices() { return _number_of_vertices; }
    edge_type number_of_edges() { return _number_of_edges; }
    bool is_directed() { return _directed; }

    // Pure Virtual Functions:: must be implemented in derived classes
    __host__ __device__ __forceinline__
    virtual edge_type get_neighbor_list_length(const vertex_type& v) const = 0;
    
    __host__ __device__ __forceinline__
    virtual vertex_type get_source_vertex(const edge_type& e) const = 0
    
    __host__ __device__ __forceinline__
    virtual vertex_type get_destination_vertex(const edge_type& e) const = 0;
    
    __host__ __device__ __forceinline__
    virtual auto get_source_and_destination_vertices(const edge_type& e) const = 0; // XXX: return type?
    
    __host__ __device__ __forceinline__
    virtual edge_type get_edge(const vertex_type& source, const vertex_type& destination) const = 0;

    protected:
        vertex_type     _number_of_vertices;
        edge_type       _number_of_edges;
        properties_type _properties;


}; // struct graph_base_t

template <typename vertex_t, typename edge_t, typename weight_t> 
struct graph_csr_t : public graph_base_t<vertex_t, edge_t, weight_t> {
    
    graph_csr_t() : graph_base_t<vertex_t, edge_t, weight_t>() {}

    graph_csr_t(edge_t* offsets, 
                vertex_t* indices, 
                weight_t* weights, 
                vertex_t number_of_vertices, 
                edge_t number_of_edges) : 
        graph_base_t<vertex_t, edge_t, weight_t>(
            number_of_vertices, 
            number_of_edges),
        csr(
            number_of_vertices, 
            number_of_vertices, 
            number_of_edges, 
            offsets, 
            indices, 
            weights) {}
    
    // Override pure virtual functions
    // Must use [override] keyword to identify functions that are
    // overriding the derived class
    __host__ __device__ __forceinline__
    edge_t get_neighbor_list_length(const vertex_t& v) const override {
        assert(v < _number_of_vertices);
        return (csr.row_offsets[v+1] - csr.row_offsets[v]);
    }

    __host__ __device__ __forceinline__
    vertex_t get_source_vertex(const edge_t& e) const override {
        assert(e < _number_of_edges);
        return (algo::search::binary::rightmost(csr.row_offsets.get(), e, _number_of_vertices));
    }
    
    __host__ __device__ __forceinline__
    vertex_t get_destination_vertex(const edge_t& e) const override { 

    }
    __host__ __device__ __forceinline__
    auto get_source_and_destination_vertices(const edge_t& e) const override {

    }
    
    __host__ __device__ __forceinline__
    edge_t get_edge(const vertex_t& source, const vertex_t& destination) const override {

    }

    // Representation specific functions
    // ...

    private:
        csr_t<edge_t, vertex_t, weight_t> csr;
};  // struct graph_csr_t

template <typename vertex_t, typename edge_t, typename weight_t> 
struct graph_csc_t : public graph_base_t<vertex_t, edge_t, weight_t> {
    
    graph_csc_t() : graph_base_t<vertex_t, edge_t, weight_t>() {}

    graph_csc_t(edge_t* offsets, 
                vertex_t* indices, 
                weight_t* weights, 
                vertex_t number_of_vertices, 
                edge_t number_of_edges) : 
        graph_base_t<vertex_t, edge_t, weight_t>(
            number_of_vertices, 
            number_of_edges),
        csc(
            number_of_vertices, 
            number_of_vertices, 
            number_of_edges, 
            offsets, 
            indices, 
            weights) {}
    
    // Override pure virtual functions
    // Must use [override] keyword to identify functions that are
    // overriding the derived class
    __host__ __device__ __forceinline__
    edge_t get_neighbor_list_length(const vertex_t& v) const override {

    }

    __host__ __device__ __forceinline__
    vertex_t get_source_vertex(const edge_t& e) const override {

    }
    
    __host__ __device__ __forceinline__
    vertex_t get_destination_vertex(const edge_t& e) const override { 

    }
    __host__ __device__ __forceinline__
    auto get_source_and_destination_vertices(const edge_t& e) const override {

    }
    
    __host__ __device__ __forceinline__
    edge_t get_edge(const vertex_t& source, const vertex_t& destination) const override {

    }

    // Representation specific functions
    // ...

    private:
        csc_t<edge_t, vertex_t, weight_t> csc;
};  // graph_csc_t

template <typename vertex_t, typename edge_t, typename weight_t> 
struct graph_coo_t : public graph_base_t<vertex_t, edge_t, weight_t> {
    
    graph_coo_t() : graph_base_t<vertex_t, edge_t, weight_t>() {}

    graph_coo_t(edge_t* row_indices,
                edge_t* column_indices, 
                weight_t* weights,
                vertex_t number_of_vertices, // XXX: coo_t only accepts two types
                edge_t number_of_edges) : 
        graph_base_t<vertex_t, edge_t, weight_t>(
            number_of_vertices, 
            number_of_edges),
        coo(number_of_vertices, 
            number_of_vertices, 
            number_of_edges, 
            row_indices, 
            column_indices, 
            weights) {}
    
    // Override pure virtual functions
    // Must use [override] keyword to identify functions that are
    // overriding the derived class
    __host__ __device__ __forceinline__
    edge_t get_neighbor_list_length(const vertex_t& v) const override {

    }

    __host__ __device__ __forceinline__
    vertex_t get_source_vertex(const edge_t& e) const override {

    }
    
    __host__ __device__ __forceinline__
    vertex_t get_destination_vertex(const edge_t& e) const override { 

    }
    __host__ __device__ __forceinline__
    auto get_source_and_destination_vertices(const edge_t& e) const override {

    }
    
    __host__ __device__ __forceinline__
    edge_t get_edge(const vertex_t& source, const vertex_t& destination) const override {

    }

    // Representation specific functions
    // ...

    private:
        coo_t<edge_t, weight_t> coo;
};  // graph_coo_t

template <typename vertex_t, typename edge_t, typename weight_t> 
struct graph_coo_t : 
    public graph_csr_t<vertex_t, edge_t, weight_t>,
    public graph_csc_t<vertex_t, edge_t, weight_t>,
    public graph_coo_t<vertex_t, edge_t, weight_t> {

} // namespace graph
} // namespace gunrock