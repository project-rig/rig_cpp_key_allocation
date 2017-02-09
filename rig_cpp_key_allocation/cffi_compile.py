"""Build the CFFI interface for the key allocation library.

Stand-alone usage:

    $ python cffi_compile.py
"""

import os
import platform
from cffi import FFI
ffi = FFI()

source_dir = os.path.dirname(__file__)

ffi.set_source(
    "_rig_cpp_key_allocation",
    r"""
    #include "constraints_graph.h"
    """,
    source_extension=".cpp",
    sources=[os.path.join(source_dir, "constraints_graph.cpp")],
    include_dirs=[source_dir] + (
        ["C:\\Libraries\\boost\\"] if platform.system() == "Windows" else []
    ),
)

ffi.cdef("""
  struct MulticastKeyConstraintGraph;
  typedef struct MulticastKeyConstraintGraph MulticastKeyConstraintGraph;

  // Create a new constraint graph
  MulticastKeyConstraintGraph* cffi_new_graph(
      const unsigned int n_nodes);

  // Delete an existing constraint graph
  void cffi_delete_graph(MulticastKeyConstraintGraph* graph);

  // Add a constraint to a graph
  void cffi_add_graph_constraint(
      MulticastKeyConstraintGraph* graph,
      const unsigned int a,
      const unsigned int b
  );

  // Add a route to the graph
  void cffi_add_route_to_graph(
      MulticastKeyConstraintGraph* graph,
      const unsigned int net,
      const unsigned int x,
      const unsigned int y,
      const uint32_t route
  );

  // Colour the graph, writing the result into the provided array
  void cffi_colour_graph(
      MulticastKeyConstraintGraph* graph,
      unsigned int* colouring
  );
""")

if __name__ == "__main__":
    ffi.compile(verbose=True)
