#include "Core/Engine.h"

class TestApplication : public UniversalEngine::Engine {
public:
    TestApplication() = default;
    ~TestApplication() = default;
};

UniversalEngine::Engine* UniversalEngine::CreateApplication() {
    return new TestApplication();
}

int main() {
    auto app = UniversalEngine::CreateApplication();
    app->Run();
    delete app;
    return 0;
}
