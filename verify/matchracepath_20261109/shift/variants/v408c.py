OLD = """    s3.y = fwd.y * 2.2f;
    s3.z = fwd.z * 2.2f;
    s4.z = fwd.z * 3.3f;
    s3.x = fwd.x * 2.2f;
    s4.x = fwd.x * 3.3f;
    s4.y = fwd.y * 3.3f;
"""
L = {"3x": "    s3.x = fwd.x * 2.2f;\n", "3y": "    s3.y = fwd.y * 2.2f;\n", "3z": "    s3.z = fwd.z * 2.2f;\n",
     "4x": "    s4.x = fwd.x * 3.3f;\n", "4y": "    s4.y = fwd.y * 3.3f;\n", "4z": "    s4.z = fwd.z * 3.3f;\n"}
import itertools
orders = ["3x 3y 3z 4x 4y 4z", "4x 4y 4z 3x 3y 3z", "3x 4x 3y 4y 3z 4z", "3y 3z 3x 4x 4y 4z", "3x 3y 4x 4y 3z 4z", "3z 3x 3y 4x 4y 4z", "3x 3y 3z 4z 4x 4y", "3x 3y 4z 3z 4x 4y"]
V = [(o.replace(" ", "_"), [(OLD, "".join(L[k] for k in o.split()))]) for o in orders]
