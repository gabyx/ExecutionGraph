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
import { executionGraphGui as exec1 } from './lib/GeneralInfoMessages_generated';
import * as serialization from '@eg/serialization';

export namespace GeneralInfoMessages {
  export import GetAllGraphTypeDescriptionsResponse = exec1.serialization.GetAllGraphTypeDescriptionsResponse;
  export import GraphTypeDescription = serialization.GraphTypeDescription;
  export import NodeTypeDescription = serialization.NodeTypeDescription;
  export import SocketTypeDescription = serialization.SocketTypeDescription;
}

// Export everything which is needed for GeneralInfoMessages (into a namespace!)
import { executionGraphGui as exec2 } from './lib/GraphManipulationMessages_generated';
export namespace GraphManipulationMessages {
  export import AddNodeRequest = exec2.serialization.AddNodeRequest;
  export import AddNodeResponse = exec2.serialization.AddNodeResponse;

  export import RemoveNodeRequest = exec2.serialization.RemoveNodeRequest;

  export import AddConnectionRequest = exec2.serialization.AddConnectionRequest;
  export import AddConnectionResponse = exec2.serialization.AddConnectionResponse;

  export import RemoveConnectionRequest = exec2.serialization.RemoveConnectionRequest;

  export import NodeConstructionInfo = exec2.serialization.NodeConstructionInfo;

  export import LogicNode = serialization.LogicNode;
  export import LogicSocket = serialization.LogicSocket;
  export import SocketLinkDescription = serialization.SocketLinkDescription;
}

// Export everything which is needed for GraphManagementMessages (into a namespace!)
import { executionGraphGui as exec3 } from './lib/GraphManagementMessages_generated';
export namespace GraphManagementMessages {
  export import AddGraphRequest = exec3.serialization.AddGraphRequest;
  export import AddGraphResponse = exec3.serialization.AddGraphResponse;

  export import RemoveGraphRequest = exec3.serialization.RemoveGraphRequest;
}

// Export everything which is needed for FileBrowserMessages (into a namespace!)
import { executionGraphGui as exec4 } from './lib/FileBrowserMessages_generated';
import { executionGraphGui as exec5 } from './lib/FileSystemInfo_generated';
export namespace FileBrowserMessages {
  export import BrowseRequest = exec4.serialization.BrowseRequest;
  export import BrowseResponse = exec4.serialization.BrowseResponse;

  export import PathInfo = exec5.serialization.PathInfo;
  export import Date = exec5.serialization.Date;
  export import Permission = exec5.serialization.Permissions;
}
