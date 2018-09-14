// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Fri Sep 14 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

// Export everything which is needed for GeneralInfoMessages (into a namespace!)
import { executionGraphGUI as exec1 } from './lib/GeneralInfoMessages_generated';
import * as serialization from '@eg/serialization';

export namespace GeneralInfoMessages {
  export import GetAllGraphTypeDescriptionsResponse = exec1.serialization.GetAllGraphTypeDescriptionsResponse;
  export import GraphTypeDescription = serialization.GraphTypeDescription;
  export import NodeTypeDescription = serialization.NodeTypeDescription;
  export import SocketTypeDescription = serialization.SocketTypeDescription;
}

// Export everything which is needed for GeneralInfoMessages (into a namespace!)
import { executionGraphGUI as exec2 } from './lib/GraphManipulationMessages_generated';
export namespace GraphManipulationMessages {
  export import AddNodeRequest = exec2.serialization.AddNodeRequest;
  export import AddNodeResponse = exec2.serialization.AddNodeResponse;

  export import RemoveNodeRequest = exec2.serialization.RemoveNodeRequest;

  export import NodeConstructionInfo = exec2.serialization.NodeConstructionInfo;

  export import LogicNode = serialization.LogicNode;
  export import LogicSocket = serialization.LogicSocket;
}

// Export everything which is needed for GraphManagementMessages (into a namespace!)
import { executionGraphGUI as exec3 } from './lib/GraphManagementMessages_generated';
export namespace GraphManagementMessages {
  export import AddGraphRequest = exec3.serialization.AddGraphRequest;
  export import AddGraphResponse = exec3.serialization.AddGraphResponse;

  export import RemoveGraphRequest = exec3.serialization.RemoveGraphRequest;
}
