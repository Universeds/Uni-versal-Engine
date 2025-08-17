// genrated by chatgt bc im super LAZY

#include "ECSTest.h"
#include "../src/Core/ECS/World.h"
#include "../src/Core/Components/TestComponent.h"
#include <iostream>
#include <string>
#include <map>

using namespace UniversalEngine;
using namespace UniversalEngine::Testing;

bool TestEntityCreation();
bool TestEntityValidation();
bool TestDeferredComponentAddition();
bool TestComponentRetrieval();
bool TestComponentRemoval();
bool TestMultipleComponents();
bool TestEntityDestruction();
bool TestPendingOperationCount();

bool TestEntityCreation() {
    World world;
    
    auto entity1 = world.CreateEntity();
    auto entity2 = world.CreateEntity();
    
    ASSERT_TRUE(entity1.IsValid());
    ASSERT_TRUE(entity2.IsValid());
    ASSERT_NE(entity1.GetID(), entity2.GetID());
    ASSERT_EQ(world.GetEntityCount(), 2);
    
    return true;
}

bool TestEntityValidation() {
    World world;
    
    auto entity = world.CreateEntity();
    ASSERT_TRUE(world.IsEntityValid(entity));
    
    Entity invalidEntity;
    ASSERT_FALSE(world.IsEntityValid(invalidEntity));
    
    return true;
}

bool TestDeferredComponentAddition() {
    World world;
    auto entity = world.CreateEntity();
    
    ASSERT_FALSE(world.HasComponent<TestComponent>(entity));
    ASSERT_EQ(world.GetPendingOperationCount(), 0);
    
    world.AddComponent<TestComponent>(entity, TestComponent(42));
    
    ASSERT_FALSE(world.HasComponent<TestComponent>(entity));
    ASSERT_EQ(world.GetPendingOperationCount(), 1);
    
    world.ecs_flush();
    
    ASSERT_TRUE(world.HasComponent<TestComponent>(entity));
    ASSERT_EQ(world.GetPendingOperationCount(), 0);
    
    auto& component = world.GetComponent<TestComponent>(entity);
    ASSERT_EQ(component.GetValue(), 42);
    
    return true;
}

bool TestComponentRetrieval() {
    World world;
    auto entity = world.CreateEntity();
    
    world.AddComponent<TestComponent>(entity, TestComponent(100));
    world.ecs_flush();
    
    ASSERT_TRUE(world.HasComponent<TestComponent>(entity));
    
    auto& component = world.GetComponent<TestComponent>(entity);
    ASSERT_EQ(component.GetValue(), 100);
    
    // Test modifying component
    component.SetValue(200);
    ASSERT_EQ(world.GetComponent<TestComponent>(entity).GetValue(), 200);
    
    return true;
}

bool TestComponentRemoval() {
    World world;
    auto entity = world.CreateEntity();
    
    // Add component
    world.AddComponent<TestComponent>(entity, TestComponent(50));
    world.ecs_flush();
    ASSERT_TRUE(world.HasComponent<TestComponent>(entity));
    
    // Remove component
    world.RemoveComponent<TestComponent>(entity);
    ASSERT_EQ(world.GetPendingOperationCount(), 1);
    ASSERT_TRUE(world.HasComponent<TestComponent>(entity)); // Still there before flush
    
    world.ecs_flush();
    ASSERT_FALSE(world.HasComponent<TestComponent>(entity)); // Gone after flush
    ASSERT_EQ(world.GetPendingOperationCount(), 0);
    
    return true;
}

bool TestMultipleComponents() {
    World world;
    auto entity1 = world.CreateEntity();
    auto entity2 = world.CreateEntity();
    
    // Add different components to different entities
    world.AddComponent<TestComponent>(entity1, TestComponent(10));
    world.AddComponent<TestComponent>(entity2, TestComponent(20));
    world.ecs_flush();
    
    ASSERT_TRUE(world.HasComponent<TestComponent>(entity1));
    ASSERT_TRUE(world.HasComponent<TestComponent>(entity2));
    
    ASSERT_EQ(world.GetComponent<TestComponent>(entity1).GetValue(), 10);
    ASSERT_EQ(world.GetComponent<TestComponent>(entity2).GetValue(), 20);
    
    return true;
}

