#pragma once

#include <boost/dynamic_bitset.hpp>
#include <map>
#include <utility>
#include <vector>

namespace ConstraintGraphs
{
  typedef boost::dynamic_bitset<> bitset;

  class ConstraintGraph
  {
    public:
      // Create a new constraint graph supporting the specified number of
      // nodes.
      ConstraintGraph(const unsigned int n_nodes);

      // Add a constraint to the graph, `a' and `b' should be nodes in the
      // graph.
      void AddConstraint(const unsigned int a, const unsigned int b);

      // Test for the presence of an edge
      bool ContainsConstraint(const unsigned int a,
                              const unsigned int b) const;

      // Colour the nodes in the graph and write the colouring out into the
      // specified array (which should be of length `n_nodes`).
      void ColourGraph(unsigned int* const colouring);

    private:
      // For each node we store an adjacency list representation of the nodes
      // which with it shares edges. To save space these adjacency lists are
      // stored as bitsets.
      unsigned int m_n_nodes;  // Number of supported nodes.
      std::vector<bitset > m_edges;
  };

  typedef std::pair<unsigned int, unsigned int> Chip;
  typedef unsigned int Route;

  class MulticastKeyConstraintGraph : public ConstraintGraph
  {
    public:
      MulticastKeyConstraintGraph(const unsigned int n_nodes);

      // Add a new route entry to the graph, this will result in further
      // constraints being added to the graph.
      void AddRoute(const unsigned int net,
                    const unsigned int x,  // x-coordinate of the route
                    const unsigned int y,  // y-coordinate of the route
                    const Route route);

    private:
      // For each chip which has routing entries we store a dictionary mapping
      // each unique set of routes to the nets which take that route.
      std::map<Chip, std::map<Route, std::vector<unsigned int > > > m_routes;
      // {chip: {route: [net, ...], ...}, ...}
  };
}


/*****************************************************************************/
// Methods provided for Python use of the above objects.
using namespace ConstraintGraphs;
extern "C"
{
  // Create a new constraint graph
  extern MulticastKeyConstraintGraph* cffi_new_graph(
      const unsigned int n_nodes);

  // Delete an existing constraint graph
  extern void cffi_delete_graph(MulticastKeyConstraintGraph* graph);

  // Add a constraint to a graph
  extern void cffi_add_graph_constraint(
      MulticastKeyConstraintGraph* graph,
      const unsigned int a,
      const unsigned int b
  );

  // Add a route to the graph
  extern void cffi_add_route_to_graph(
      MulticastKeyConstraintGraph* graph,
      const unsigned int net,
      const unsigned int x,
      const unsigned int y,
      const unsigned int route
  );

  // Colour the graph, writing the result into the provided array
  extern void cffi_colour_graph(
      MulticastKeyConstraintGraph* graph,
      unsigned int* colouring
  );
}
/*****************************************************************************/
