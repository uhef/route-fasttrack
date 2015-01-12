# A\* Algorithm on C++11

Shortest route calculation on C++11. Done as a solution to fast track challenge: http://reaktor.fi/careers/c-cplusplus-rust-fast-track/

Uses [rapidjson](https://github.com/miloyip/rapidjson) for JSON serialization and deserialization.

Uses C++11. Tested on G++ 4.8.2

Although the problem is not an exemplary use case for A\* I wanted to try it out. A\* would perform better given a more efficient heuristic (like geometric distance between node and goal node). Current heuristic is simply the shortest distance from node to any neighboring node unless the node has no neighbors (in which case the heuristic is 0 for goal node and MAX\_INT for any other node).

Algorithm could be improved further by introducing closed set for nodes that have already been traversed.
