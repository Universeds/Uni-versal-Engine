#include "Core/Engine.h"
#include <iostream>

void RunECSTests();

class TestApplication : public UniversalEngine::Engine {
public:
    TestApplication() = default;
    ~TestApplication() = default;
};

UniversalEngine::Engine* UniversalEngine::CreateApplication() {
    return new TestApplication();
}

int main() {
    std::cout << "its WORKING YEAAAA" << std::endl;

    RunECSTests();
    auto app = UniversalEngine::CreateApplication();
    app->Run();
    delete app;
    
    return 0;
}
