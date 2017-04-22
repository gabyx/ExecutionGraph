
#include "TestFunctions.hpp"

#include <memory>
#include <meta/meta.hpp>
#include "ExecutionGraph/nodes/LogicNode.hpp"
#include "ExecutionGraph/nodes/LogicSocket.hpp"
#include "ExecutionGraph/graphs/ExecutionTreeInOut.hpp"

using namespace executionGraph;



struct A{
    static const int N = 1<<4;
    int memory[N];
    A(){
        std::fill_n(memory, N, 2);
    }

    A& operator+=(const A& b)
    {
        for(int i = 0; i<N; ++i)
        {
            memory[i] += b.memory[i];
        }
        return *this;
    }
};

using SocketTypes = meta::list<double,std::shared_ptr<A>>;
using Config = GeneralConfig< SocketTypes >;

template<typename TConfig>
class IntegerNode : public typename TConfig::NodeBaseType
{
public:
    using Config = TConfig;
    using Base   = typename Config::NodeBaseType;

    enum Ins
    {
        Value1,
        Value2
    };
    enum Outs
    {
        Result1,
    };
    EXEC_GRAPH_DEFINE_SOCKET_TRAITS(Ins, Outs);

    using InSockets  = InSocketDeclList<InSocketDecl<Value1, std::shared_ptr<A>>,
                                        InSocketDecl<Value2, std::shared_ptr<A>>
                                       >;

    using OutSockets = OutSocketDeclList<OutSocketDecl<Result1, std::shared_ptr<A>> >;

    EXEC_GRAPH_DEFINE_LOGIC_NODE_GET_TYPENAME();
    EXEC_GRAPH_DEFINE_LOGIC_NODE_VALUE_GETTERS(Ins, InSockets, Outs, OutSockets);

    template<typename... Args>
    IntegerNode(Args&&... args)
        : Base(std::forward<Args>(args)...)
    {
        // Add all sockets
        this->template addSockets<InSockets>(std::make_tuple(std::make_shared<A>(),std::make_shared<A>()));
        this->template addSockets<OutSockets>(std::make_tuple(std::make_shared<A>()));
    }

    void reset() override {};

    void compute() override {
        *getOutVal<Result1>() += *getInVal<Value1>();
        *getOutVal<Result1>() += *getInVal<Value2>();
    }
};

MY_TEST(ExecutionTree_Test, Int_Int)
{
    // Integer node connection (wrong connection)
    auto node1a = std::make_unique<IntegerNode<Config>>(0);
    auto node1b = std::make_unique<IntegerNode<Config>>(1);

    auto node2a = std::make_unique<IntegerNode<Config>>(2);
    auto node2b = std::make_unique<IntegerNode<Config>>(3);

    auto node3a = std::make_unique<IntegerNode<Config>>(4);
    auto node3b = std::make_unique<IntegerNode<Config>>(5);

    auto node4a = std::make_unique<IntegerNode<Config>>(6);
    auto resultNode = node4a.get();

    try{
        node1a->getISocket<double>(0);
        EXEC_GRAPH_THROWEXCEPTION("Should throw exception here!");
    }
    catch(BadSocketCastException& e){
        // all correct
    }
    catch(...)
    {
        EXEC_GRAPH_THROWEXCEPTION("Wrong Exception thrown!");
    }
    // Link
    node4a->setGetLink(*node3a,0,0);
    node4a->setGetLink(*node3b,0,1);

    node3a->setGetLink(*node2a,0,0);
    node3a->setGetLink(*node2b,0,1);

    node3b->setGetLink(*node1a,0,0);
    node3b->setGetLink(*node1b,0,1);

    ExecutionTreeInOut<Config> execTree;
    execTree.addNode(std::move(node1a));
    execTree.addNode(std::move(node1b));
    execTree.addNode(std::move(node2a));
    execTree.addNode(std::move(node2b));
    execTree.addNode(std::move(node3a));
    execTree.addNode(std::move(node3b));
    execTree.addNode(std::move(node4a));

    execTree.setNodeClass(0, ExecutionTreeInOut<Config>::NodeClassification::InputNode);
    execTree.setNodeClass(1, ExecutionTreeInOut<Config>::NodeClassification::InputNode);
    execTree.setNodeClass(2, ExecutionTreeInOut<Config>::NodeClassification::InputNode);
    execTree.setNodeClass(3, ExecutionTreeInOut<Config>::NodeClassification::InputNode);
    execTree.setNodeClass(6, ExecutionTreeInOut<Config>::NodeClassification::OutputNode);


    execTree.setup();

    std::cout << execTree.getExecutionOrderInfo() << std::endl;

    execTree.execute(0);

    std::cout << "Result : "<< resultNode->getOutVal<IntegerNode<Config>::Result1>()->memory[1] << std::endl;
    EXEC_GRAPH_THROWEXCEPTION_IF( resultNode->getOutVal<IntegerNode<Config>::Result1>()->memory[1] != 30 ,"wrong result");

}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
