// ----------------------------------------------------------------------------
// Gunrock -- Fast and Efficient GPU Graph Library
// ----------------------------------------------------------------------------
// This source code is distributed under the terms of LICENSE.TXT
// in the root directory of this source distribution.
// ----------------------------------------------------------------------------

/**
 * @file
 * dynamic_graph_unweighted.cuh
 *
 * @brief DYN (Dynamic) Graph Data Structure
 */

#pragma once

#include <gunrock/graph/dynamic_graph/dynamic_graph.cuh>
#include <gunrock/graph/dynamic_graph/dynamic_graph_base.cuh>

namespace gunrock {
namespace graph {

/**
 * @brief Unweighted dynamic graph data structure which uses
 * a per-vertex data structure based on the graph flags.
 *
 * @tparam VertexT Vertex identifier type.
 * @tparam SizeT Graph size type.
 * @tparam ValueT Associated value type.
 * @tparam GraphFlag graph flag
 */
template <typename VertexT, typename SizeT, typename ValueT, GraphFlag FLAG,
          unsigned int cudaHostRegisterFlag>
struct Dyn<VertexT, SizeT, ValueT, FLAG, cudaHostRegisterFlag, true, false>
    : DynamicGraphBase<VertexT, SizeT, ValueT, FLAG> {
  /**
   * @brief Insert a batch of edges into weighted slab hash graph
   *
   * @param[in] edges Pointer to pairs of edges
   * @param[in] batch_size Size of the inserted batch
   * @param[in] batch_directed Batch contains directed edges (i.e, false means
   * double the batch edges)
   * @param[in] target Location of the edges data
   */
  template <typename PairT>
  cudaError_t InsertEdgesBatch(util::Array1D<SizeT, PairT> edges,
                               SizeT batchSize, bool batch_directed = true,
                               util::Location target = util::DEVICE) {
    return cudaSuccess;
  }

  /**
   * @brief Deletes a batch of edges from a weighted slab hash graph
   *
   * @param[in] edges Pointer to pairs of edges
   * @param[in] batch_size Size of the inserted batch
   * @param[in] target Location of the edges data
   */
  template <typename PairT>
  cudaError_t DeleteEdgesBatch(util::Array1D<SizeT, PairT> &edges,
                               SizeT batchSize,
                               util::Location target = util::DEVICE) {
    if (target != util::DEVICE) {
      edges.Move(util::HOST, util::DEVICE);
    }

    this->dynamicGraph.DeleteEdgesBatch(edges.GetPointer(util::DEVICE),
                                        batchSize, !this->is_directed);

    if (target != util::DEVICE) {
      edges.Release(util::DEVICE);
    }
    return cudaSuccess;
  }

  /**
   * @brief Converts CSR to Dynamic graph
   *
   * @param[in] csr Input Gunrock CSR graph
   * @param[in] target Location of CSR graph
   * @param[in] stream Stream id
   * @param[in] quiet Whether to log conversion steps or not
   */
  template <typename CsrT_in>
  cudaError_t FromCsr(CsrT_in &csr, util::Location target = util::HOST,
                      cudaStream_t stream = 0, bool quiet = false) {
    if (target != util::HOST) {
      csr.Move(util::DEVICE, util::HOST);
    }
    this->dynamicGraph.Allocate();
    this->dynamicGraph.BulkBuildFromCsr(
        csr.row_offsets.GetPointer(util::HOST),
        csr.column_indices.GetPointer(util::HOST), csr.nodes, csr.directed,
        csr.edge_values.GetPointer(util::HOST));
    if (target != util::HOST) {
      csr.Release(util::HOST);
    }
    this->nodes = csr.nodes;
    this->edges = csr.edges;
    return cudaSuccess;
  }

  /**
   * @brief Converts Dynamic graph to CSR
   *
   * @param[out] csr Output Gunrock CSR data structure
   * @param[in] stream Stream id
   * @param[in] quiet Whether to log conversion steps or not
   */
  template <typename CsrT_in>
  cudaError_t ToCsr(CsrT_in &csr, cudaStream_t stream = 0, bool quiet = false) {
    this->dynamicGraph.ToCsr(csr.row_offsets.GetPointer(util::DEVICE),
                             csr.column_indices.GetPointer(util::DEVICE),
                             csr.nodes, csr.edges);
    return cudaSuccess;
  }
};

template <typename VertexT, typename SizeT, typename ValueT, GraphFlag FLAG,
          unsigned int cudaHostRegisterFlag>
struct Dyn<VertexT, SizeT, ValueT, FLAG, cudaHostRegisterFlag, false, false> {
  template <typename PairT>
  cudaError_t InsertEdgesBatch(util::Array1D<SizeT, PairT> edges,
                               SizeT batchSize, bool batch_directed = true,
                               util::Location target = util::DEVICE) {
    return cudaSuccess;
  }

  template <typename CsrT_in>
  cudaError_t FromCsr(CsrT_in &csr,
                      util::Location target = util::LOCATION_DEFAULT,
                      cudaStream_t stream = 0, bool quiet = false) {
    return cudaSuccess;
  }

  template <typename CsrT_in>
  cudaError_t ToCsr(CsrT_in &csr, cudaStream_t stream = 0, bool quiet = false) {
    return cudaSuccess;
  }

  cudaError_t Release(util::Location target = util::LOCATION_ALL) {
    return cudaSuccess;
  }
};

}  // namespace graph
}  // namespace gunrock

// Leave this at the end of the file
// Local Variables:
// mode:c++
// c-file-style: "NVIDIA"
// End:
