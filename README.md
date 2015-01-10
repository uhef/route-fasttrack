# A\* Algorithm on C++11

Shortest route calculation on C++11. Done as a solution to fast track challenge: http://reaktor.fi/careers/c-cplusplus-rust-fast-track/

Uses [rapidjson](https://github.com/miloyip/rapidjson) for JSON serialization and deserialization.

Uses C++11. Compiles at least on G++ 4.8.2

Problem doesn't benefit too much from usage of A\* since for instance geometric distance between node and goal node cannot be calculated, but I wanted to try it out nevertheless. Heuristic is simply the shortest distance from node to any neighboring node unless if node has no neighbors (in which case the heuristic is 0 for goal node and MAX\_INT for any other node).
