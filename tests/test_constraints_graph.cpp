#include <gtest/gtest.h>
#include <utility>
#include <vector>
#include "constraints_graph.h"

using namespace ConstraintGraphs;


class ConstraintsGraphTest : public ::testing::Test
{
};


class MulticastKeyConstraintsGraphTest : public ::testing::Test
{
};


class CFFITest : public ::testing::Test
{
};


TEST(ConstraintsGraphTest, test_add_edge)
{
  typedef std::pair<unsigned int, unsigned int> Constraint;
  auto edges = std::vector<Constraint>();
  edges.push_back(Constraint(0, 1));
  edges.push_back(Constraint(1, 3));

  auto not_edges = std::vector<Constraint>();
  not_edges.push_back(Constraint(0, 2));
  not_edges.push_back(Constraint(0, 3));
  not_edges.push_back(Constraint(1, 2));
  not_edges.push_back(Constraint(2, 3));

  // Test adding edges
  auto g = ConstraintGraph(4);
  for (auto& edge : edges)
  {
    g.AddConstraint(edge.first, edge.second);
  }

  // Check for the presence and absence of edges
  for (auto& edge : edges)
  {
    // Present edges
    EXPECT_TRUE(g.ContainsConstraint(edge.first, edge.second));
  }
  for (auto& edge : not_edges)
  {
    // Absent edges
    EXPECT_FALSE(g.ContainsConstraint(edge.first, edge.second));
    EXPECT_FALSE(g.ContainsConstraint(edge.second, edge.first));
  }
}


TEST(ConstraintsGraphTest, test_contains_constraint_out_of_range)
{
  // Check that false is returned for edges that are out of range
  // Create a complete graph
  auto g = ConstraintGraph(4);
  for (unsigned int i = 0; i < 4; i++)
  {
    for (unsigned int j = i + 1; j < 4; j++)
    {
      g.AddConstraint(i, j);
    }
  }

  ASSERT_TRUE(g.ContainsConstraint(0, 1));  // Sanity check

  ASSERT_FALSE(g.ContainsConstraint(0, 4));
  ASSERT_FALSE(g.ContainsConstraint(4, 0));
}


TEST(ConstraintsGraphTest, test_colour_simple_graph)
{
  /* Construct the following graph:
   *
   *   (4)        (2)
   *    |         / \
   *    |        /   \
   *   (0) --- (1)---(3)
   *    \      /
   *     \   /
   *      (5)
   *
   * And check that a valid colouring is produced.
   */
  // Start by adding the constraints to a vector, this makes checking over them
  // later slightly easier.
  typedef std::pair<unsigned int, unsigned int> Constraint;
  auto constraints = std::vector<Constraint>();
  constraints.push_back(Constraint(0, 1));
  constraints.push_back(Constraint(0, 1));
  constraints.push_back(Constraint(0, 4));
  constraints.push_back(Constraint(0, 5));
  constraints.push_back(Constraint(1, 2));
  constraints.push_back(Constraint(1, 3));
  constraints.push_back(Constraint(1, 5));
  constraints.push_back(Constraint(2, 3));

  auto g = ConstraintGraph(6);
  for (auto c : constraints)
  {
    g.AddConstraint(c.first, c.second);
  }

  // Get a colouring of the graph
  unsigned int colouring[6];
  g.ColourGraph(colouring);

  // Check that the colouring is valid.
  for (auto c : constraints)
  {
    // The colours of nodes between which there is a constraint must not be the
    // same.
    ASSERT_NE(colouring[c.first], colouring[c.second]);
  }
}


TEST(ConstraintsGraphTest, test_colour_graph_with_cliques)
{
  /* Construct the following graph:
   *
   *   (4)        (2)
   *    |         / \
   *    |        /   \
   *   (0) --- (1)---(3)    (8)
   *    \      /            /
   *     \   /             /
   *      (5)     (6)---(7)
   *
   * And check that a valid colouring is produced.
   */
  // Start by adding the constraints to a vector, this makes checking over them
  // later slightly easier.
  typedef std::pair<unsigned int, unsigned int> Constraint;
  auto constraints = std::vector<Constraint>();
  constraints.push_back(Constraint(0, 1));
  constraints.push_back(Constraint(0, 1));
  constraints.push_back(Constraint(0, 4));
  constraints.push_back(Constraint(0, 5));
  constraints.push_back(Constraint(1, 2));
  constraints.push_back(Constraint(1, 3));
  constraints.push_back(Constraint(1, 5));
  constraints.push_back(Constraint(2, 3));
  constraints.push_back(Constraint(6, 7));
  constraints.push_back(Constraint(7, 8));

  auto g = ConstraintGraph(9);
  for (auto c : constraints)
  {
    g.AddConstraint(c.first, c.second);
  }

  // Get a colouring of the graph
  unsigned int colouring[9];
  g.ColourGraph(colouring);

  // Check that the colouring is valid.
  for (auto c : constraints)
  {
    // The colours of nodes between which there is a constraint must not be the
    // same.
    ASSERT_NE(colouring[c.first], colouring[c.second]);
  }
}


