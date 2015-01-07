#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <forward_list>

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

int main() {
  Document d = readJSON("graph.json");

  std::cout << d.IsArray() << std::endl;

  std::unordered_map<int, Weight> weights;
  for (SizeType i = 0; i < d.Size(); ++i) {
    int source = d[i]["from"].GetInt();
    int target = d[i]["to"].GetInt();
    int weight = d[i]["weight"].GetInt();
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

  return 0;
}

