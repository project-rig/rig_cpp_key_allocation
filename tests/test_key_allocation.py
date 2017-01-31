import pytest
from rig.place_and_route.routing_tree import RoutingTree
from rig.routing_table import Routes
from six import iteritems

from rig_cpp_key_allocation import assign_multicast_net_ids


@pytest.mark.parametrize(
    "prior_constraints",
    [{},  # No constraints
     {'d': {'e'}, 'e': {'d'}},  # (e) may not share a colour with (d)
     {'d': {'e'}},  # Likewise, but improperly specified
     ]
)
def test_assign_multicast_net_ids(prior_constraints):
    """Test the entire process of assigning appropriate identifiers to
    multicast nets.

    Six nets are constructed:

    (a):
      (0, 1) --> (1, 1) [1]
                   ^
                   |
                   |
      (0, 0) --> (1, 0) [1, 2]


    (b):
      (0, 1) --> (1, 1) [1]


    (c):
      (0, 0) --> (1, 0) [1]


    (d):
                 (1, 1) [1, 2]

    (e): (0, 2) [1]


    (f)

      (0, 2) [1]
        ^
        |
        |
      (0, 1)

    The net graph should be of the form:

        (a) --- (c)
         | \
         |  (f)
         |     \
        (d) --- (b)

        (e)

    If prior constraints are provided then the graph should be of the form:

        (a) --- (c)
         | \
         |  (f)
         |     \
        (d) --- (b)
         |
         |
        (e)
    """
    # Construct the routing trees
    tree_a_11 = RoutingTree((1, 1), [(Routes.core(1), object())])
    tree_a_10 = RoutingTree((1, 0), [(Routes.core(1), object()),
                                     (Routes.core(2), object()),
                                     (Routes.north, tree_a_11)])
    tree_a0 = RoutingTree((0, 0), [(Routes.east, tree_a_10)])

    tree_b_11 = RoutingTree((1, 1), [(Routes.core(1), object())])
    tree_b = RoutingTree((0, 1), [(Routes.east, tree_b_11)])
    tree_a1 = tree_b  # The same

    tree_c_10 = RoutingTree((1, 0), [(Routes.core(1), object())])
    tree_c = RoutingTree((0, 0), [(Routes.east, tree_c_10)])

    tree_d = RoutingTree((1, 1), [(Routes.core(1), object()),
                                  (Routes.core(2), object())])

    tree_e = RoutingTree((0, 2), [(Routes.core(1), object())])

    tree_f = RoutingTree((0, 1), [(Routes.north, tree_e)])

    # Build the net graph
    routes = {
        'a': [tree_a0, tree_a1],
        'b': tree_b,
        'c': tree_c,
        'd': tree_d,
        'e': tree_e,
        'f': tree_f,
    }

    # Nets are assigned identifiers
    net_ids = assign_multicast_net_ids(routes, prior_constraints)

    # Basic check
    expected_graph = {
        'a': {'c', 'd', 'f'},
        'b': {'d', 'f'},
    }  # Truncated because no need to check a != c and c != a

    if prior_constraints:
        expected_graph['d'] = {'e'}

    for a, bs in iteritems(expected_graph):
        for b in bs:
            assert net_ids[a] != net_ids[b]
