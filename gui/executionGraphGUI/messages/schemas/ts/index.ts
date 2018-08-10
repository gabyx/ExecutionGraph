import { executionGraphGUI as exec1 } from './GeneralInfoMessages_generated';
import { GraphTypeDescription, NodeTypeDescription, SocketTypeDescription } from '@serialization/index';

export namespace GeneralInfoMessages {
    export import GetAllGraphTypeDescriptionsResponse = exec1.serialization.GetAllGraphTypeDescriptionsResponse;
    export import GraphTypeDescription = GraphTypeDescription;
    export import NodeTypeDescription = NodeTypeDescription;
    export import SocketTypeDescription = SocketTypeDescription;
}