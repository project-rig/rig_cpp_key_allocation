#include "constraints_graph.h"
#include <deque>

using namespace ConstraintGraphs;


/*****************************************************************************/
extern "C"
{
  // Create a new constraint graph
  extern MulticastKeyConstraintGraph* cffi_new_graph(
      const unsigned int n_nodes
  )
  {
    return new MulticastKeyConstraintGraph(n_nodes);
  }

  // Delete an existing constraint graph
  extern void cffi_delete_graph(MulticastKeyConstraintGraph* graph)
  {
    delete graph;
  }

  // Add a constraint to a graph
  extern void cffi_add_graph_constraint(
      MulticastKeyConstraintGraph* graph,
      const unsigned int a,
      const unsigned int b
  )
  {
    graph->AddConstraint(a, b);
  }

  // Add a route to the graph
  extern void cffi_add_route_to_graph(
      MulticastKeyConstraintGraph* graph,
      const unsigned int net,
      const unsigned int x,
      const unsigned int y,
      const uint32_t route
  )
  {
    graph->AddRoute(net, x, y, route);
  }

  // Colour the graph, writing the result into the provided array
  extern void cffi_colour_graph(
      MulticastKeyConstraintGraph* graph,
      unsigned int* colouring
  )
  {
    graph->ColourGraph(colouring);
  };
}
/*****************************************************************************/


ConstraintGraph::ConstraintGraph(const unsigned int n_nodes)
{
  m_n_nodes = n_nodes;

  // Initialise the edge lists for each of the nodes.
  m_edges = std::vector<bitset>(n_nodes, bitset(n_nodes));
}


// Add a constraint to a constraint graph.
void ConstraintGraph::AddConstraint(const unsigned int a,
                                    const unsigned int b)
{
  // Add `b' to the bit set for `a' and vice-verse provided that `a' and `b'
  // are not the same.
  if (a < m_n_nodes && b < m_n_nodes && a != b)
  {
    m_edges[a].set(b);
    m_edges[b].set(a);
  }
}


// Check for the presence of a constraint in the graph.
bool ConstraintGraph::ContainsConstraint(const unsigned a,
                                         const unsigned b) const
{
  if (a < m_n_nodes && b < m_n_nodes)
  {
    return m_edges[a][b];
  }
  else
  {
    return false;
  }
}

/*****************************************************************************/
// Return the index of the node with the highest degree
unsigned int GetHighestDegreeNode(const bitset& nodes,
                                  const std::vector<bitset>& edges)
{
  unsigned int node = 0;
  unsigned int largest_degree = 0;

  for (auto n = nodes.find_first();
       n < nodes.size();
       n = nodes.find_next(n))
  {
    // Get the degree for this node
    const unsigned int node_degree = edges[n].count();

    // If the degree is largest than the previously recorded largest degree or
    // no degree has yet been set record this as the node with the highest
    // degree.
    if (node_degree > largest_degree || largest_degree == 0)
    {
      node = n;
      largest_degree = node_degree;
    }
  }

  return node;
}

void ConstraintGraph::ColourGraph(unsigned int* const colouring)
{
  // Store a list of sets of nodes which can share a colour
  auto colours = std::vector<bitset>();

  // Maintain a list of nodes which haven't been visited
  auto unvisited = bitset(m_n_nodes);
  unvisited.set();  // All nodes are unvisited

  // While there are still unvisited nodes
  while (unvisited.any())
  {
    auto queue = std::deque<unsigned int>();

    // Add the unvisited node with the greatest degree to the queue.
    queue.push_back(GetHighestDegreeNode(unvisited, m_edges));

    // While elements remain in the queue colour the nodes.
    while (!queue.empty())
    {
      const auto node = queue.front();
      queue.pop_front();

      if (unvisited[node])
      {
        // Mark this node as visited
        unvisited.set(node, false);

        // Find the first legal colour for the node and assign it that colour,
        // if no existing colour would be suitable then add a new colour to the
        // set of colours.
        auto edges = m_edges[node];
        auto coloured = false;  // Indicate that the node was assigned a colour
        for (auto& colour_group : colours)
        {
          if (!colour_group.intersects(edges))
          {
            // Add the node to this colour group and exit the loop.
            colour_group.set(node);
            coloured = true;
            break;
          }
        }

        if (!coloured)
        {
          // If no colour group was found suitable then add a whole new colour
          // group to the set of colours and add the node to this colour.
          colours.push_back(bitset(m_n_nodes));
          colours.back().set(node);
        }

        // Add all unvisited nodes to which this node is connected to the
        // queue.
        auto connected_and_unvisited = edges & unvisited;
        for (auto n = connected_and_unvisited.find_first();
             n < connected_and_unvisited.size();
             n = connected_and_unvisited.find_next(n))
        {
          queue.push_back(n);
        }
      }
    }
  }

  // Now write the colouring into the colouring array
  unsigned int colour = 0;
  for (const auto& nodes : colours)
  {
    // Write the colour into the colouring for each node contained within this
    // group.
    for (auto n = nodes.find_first();
         n < nodes.size();
         n = nodes.find_next(n))
    {
      colouring[n] = colour;
    }
    colour++;  // Pick the next colour
  }
}
/*****************************************************************************/


MulticastKeyConstraintGraph::MulticastKeyConstraintGraph(
    const unsigned int n_nodes) : ConstraintGraph(n_nodes)
{
}


// Add a route to the graph, then add as many constraints as are necessary to
// ensure that multicast keys used by nets taking different routes at this
// point are different.
void MulticastKeyConstraintGraph::AddRoute(const unsigned int net,
                                           const unsigned int x,
                                           const unsigned int y,
                                           const Route route)
{
  auto chip = Chip(x, y);  // Get a reference to the chip
  m_routes[chip][route].push_back(net);  // Add the net

  // Add a constraint to the graph for every other net which takes a different
  // route from this chip.
  for (const auto& item : m_routes[chip])
  {
    if (item.first != route)  // If the routes are different
    {
      for (const auto& other_net : item.second)  // For every net
      {
        AddConstraint(net, other_net);
      }
    }
  }
};
