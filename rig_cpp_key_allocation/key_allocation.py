import logging
from rig.place_and_route.routing_tree import RoutingTree
from _rig_cpp_key_allocation import ffi
from _rig_cpp_key_allocation.lib import (
    cffi_new_graph, cffi_delete_graph, cffi_add_graph_constraint,
    cffi_add_route_to_graph, cffi_colour_graph
)
from six import iterkeys, iteritems

logger = logging.getLogger(__file__)


def assign_multicast_net_ids(nets, additional_constraints=dict()):
    """TODO

    Parameters
    ----------
    nets : {net: RoutingTree or [RoutingTree, ...], ...}
        A mapping from nets to the routing tree(s) to which they correspond.
    additional_constraints : {net: [net, ...], ...}
        A mapping from nets to iterables of other nets with which they may not
        share a routing identifier.

    Returns
    -------
    {net: int}
        TODO
    """
    logger.debug("Building multicast nets constraint graph...")
    # Give each key a unique ID which will be used when building the graph.
    net_ids = {net: i for i, net in enumerate(iterkeys(nets))}

    # Create a graph of the appropriate size
    graph = cffi_new_graph(len(net_ids))

    # Add the prior constraints to the graph
    for net, other_nets in iteritems(additional_constraints):
        for other_net in other_nets:
            net_id = net_ids[net]
            other_net_id = net_ids[other_net]

            # Add the constraint to the graph
            cffi_add_graph_constraint(graph, net_id, other_net_id)

    # Add the constraints resulting from the network connectivity to the graph.
    for net, trees in iteritems(nets):
        net_id = net_ids[net]  # Get the net index

        # Ensure that routes is iterable
        if isinstance(trees, RoutingTree):
            trees = [trees]

        # Loop over the routes, traversing and adding the routes to the graph
        # as we go.
        for tree in trees:
            for _, (x, y), routes in tree.traverse():
                # Convert the route into an integer representation
                route = 0x0
                for r in routes:
                    route |= (1 << r)

                # Add the route to the graph
                cffi_add_route_to_graph(graph, net_id, x, y, route)

    # Colour the graph before deleting it (and removing the dangling pointer!)
    logger.debug("Assigning IDs to multicast nets...")
    colouring = ffi.new("unsigned int[]", len(net_ids))
    cffi_colour_graph(graph, colouring)
    cffi_delete_graph(graph)
    del graph  # Not required any more

    # Read the colouring out into a dictionary
    net_colouring = {net: colouring[i] for net, i in iteritems(net_ids)}
    return net_colouring