bool TestEntityDestruction() {
    World world;
    auto entity = world.CreateEntity();
    
    world.AddComponent<TestComponent>(entity, TestComponent(99));
    world.ecs_flush();
    
    ASSERT_TRUE(world.IsEntityValid(entity));
    ASSERT_TRUE(world.HasComponent<TestComponent>(entity));
    ASSERT_EQ(world.GetEntityCount(), 1);
    
    // Destroy entity
    world.DestroyEntity(entity);
    
    ASSERT_FALSE(world.IsEntityValid(entity));
    ASSERT_FALSE(world.HasComponent<TestComponent>(entity));
    ASSERT_EQ(world.GetEntityCount(), 0);
    
    return true;
}

bool TestPendingOperationCount() {
    World world;
    auto entity = world.CreateEntity();
    
    ASSERT_EQ(world.GetPendingOperationCount(), 0);
    
    world.AddComponent<TestComponent>(entity, TestComponent(1));
    ASSERT_EQ(world.GetPendingOperationCount(), 1);
    
    world.AddComponent<TestComponent>(world.CreateEntity(), TestComponent(2));
    ASSERT_EQ(world.GetPendingOperationCount(), 2);
    
    world.ecs_flush();
    ASSERT_EQ(world.GetPendingOperationCount(), 0);
    
    return true;
}

void RunAllTests() {
    TestSuite ecsTestSuite("ECS System Tests");
    
    ecsTestSuite.AddTest("Entity Creation", TestEntityCreation);
    ecsTestSuite.AddTest("Entity Validation", TestEntityValidation);
    ecsTestSuite.AddTest("Deferred Component Addition", TestDeferredComponentAddition);
    ecsTestSuite.AddTest("Component Retrieval", TestComponentRetrieval);
    ecsTestSuite.AddTest("Component Removal", TestComponentRemoval);
    ecsTestSuite.AddTest("Multiple Components", TestMultipleComponents);
    ecsTestSuite.AddTest("Entity Destruction", TestEntityDestruction);
    ecsTestSuite.AddTest("Pending Operation Count", TestPendingOperationCount);
    
    auto results = ecsTestSuite.RunTests();
    ecsTestSuite.PrintSummary(results);
}

bool RunSingleTest(const std::string& testName) {
    std::map<std::string, std::function<bool()>> testMap = {
        {"Entity Creation", TestEntityCreation},
        {"Entity Validation", TestEntityValidation},
        {"Deferred Component Addition", TestDeferredComponentAddition},
        {"Component Retrieval", TestComponentRetrieval},
        {"Component Removal", TestComponentRemoval},
        {"Multiple Components", TestMultipleComponents},
        {"Entity Destruction", TestEntityDestruction},
        {"Pending Operation Count", TestPendingOperationCount}
    };
    
    auto it = testMap.find(testName);
    if (it != testMap.end()) {
        std::cout << "Running test: " << testName << "..." << std::endl;
        bool result = it->second();
        std::cout << "  " << (result ? "✓ PASSED" : "✗ FAILED") << std::endl;
        return result;
    } else {
        std::cout << "Test not found: " << testName << std::endl;
        return false;
    }
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg.find("--test=") == 0) {
            std::string testName = arg.substr(7); // Remove "--test=" prefix
            if (testName.front() == '"' && testName.back() == '"') {
                testName = testName.substr(1, testName.length() - 2);
            }
            bool result = RunSingleTest(testName);
            return result ? 0 : 1;
        }
    }
    
    // Run all tests if no specific test is requested
    RunAllTests();
    return 0;
}
