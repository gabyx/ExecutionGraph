
#include "TestFunctions.hpp"

#include <meta/meta.hpp>
#include "ExecutionGraph/nodes/LogicNode.hpp"
#include "ExecutionGraph/nodes/LogicSocket.hpp"

using namespace executionGraph;

using Config = GeneralConfig<>;

template<typename TConfig>
class IntegerNode : public TConfig::NodeBaseType
{
public:
    using Config = TConfig;
    using Base = typename Config::NodeBaseType;

    enum class Inputs{ Value1, Value2};
    enum class Outputs{ Result1, Result2};
    EXEC_GRAPH_DEFINE_SOCKET_TRAITS(Inputs,Outputs);

    using InputSockets   = InputSocketDeclList< InputSocketDecl<Inputs::Value1, int>,
                                                InputSocketDecl<Inputs::Value2, int> >;

    using OutputSockets  = OutputSocketDeclList< OutputSocketDecl<Outputs::Result1, int>,
                                                 OutputSocketDecl<Outputs::Result2, int> >;


    //EXEC_GRAPH_DEFINE_SOCKET_INFO;

    template<typename... Args>
    IntegerNode(Args&&... args) : Base(std::forward<Args>(args)...) {
        // Add all sockets
        this->template addSockets<InputSockets>(std::make_tuple(1,2));
        this->template addSockets<OutputSockets>(std::make_tuple(1,2));
    }

    void reset() override {};
    void compute() override {
    }

};


MY_TEST(Node_Test, Int_Int)
{
    // Integer node connection (wrong connection)
    IntegerNode<Config> node1(1);
    int a = 2;
    node1.addOSock<int>(a,"integer");

    IntegerNode<Config> node2(2);
    node2.addISock<int>(a,"integer");

    try{
        node1.addWriteLink(0,node2,10);
    }
    catch( NodeConnectionException& e)
    {
        return;
    }

    throw std::runtime_error("Not catched exception!!!");
}

int main(int argc, char **argv) {
        testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

