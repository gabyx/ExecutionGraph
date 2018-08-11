

// Export everything which is needed for GeneralInfoMessages (into a namespace!)
import { executionGraphGUI as exec1 } from './GeneralInfoMessages_generated';
import * as serialization from '@serialization/index';
export namespace GeneralInfoMessages {
    export import GetAllGraphTypeDescriptionsResponse = exec1.serialization.GetAllGraphTypeDescriptionsResponse;
    export import GraphTypeDescription = serialization.GraphTypeDescription;
    export import NodeTypeDescription = serialization.NodeTypeDescription;
    export import SocketTypeDescription = serialization.SocketTypeDescription;
}

// Export everything which is needed for GeneralInfoMessages (into a namespace!) 
import { executionGraphGUI as exec2 } from './GraphManipulationMessages_generated';
export namespace GraphManipulationMessages {
    export import AddNodeRequest = exec2.serialization.AddNodeRequest;
    export import AddNodeResponse = exec2.serialization.AddNodeResponse;

    export import NodeConstructionInfo = exec2.serialization.NodeConstructionInfo;

    export import LogicNode = serialization.LogicNode;
    export import LogicSocket = serialization.LogicSocket;
}