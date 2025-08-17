//Genrated by chatgt bc im super LAZY

#include "ECSTest.h"
#include "../src/Core/ECS/World.h"
#include "../src/Core/Components/TestComponent.h"

using namespace UniversalEngine;
using namespace UniversalEngine::Testing;

class ECSTests {
public:
    static bool TestEntityCreation() {
        World world;
        
        // Test entity creation
        auto entity1 = world.CreateEntity();
        auto entity2 = world.CreateEntity();
        
        ASSERT_TRUE(entity1.IsValid());
        ASSERT_TRUE(entity2.IsValid());
        ASSERT_NE(entity1.GetID(), entity2.GetID());
        ASSERT_EQ(world.GetEntityCount(), 2);
        
        return true;
    }
    
    static bool TestEntityValidation() {
        World world;
        
        auto entity = world.CreateEntity();
        ASSERT_TRUE(world.IsEntityValid(entity));
        
        // Test invalid entity
        Entity invalidEntity;
        ASSERT_FALSE(world.IsEntityValid(invalidEntity));
        
        return true;
    }
    
    static bool TestDeferredComponentAddition() {
        World world;
        auto entity = world.CreateEntity();
        
        // Component should not be available before flush
        ASSERT_FALSE(world.HasComponent<TestComponent>(entity));
        ASSERT_EQ(world.GetPendingOperationCount(), 0);
        
        // Add component
        world.AddComponent<TestComponent>(entity, TestComponent(42));
        
        // Should still not be available before flush
        ASSERT_FALSE(world.HasComponent<TestComponent>(entity));
        ASSERT_EQ(world.GetPendingOperationCount(), 1);
        
        // Flush operations
        world.ecs_flush();
        
        // Now component should be available
        ASSERT_TRUE(world.HasComponent<TestComponent>(entity));
        ASSERT_EQ(world.GetPendingOperationCount(), 0);
        
        // Test component value
        auto& component = world.GetComponent<TestComponent>(entity);
        ASSERT_EQ(component.GetValue(), 42);
        
        return true;
    }
    
    static bool TestComponentRetrieval() {
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
    
    static bool TestComponentRemoval() {
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
    
    static bool TestMultipleComponents() {
        World world;
        auto entity1 = world.CreateEntity();
        auto entity2 = world.CreateEntity();
        
        world.AddComponent<TestComponent>(entity1, TestComponent(10));
        world.AddComponent<TestComponent>(entity2, TestComponent(20));
        world.ecs_flush();
        
        ASSERT_TRUE(world.HasComponent<TestComponent>(entity1));
        ASSERT_TRUE(world.HasComponent<TestComponent>(entity2));
        
        ASSERT_EQ(world.GetComponent<TestComponent>(entity1).GetValue(), 10);
        ASSERT_EQ(world.GetComponent<TestComponent>(entity2).GetValue(), 20);
        
        return true;
    }
    
    static bool TestEntityDestruction() {
        World world;
        auto entity = world.CreateEntity();
        
        world.AddComponent<TestComponent>(entity, TestComponent(99));
        world.ecs_flush();
        
        ASSERT_TRUE(world.IsEntityValid(entity));
        ASSERT_TRUE(world.HasComponent<TestComponent>(entity));
        ASSERT_EQ(world.GetEntityCount(), 1);
        
        world.DestroyEntity(entity);
        
        ASSERT_FALSE(world.IsEntityValid(entity));
        ASSERT_FALSE(world.HasComponent<TestComponent>(entity));
        ASSERT_EQ(world.GetEntityCount(), 0);
        
        return true;
    }
    
    static bool TestPendingOperationCount() {
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
};

void RunECSTests() {
    TestSuite ecsTestSuite("ECS System Tests");
    
    ecsTestSuite.AddTest("Entity Creation", ECSTests::TestEntityCreation);
    ecsTestSuite.AddTest("Entity Validation", ECSTests::TestEntityValidation);
    ecsTestSuite.AddTest("Deferred Component Addition", ECSTests::TestDeferredComponentAddition);
    ecsTestSuite.AddTest("Component Retrieval", ECSTests::TestComponentRetrieval);
    ecsTestSuite.AddTest("Component Removal", ECSTests::TestComponentRemoval);
    ecsTestSuite.AddTest("Multiple Components", ECSTests::TestMultipleComponents);
    ecsTestSuite.AddTest("Entity Destruction", ECSTests::TestEntityDestruction);
    ecsTestSuite.AddTest("Pending Operation Count", ECSTests::TestPendingOperationCount);
    
    auto results = ecsTestSuite.RunTests();
    ecsTestSuite.PrintSummary(results);
}
