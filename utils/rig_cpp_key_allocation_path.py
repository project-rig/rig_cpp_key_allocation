#!/usr/bin/env python

"""Print the path of the local Rig installation."""

if __name__=="__main__":  # pragma: no cover
    import rig_cpp_key_allocation
    import os.path
    print(os.path.dirname(rig_cpp_key_allocation.__file__))
