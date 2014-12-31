#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <iostream>
#include <fstream>

using namespace rapidjson;

int main() {
  char* inputBuffer = new char[2048];
  FILE* inputFile = fopen("graph.json", "r");
  FileReadStream inputFileStream(inputFile, inputBuffer, 2048);

  Document d;
  d.ParseStream(inputFileStream);

  fclose(inputFile);
  delete inputBuffer;

  std::cout << d.IsArray() << std::endl;

  for (SizeType i = 0; i < d.Size(); ++i) {
    std::cout << d[i]["from"].GetInt() << std::endl;
  }

  return 0;
}

