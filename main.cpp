#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <forward_list>
#include <queue>

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

class Node {
public:
  int id;
  int f_value;
  Node(int node, int f) {
    id = node;
    f_value = f;
  }
};

class NodeComparison
{
public:
  bool operator() (const Node& left, const Node& right) const {
    return (left.f_value > right.f_value);
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

void calculateRoute(int start, int goal, const std::unordered_map<int, Weight>& weights) {
  std::cout << "Calculating route between " << start << " and " << goal << std::endl;
  std::priority_queue<Node, std::vector<Node>, NodeComparison> fringe;
  fringe.push(Node(start, 0));
  while(!fringe.empty()) {
    std::cout << "Next element: " << fringe.top().id << std::endl;
    fringe.pop();
  }
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
  logNode(weights, 0);
  logNode(weights, 1);
  logNode(weights, 2);

  for (SizeType i = 0; i < journeysDocument.Size(); ++i) {
    int start = journeysDocument[i]["from"].GetInt();
    int goal = journeysDocument[i]["to"].GetInt();
    if (!journeysDocument[i].HasMember("route")) {
      calculateRoute(start, goal, weights);
    }
  }

  return 0;
}

