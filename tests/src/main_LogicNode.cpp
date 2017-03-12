
#include "TestFunctions.hpp"

#include "ExecutionGraph/nodes/LogicNode.hpp"
#include "ExecutionGraph/nodes/LogicSocket.hpp"

using namespace ExecutionGraph;

using Config = GeneralConfig<>;

template<typename TConfig>
class IntegerNode : public TConfig::NodeBaseType
{
public:
    template<typename... Args>
    IntegerNode(Args&&... args) : TConfig::NodeBaseType(std::forward<Args>(args)...) {}

    void reset() override {};
    void compute() override {};

};


int main(){


    IntegerNode<Config> node1(1);
    int a = 2;
    node1.addOSock<int>(a,"integer");

    IntegerNode<Config> node2(2);
    node2.addISock<int>(a,"integer");

    node1.addWriteLink(0,node2,0);

}
