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

typedef std::forward_list<std::pair<int, int>> node_neighbors;

class NodeComparison
{
public:
  bool operator() (const std::pair<int, int>& left, const std::pair<int, int>& right) const {
    return (left.second > right.second);
  }
};

Document readJSON(const std::string& fileName) {
  char* inputBuffer = new char[2048];
  FILE* inputFile = fopen(fileName.c_str(), "r");
  FileReadStream inputFileStream(inputFile, inputBuffer, 2048);

  Document d;
  d.ParseStream(inputFileStream);

  fclose(inputFile);
  delete inputBuffer;

  return d;
}

int calculate_priority(const std::unordered_map<int, node_neighbors>& weights, int goal, int id, int g_value) {
  if(!weights.count(id) || weights.at(id).empty()) {
    if(id == goal) return 0;
    else return INT_MAX;
  }
  const node_neighbors& weight = weights.at(id);
  std::forward_list<std::pair<int, int>>::const_iterator it = weight.begin();
  int smallest_distance = (*it).second;
  while(++it != weight.end()) {
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

std::stack<int> calculateRoute(int start, int goal, const std::unordered_map<int, node_neighbors>& weights) {
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

    if(weights.count(current_id) && !weights.at(current_id).empty()) {
      const node_neighbors& weight = weights.at(current_id);
      for(std::pair<int, int> neighbor : weight) {
        int tentative_g = g_value[current_id] + neighbor.second;
        if(!g_value.count(neighbor.first) || tentative_g < g_value[neighbor.first]) {
          g_value[neighbor.first] = tentative_g;
          int priority = calculate_priority(weights, goal, neighbor.first, tentative_g);
          fringe.push(std::make_pair(neighbor.first, priority));
          came_from[neighbor.first] = current_id;
        }
      }
    }
  }
  return std::stack<int>();
}

int main() {
  Document graph_document = readJSON("graph.json");
  Document journeys_document = readJSON("journeys.json");

  std::unordered_map<int, node_neighbors> weights;
  for (SizeType i = 0; i < graph_document.Size(); ++i) {
    int source = graph_document[i]["from"].GetInt();
    int target = graph_document[i]["to"].GetInt();
    int weight = graph_document[i]["weight"].GetInt();
    if(weights.count(source)) {
      node_neighbors& element = weights.at(source);
      element.push_front(std::make_pair(target, weight));
    }
    else {
      node_neighbors neighbors;
      neighbors.push_front(std::make_pair(target, weight));
      weights[source] = neighbors;
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

    std::stack<int> route = calculateRoute(start, goal, weights);
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

