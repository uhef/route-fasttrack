#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <forward_list>
#include <queue>
#include <climits>
#include <stack>

using namespace rapidjson;

class Weight {
public:
  int node;
  int weight;
  int neighborCount;
  std::forward_list<std::pair<int, int>> neighbors;

  Weight(int n, int w, std::pair<int, int> neighbor) {
    node = n;
    weight = w;
    neighborCount = 0;
    neighbors.push_front(neighbor);
  }
};

class NodeComparison
{
public:
  bool operator() (const std::pair<int, int>& left, const std::pair<int, int>& right) const {
    return (left.second > right.second);
  }
};

void logNode(const std::unordered_map<int, Weight> weights, int index) {
  const Weight& w = weights.at(index);
  std::cout << "Node " << index << " leads to " << w.node
	    << " by weight " << w.weight
	    << " neighbor count: " << w.neighborCount << std::endl;
  std::cout << "\tNeighbors are:" << std::endl;
  for (std::pair<int, int> neighbor : w.neighbors) {
    std::cout << "\tNode: " << neighbor.first << " weight: " << neighbor.second << std::endl;
  }
}

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

int calculate_priority(const std::unordered_map<int, Weight>& weights, int goal, int id, int g_value) {
  if(!weights.count(id) || weights.at(id).neighbors.empty()) {
    if(id == goal) return 0;
    else return INT_MAX;
  }
  const Weight& weight = weights.at(id);
  std::forward_list<std::pair<int, int>>::const_iterator it = weight.neighbors.begin();
  int smallest_distance = (*it).second;
  while(++it != weight.neighbors.end()) {
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

std::stack<int> calculateRoute(int start, int goal, const std::unordered_map<int, Weight>& weights) {
  std::cout << "Calculating route between " << start << " and " << goal << std::endl;
  std::unordered_map<int, int> came_from;
  std::unordered_map<int, int> g_value;
  std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, NodeComparison> fringe;
  fringe.push(std::make_pair(start, 0));
  came_from[start] = start;
  g_value[start] = 0;

  while(!fringe.empty()) {
    int current_id = fringe.top().first;
    // std::cout << " " << current.id;
    fringe.pop();
    if(current_id == goal) {
      std::cout << "Found solution!" << std::endl;
      return reconstruct_path(came_from, start, goal);
    }

    if(weights.count(current_id) && !weights.at(current_id).neighbors.empty()) {
      const Weight& weight = weights.at(current_id);
      for(std::pair<int, int> neighbor : weight.neighbors) {
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
  std::cout << "Didn't find solution :(" << std::endl;
  return std::stack<int>();
}

int main() {
  Document graphDocument = readJSON("graph.json");
  Document journeysDocument = readJSON("journeys.json");

  std::cout << graphDocument.IsArray() << std::endl;
  std::cout << journeysDocument.IsArray() << std::endl;

  std::unordered_map<int, Weight> weights;
  for (SizeType i = 0; i < graphDocument.Size(); ++i) {
    int source = graphDocument[i]["from"].GetInt();
    int target = graphDocument[i]["to"].GetInt();
    int weight = graphDocument[i]["weight"].GetInt();
    try {
      Weight& element = weights.at(source);
      element.neighborCount++;
      element.neighbors.push_front(std::make_pair(target, weight));
    }
    catch(std::out_of_range& e) {
      weights.emplace(source, Weight(target, weight, std::make_pair(target, weight)));
    }
  }

  for (SizeType i = 0; i < journeysDocument.Size(); ++i) {
    int start = journeysDocument[i]["from"].GetInt();
    int goal = journeysDocument[i]["to"].GetInt();
    if (!journeysDocument[i].HasMember("route")) {
      std::stack<int> route = calculateRoute(start, goal, weights);
      std::cout << "Route:" << std::endl;
      while(!route.empty()) {
        std::cout << route.top() << std::endl;
        route.pop();
      }
    }
  }

  return 0;
}

