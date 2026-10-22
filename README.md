# Dynamic Emergency Response & Resource Optimization System

C++ console simulation of an emergency dispatch centre. When several emergencies arrive together, it decides which case is served first, which ambulance goes, which route it takes and which hospital receives the patient.

**Team Metamorph (DSCPP-III-2026-T207)**: Suryansh Gusain, Yuvraj Saini, Aakash Gupta
Graphic Era (Deemed to be University), Dehradun. Mentor: Ms. Ashwini Yeole

## How it works
1. Emergencies enter a custom **max-heap**, ranked by priority score = severity x 100 + waiting minutes.
2. The nearest free ambulance that can handle the severity is chosen (Basic: severity 1-3, ICU: 1-5).
3. **Dijkstra's algorithm** on a weighted road graph finds the route to the scene and then to the nearest hospital with a free bed.
4. Ambulances and hospitals are looked up by ID in hash tables (`unordered_map`).

## Files
| File | Purpose |
|---|---|
| `entities.h` / `entities.cpp` | Location, Emergency, Ambulance (Basic, ICU), Hospital classes |
| `emergency_heap.h` | Custom max-heap priority queue |
| `road_graph.h` | Weighted graph and Dijkstra |
| `resource_registry.h` | Hash-based lookup of ambulances and hospitals |
| `main.cpp` | Dispatch pipeline and five-emergency demo |
| `output.txt` | Output of the demo run |

## Build and run
```
g++ -std=c++17 -Wall -Wextra -o dispatch main.cpp entities.cpp
./dispatch
```
On Windows, run `dispatch.exe` instead of `./dispatch`.

## Status
Phase II complete: classes, heap, graph with Dijkstra, hash lookup and the full dispatch pipeline.
Planned: file-based logs, handling of no-ambulance and no-bed cases, BFS/DFS comparison.
