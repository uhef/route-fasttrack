#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <forward_list>
#include <queue>
#include <climits>
#include <stack>

using namespace rapidjson;

typedef std::forward_list<std::pair<int, int>> Neighbors;

class NodeComparison
{
public:
  bool operator() (const std::pair<int, int>& left, const std::pair<int, int>& right) const {
    return (left.second > right.second);
  }
};

Document read_json(const std::string& file_name) {
  char* input_buffer = new char[2048];
  FILE* input_file = fopen(file_name.c_str(), "r");
  FileReadStream input_stream(input_file, input_buffer, 2048);

  Document document;
  document.ParseStream(input_stream);

  fclose(input_file);
  delete input_buffer;

  return document;
}

int calculate_priority(const std::unordered_map<int, Neighbors>& node_neighbors, int goal, int id, int g_value) {
  if(!node_neighbors.count(id) || node_neighbors.at(id).empty()) {
    if(id == goal) return 0;
    else return INT_MAX;
  }
  const Neighbors& neighbors = node_neighbors.at(id);
  std::forward_list<std::pair<int, int>>::const_iterator it = neighbors.begin();
  int smallest_distance = (*it).second;
  while(++it != neighbors.end()) {
    if((*it).second < smallest_distance) {
      smallest_distance = (*it).second;
    }
  }
  return g_value + smallest_distance;
}

std::stack<int> reconstruct_path(const std::unordered_map<int, int> came_from, int start, int goal) {
  std::stack<int> route;
  int current = goal;
  while(current != start) {
    route.push(current);
    current = came_from.at(current);
  }
  route.push(start);
  return route;
}

std::stack<int> calculate_route(int start, int goal, const std::unordered_map<int, Neighbors>& node_neighbors) {
  std::unordered_map<int, int> came_from;
  std::unordered_map<int, int> g_value;
  std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, NodeComparison> fringe;
  fringe.push(std::make_pair(start, 0));
  came_from[start] = start;
  g_value[start] = 0;

  while(!fringe.empty()) {
    int current_id = fringe.top().first;
    fringe.pop();
    if(current_id == goal) {
      return reconstruct_path(came_from, start, goal);
    }

    if(node_neighbors.count(current_id) && !node_neighbors.at(current_id).empty()) {
      const Neighbors& neighbors = node_neighbors.at(current_id);
      for(std::pair<int, int> neighbor : neighbors) {
        int tentative_g = g_value[current_id] + neighbor.second;
        if(!g_value.count(neighbor.first) || tentative_g < g_value[neighbor.first]) {
          g_value[neighbor.first] = tentative_g;
          int priority = calculate_priority(node_neighbors, goal, neighbor.first, tentative_g);
          fringe.push(std::make_pair(neighbor.first, priority));
          came_from[neighbor.first] = current_id;
        }
      }
    }
  }
  return std::stack<int>();
}

int main() {
  Document graph_document = read_json("graph.json");
  Document journeys_document = read_json("journeys.json");

  std::unordered_map<int, Neighbors> node_neighbors;
  for (SizeType i = 0; i < graph_document.Size(); ++i) {
    int source = graph_document[i]["from"].GetInt();
    int target = graph_document[i]["to"].GetInt();
    int weight = graph_document[i]["weight"].GetInt();
    if(node_neighbors.count(source)) {
      Neighbors& neighbors = node_neighbors.at(source);
      neighbors.push_front(std::make_pair(target, weight));
    }
    else {
      Neighbors neighbors;
      neighbors.push_front(std::make_pair(target, weight));
      node_neighbors[source] = neighbors;
    }
  }

  for (SizeType i = 0; i < journeys_document.Size(); ++i) {
    int start = journeys_document[i]["from"].GetInt();
    int goal = journeys_document[i]["to"].GetInt();
    Document::AllocatorType& allocator = journeys_document.GetAllocator();

    if(!journeys_document[i].HasMember("route")) {
      Value route_array(kArrayType);
      journeys_document[i].AddMember("route", route_array, allocator);
    }
    journeys_document[i]["route"].Clear();

    std::stack<int> route = calculate_route(start, goal, node_neighbors);
    Value& route_array = journeys_document[i]["route"];
    while(!route.empty()) {
      Value route_node(kNumberType);
      route_node.SetInt(route.top());
      route_array.PushBack(route_node, allocator);
      route.pop();
    }
  }

  StringBuffer buffer;
  Writer<StringBuffer> writer(buffer);
  journeys_document.Accept(writer);
  std::cout << buffer.GetString() << std::endl;

  return 0;
}

