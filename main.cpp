#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <iostream>
#include <fstream>
#include <unordered_map>

using namespace rapidjson;

class Weight {
public:
  int node;
  int weight;
  int neighborCount;

  Weight(int n, int w) {
    node = n;
    weight = w;
  }
};

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
    }
    catch(std::out_of_range& e) {
      weights.emplace(source, Weight(target, weight));
    }
  }
  std::cout << "Node 0 leads to " << weights.at(0).node << " by weight " << weights.at(0).weight << " neighbor count: " << weights.at(0).neighborCount << std::endl;
  std::cout << "Node 1 leads to " << weights.at(1).node << " by weight " << weights.at(1).weight << " neighbor count: " << weights.at(1).neighborCount << std::endl;
  std::cout << "Node 2 leads to " << weights.at(2).node << " by weight " << weights.at(2).weight << " neighbor count: " << weights.at(2).neighborCount << std::endl;

  return 0;
}

