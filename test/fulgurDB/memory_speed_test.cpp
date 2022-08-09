

#include "gtest/gtest.h"
#include "logging/circular_buffer_pool.h"

#include <iostream>

TEST(SpeedTest1, MoveTest) {

  CircularBufferPool *a = new CircularBufferPool();
  CircularBufferPool *b = new CircularBufferPool();

  for (int i = 0; i < BUFFER_POOL_SIZE; i++) {
    std::unique_ptr<LogBuffer> buffer(new LogBuffer());
    // std::cout << a->GetSize() << "FUCK1\n";
    a->Put(std::move(buffer));
  }

  // std::cout << a->GetSize() << "FUCK2\n";

  auto startTime = std::chrono::system_clock::now();
  for (int i = 0; i < BUFFER_POOL_SIZE; i++) {
    // std::cout << a->GetSize() << "FUCK3\n";
    b->Put(std::move(a->Get()));
  }

  for (int i = 0; i < BUFFER_POOL_SIZE; i++) {
    // std::cout << a->GetSize() << "FUCK3\n";
    a->Put(std::move(b->Get()));
  }

  // std::cout << "FUCK\n";

  auto endTime = std::chrono::system_clock::now();
  std::cout << "time:" 
    << std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() 
    << std::endl;


  delete a;
  delete b;
}


TEST(SpeedTest2, NewTest) {

  CircularBufferPool *a = new CircularBufferPool();



  // std::cout << a->GetSize() << "FUCK2\n";

  auto startTime = std::chrono::system_clock::now();
  for (int i = 0; i < BUFFER_POOL_SIZE; i++) {
    std::unique_ptr<LogBuffer> buffer(new LogBuffer());
    // std::cout << a->GetSize() << "FUCK1\n";
    a->Put(std::move(buffer));
  }

  // std::cout << "FUCK\n";

  auto endTime = std::chrono::system_clock::now();
  std::cout << "time:" 
    << std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() 
    << std::endl;


  delete a;
}
