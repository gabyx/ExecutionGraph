// ========================================================================================
//  ExecutionGraph
//  Copyright (C) 2017 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================

#ifndef ExecutionGraph_Nodes_DummyNode_hpp
#define ExecutionGraph_Nodes_DummyNode_hpp

namespace execGraph
{
class DummyNode
{
public:
    struct Inputs
    {
        enum
        {
            Enable,
            INPUTS_LAST
        };
    };

    struct Outputs
    {
        enum
        {
            OUTPUTS_LAST
        };
    };

    enum
    {
        N_INPUTS  = Inputs::INPUTS_LAST,
        N_OUTPUTS = Outputs::OUTPUTS_LAST - Inputs::INPUTS_LAST,
        N_SOCKETS = N_INPUTS + N_OUTPUTS
    };

    DummyNode(unsigned int id);

    ~DummyNode();
};
};

#endif
