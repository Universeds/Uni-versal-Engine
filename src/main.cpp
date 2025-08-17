#include "Core/Engine.h"
#include <iostream>

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

    auto app = UniversalEngine::CreateApplication();
    app->Run();
    delete app;
    
    return 0;
}