TEST(MulticastKeyConstraintsGraphTest, test_add_route)
{
  /* Add multiple routes to a graph and check that the appropriate constraints
   * are created.
   */
  auto g = MulticastKeyConstraintGraph(4);  // Create the graph

  // Create net 0
  g.AddRoute(0, 0, 0, 0x1);  // East at chip (0, 0)
  g.AddRoute(0, 1, 0, 0x4);  // North at chip (1, 0)

  // Create net 1
  g.AddRoute(1, 0, 0, 0x1);  // East at chip (0, 0)
  g.AddRoute(1, 1, 0, 0x2);  // North east at chip (1, 0)

  // Create net 2
  g.AddRoute(2, 0, 1, 0x4);  // North at chip (0, 1)
  g.AddRoute(2, 0, 2, 0x2);  // North east at chip (0, 2)

  // Create net 3
  g.AddRoute(3, 0, 2, 0x1);  // East at chip (0, 2)
  g.AddRoute(3, 1, 2, 0x2);  // North east at chip (1, 2)

  // The graph should have edges (0, 1) and (2, 3).
  ASSERT_FALSE(g.ContainsConstraint(0, 0));
  ASSERT_TRUE(g.ContainsConstraint(0, 1));
  ASSERT_FALSE(g.ContainsConstraint(0, 2));
  ASSERT_FALSE(g.ContainsConstraint(0, 3));

  ASSERT_TRUE(g.ContainsConstraint(1, 0));
  ASSERT_FALSE(g.ContainsConstraint(1, 1));
  ASSERT_FALSE(g.ContainsConstraint(1, 2));
  ASSERT_FALSE(g.ContainsConstraint(1, 3));

  ASSERT_FALSE(g.ContainsConstraint(2, 0));
  ASSERT_FALSE(g.ContainsConstraint(2, 1));
  ASSERT_FALSE(g.ContainsConstraint(2, 2));
  ASSERT_TRUE(g.ContainsConstraint(2, 3));

  ASSERT_FALSE(g.ContainsConstraint(3, 0));
  ASSERT_FALSE(g.ContainsConstraint(3, 1));
  ASSERT_TRUE(g.ContainsConstraint(3, 2));
  ASSERT_FALSE(g.ContainsConstraint(3, 3));
}


TEST(CFFITest, test_colour_graph)
{
  // Same as the other test with this name but instead uses the externed
  // functions that will be called by Python.

  /* We construct a graph consisting of four nets, a pair of these nets take a
   * different turn at one chip. One of these nets shares an additional
   * constraint with another net. Overall three colours should be required to
   * colour the graph.
   */
  auto g = cffi_new_graph(4);  // Create the graph

  // Create net 0
  cffi_add_route_to_graph(g, 0, 0, 0, 0x1);  // East at chip (0, 0)
  cffi_add_route_to_graph(g, 0, 1, 0, 0x4);  // North at chip (1, 0)

  // Create net 1
  cffi_add_route_to_graph(g, 1, 0, 0, 0x1);  // East at chip (0, 0)
  cffi_add_route_to_graph(g, 1, 1, 0, 0x2);  // North east at chip (1, 0)

  // Create net 2
  cffi_add_route_to_graph(g, 2, 0, 1, 0x1);  // East at chip (0, 1)
  cffi_add_route_to_graph(g, 2, 1, 1, 0x2);  // North east at chip (1, 1)

  // Create net 3
  cffi_add_route_to_graph(g, 3, 0, 2, 0x1);  // East at chip (0, 2)
  cffi_add_route_to_graph(g, 3, 1, 2, 0x2);  // North east at chip (1, 2)

  // Add an additional constraint which stops nets 0 and 2 sharing an
  // identifier.
  cffi_add_graph_constraint(g, 0, 2);

  /* The graph should now be equal to:
   *
   *   (1)    (3)
   *    |
   *    |
   *   (0) -- (2)
   */
  unsigned int colouring[4];
  cffi_colour_graph(g, colouring);

  // Check that the colouring was valid
  ASSERT_NE(colouring[0], colouring[1]);
  ASSERT_NE(colouring[0], colouring[2]);

  // Clean up the graph
  cffi_delete_graph(g);
}
