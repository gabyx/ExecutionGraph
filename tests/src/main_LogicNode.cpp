
#include "TestFunctions.hpp"

#include <meta/meta.hpp>
#include "ExecutionGraph/nodes/LogicNode.hpp"
#include "ExecutionGraph/nodes/LogicSocket.hpp"

using namespace ExecutionGraph;

using Config = GeneralConfig<>;

template<typename TConfig>
class IntegerNode : public TConfig::NodeBaseType
{
public:


    using SocketInputTypes  = SocketListTraits< SocketTrait<"Value1">, SocketTrait<"Value2"> >;

    using SocketOutputTypes = SocketListTraits< SocketTrait<"Result1">, SocketTrait<"Result2"> >;

    //EXEC_GRAPH_DEFINE_SOCKET_INFO;

    template<typename... Args>
    IntegerNode(Args&&... args) : TConfig::NodeBaseType(std::forward<Args>(args)...) {
        // Add all sockets
    }

    void reset() override {};
    void compute() override {}

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

