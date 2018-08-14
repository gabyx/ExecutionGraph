

// Export everything which is needed for GeneralInfoMessages (into a namespace!)
import { executionGraphGUI as exec1 } from '@messages/GeneralInfoMessages_generated';
import * as serialization from '../../../../../include/executionGraph/serialization/schemas/ts';

export namespace GeneralInfoMessages {
    export import GetAllGraphTypeDescriptionsResponse = exec1.serialization.GetAllGraphTypeDescriptionsResponse;
    export import GraphTypeDescription = serialization.GraphTypeDescription;
    export import NodeTypeDescription = serialization.NodeTypeDescription;
    export import SocketTypeDescription = serialization.SocketTypeDescription;
}

// Export everything which is needed for GeneralInfoMessages (into a namespace!) 
import { executionGraphGUI as exec2 } from '@messages/GraphManipulationMessages_generated';
export namespace GraphManipulationMessages {
    export import AddNodeRequest = exec2.serialization.AddNodeRequest;
    export import AddNodeResponse = exec2.serialization.AddNodeResponse;

    export import RemoveNodeRequest = exec2.serialization.RemoveNodeRequest;

    export import NodeConstructionInfo = exec2.serialization.NodeConstructionInfo;

    export import LogicNode = serialization.LogicNode;
    export import LogicSocket = serialization.LogicSocket;
}

// Export everything which is needed for GraphManagementMessages (into a namespace!) 
import { executionGraphGUI as exec3 } from '@messages/GraphManagementMessages_generated';
export namespace GraphManagementMessages {
    export import AddGraphRequest = exec3.serialization.AddGraphRequest;
    export import AddGraphResponse = exec3.serialization.AddGraphResponse;

    export import RemoveGraphRequest = exec3.serialization.RemoveGraphRequest;
}