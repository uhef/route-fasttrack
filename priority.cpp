#include <iostream>
#include <queue>
#include <string>

int main() {
  std::cout << "Hello priority queue!" << std::endl;
  std::priority_queue<int> pq;
  pq.push(3);
  pq.push(6);
  pq.push(1);
  while(!pq.empty()) {
    std::cout << "Next element: " << pq.top() << std::endl;
    pq.pop();
  }
  return 0;
}

