
#include "TestFunctions.hpp"

#include <meta/meta.hpp>
#include "ExecutionGraph/nodes/LogicNode.hpp"
#include "ExecutionGraph/nodes/LogicSocket.hpp"
#include "ExecutionGraph/graphs/ExecutionTreeInOut.hpp"

using namespace executionGraph;

using Config = GeneralConfig<>;

template<typename TConfig>
class IntegerNode : public TConfig::NodeBaseType
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

    using InSockets  = InSocketDeclList<InSocketDecl<Value1, int>,
                                        InSocketDecl<Value2, int>>;

    using OutSockets = OutSocketDeclList<OutSocketDecl<Result1, int>>;

    EXEC_GRAPH_DEFINE_LOGIC_NODE_GET_TYPENAME();
    EXEC_GRAPH_DEFINE_LOGIC_NODE_VALUE_GETTERS(Ins, InSockets, Outs, OutSockets);

    template<typename... Args>
    IntegerNode(Args&&... args)
        : Base(std::forward<Args>(args)...)
    {
        // Add all sockets
        this->template addSockets<InSockets>(std::make_tuple(2,2));
        this->template addSockets<OutSockets>(std::make_tuple(0));
    }

    void reset() override {};

    void compute() override {

        // ugly syntax due to template shit
        //        this->template getValue<typename OutSockets::template Get<Result1>>() =
        //            this->template getValue<typename InSockets::template Get<Value1>>() +
        //            this->template getValue<typename InSockets::template Get<Value2>>();
        getOutVal<Result1>() = getInVal<Value1>() + getInVal<Value2>();
    }

};

MY_TEST(ExecutionTree_Test, Int_Int)
{
    using IntNode = IntegerNode<Config>;
    auto node1a = std::make_unique<IntNode>(0,"1a");
    auto node1b = std::make_unique<IntNode>(1,"1b");

    auto node2a = std::make_unique<IntNode>(2,"2a");
    auto node2b = std::make_unique<IntNode>(3,"2b");

    auto node3a = std::make_unique<IntNode>(4,"3a");
    auto node3b = std::make_unique<IntNode>(5,"3b");

    auto node4a = std::make_unique<IntNode>(6,"4a");
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
    node4a->setGetLink(*node3a,
                       node3a->getOutIdx<IntNode::Result1>(),
                       IntNode::Value1);
    node4a->setGetLink(*node3b,0,1);

    node3a->setGetLink(*node1a,0,0);
    node3a->setGetLink(*node1b,0,1);
//    node1a->addWriteLink(0,*node3a,0);
//    node1b->addWriteLink(0,*node3a,1);

    node3b->setGetLink(*node2a,0,0);
    node3b->setGetLink(*node2b,0,1);
    node1a->setGetLink(*node1a,0,0);

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

    std::cout << "Result : "<< resultNode->getOutVal<IntegerNode<Config>::Result1>() << std::endl;

    EXEC_GRAPH_THROWEXCEPTION_IF( resultNode->getOutVal<IntegerNode<Config>::Result1>() != 16 ,"wrong result");
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
