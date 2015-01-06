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

  std::unordered_map<int, int> weights;
  for (SizeType i = 0; i < d.Size(); ++i) {
    weights[d[i]["from"].GetInt()] = d[i]["to"].GetInt();
  }
  std::cout << "Node 0 leads to " << weights[0] << std::endl;
  std::cout << "Node 1 leads to " << weights[1] << std::endl;
  std::cout << "Node 2 leads to " << weights[2] << std::endl;

  return 0;
}

