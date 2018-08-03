import { executionGraphGUI as exec1 } from './GeneralInfoMessages_generated';

import { executionGraphGUI as exec2 } from './GraphTypeDescription_generated';
import { executionGraphGUI as exec3 } from './NodeTypeDescription_generated';
import { executionGraphGUI as exec4 } from './SocketTypeDescription_generated';

export namespace GeneralInfoMessages
{
    export import GetAllGraphTypeDescriptionsResponse = exec1.serialization.GetAllGraphTypeDescriptionsResponse;
    export import GraphTypeDescription = exec2.serialization.GraphTypeDescription;
    export import NodeTypeDescription = exec3.serialization.NodeTypeDescription;
    export import SocketTypeDescription = exec4.serialization.SocketTypeDescription;
}