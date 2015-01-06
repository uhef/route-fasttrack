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
  std::forward_list<int> neighbors;

  Weight(int n, int w, int neighbor) {
    node = n;
    weight = w;
    neighborCount = 0;
    neighbors.push_front(neighbor);
  }
};

void logNode(const std::unordered_map<int, Weight> weights, int index) {
  std::cout << "Node " << index << " leads to " << weights.at(index).node
	    << " by weight " << weights.at(index).weight
	    << " neighbor count: " << weights.at(index).neighborCount << std::endl;
}

int main() {
  char* inputBuffer = new char[2048];
  FILE* inputFile = fopen("graph.json", "r");
  FileReadStream inputFileStream(inputFile, inputBuffer, 2048);

  Document d;
  d.ParseStream(inputFileStream);

  fclose(inputFile);
  delete inputBuffer;

  std::cout << d.IsArray() << std::endl;

  std::unordered_map<int, Weight> weights;
  for (SizeType i = 0; i < d.Size(); ++i) {
    int source = d[i]["from"].GetInt();
    int target = d[i]["to"].GetInt();
    int weight = d[i]["weight"].GetInt();
    try {
      Weight& element = weights.at(source);
      element.neighborCount++;
      element.neighbors.push_front(target);
    }
    catch(std::out_of_range& e) {
      weights.emplace(source, Weight(target, weight, target));
    }
  }
  logNode(weights, 0);
  logNode(weights, 1);
  logNode(weights, 2);

  return 0;
}

