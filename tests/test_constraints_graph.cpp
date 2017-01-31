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


TEST(MulticastKeyConstraintsGraphTest, test_colour_graph)
{
  /* We construct a graph consisting of four nets, a pair of these nets take a
   * different turn at one chip. One of these nets shares an additional
   * constraint with another net. Overall three colours should be required to
   * colour the graph.
   */
  auto g = MulticastKeyConstraintGraph(4);  // Create the graph

  // Create net 0
  g.AddRoute(0, 0, 0, 0x1);  // East at chip (0, 0)
  g.AddRoute(0, 1, 0, 0x4);  // North at chip (1, 0)

  // Create net 1
  g.AddRoute(1, 0, 0, 0x1);  // East at chip (0, 0)
  g.AddRoute(1, 1, 0, 0x2);  // North east at chip (1, 0)

  // Create net 2
  g.AddRoute(2, 0, 1, 0x1);  // East at chip (0, 1)
  g.AddRoute(2, 1, 1, 0x2);  // North east at chip (1, 1)

  // Create net 3
  g.AddRoute(3, 0, 2, 0x1);  // East at chip (0, 2)
  g.AddRoute(3, 1, 2, 0x2);  // North east at chip (1, 2)

  // Add an additional constraint which stops nets 0 and 2 sharing an
  // identifier.
  g.AddConstraint(0, 2);

  /* The graph should now be equal to:
   *
   *   (1)    (3)
   *    |
   *    |
   *   (0) -- (2)
   */
  unsigned int colouring[4];
  g.ColourGraph(colouring);

  // Check that the colouring was valid
  ASSERT_NE(colouring[0], colouring[1]);
  ASSERT_NE(colouring[0], colouring[2]);
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
