//Make the node elements draggagle:
jsPlumb.ready(initializeJSPlumb);


function initializeJSPlumb() {

    var instance = window.jsp = jsPlumb.getInstance({
        // default drag options
        DragOptions: { cursor: 'pointer', zIndex: 2000 },
        // the overlays to decorate each connection with.  note that the label overlay uses a function to generate the label text; in this
        // case it returns the 'labelText' member that we set on each connection in the 'init' method below.
        ConnectionOverlays: [
            ["Arrow", {
                location: 1,
                visible: true,
                width: 11,
                length: 11,
                id: "arrow-overlay"
            }],
            ["Label", {
                location: 0.5,
                id: "label-overlay",
                cssClass: "label-overlay"
            }]
        ],
        Container: "workspace"
    });

    // this is the paint style for the connecting lines..
    var connectorPaintStyle = {
        strokeWidth: 3.5,
        stroke: "#61B7CF",
        joinstyle: "round",
        outlineStroke: null,
        outlineWidth: 2
    };
    // .. and this is the hover style.
    var connectorHoverStyle = {
        strokeWidth: 3,
        stroke: "#216477",
        outlineWidth: 5,
        outlineStroke: "#"
    };

    var endpointHoverStyle = {
        fill: "#216477",
        stroke: "#216477"
    };

    // the definition of source endpoints (the small blue ones)
    var radius = 6
    var sourceEndpoint = {
        endpoint: "Dot",
        paintStyle: {
            stroke: "#61B7CF",
            fill: "transparent",
            radius: radius,
            strokeWidth: 2
        },
        isSource: true,
        isTarget: true,
        connector: ["Bezier", { curviness: 150 /*stub: [40, 60], gap: 10, cornerRadius: 5, alwaysRespectStubs: true*/ }],
        connectorStyle: connectorPaintStyle,
        hoverPaintStyle: endpointHoverStyle,
        connectorHoverStyle: connectorHoverStyle,
        maxConnections: -1,
        overlays: [
            ["Label", {
                location: [0.5, 1.5],
                label: "Drag",
                cssClass: "endpointSourceLabel",
                visible: false
            }]
        ]
    };

    // the definition of target endpoints (will appear when the user drags a connection)
    var targetEndpoint = {
        endpoint: "Dot",
        paintStyle: { fill: "#61B7CF", radius: radius },
        hoverPaintStyle: endpointHoverStyle,
        maxConnections: -1,
        isSource: true,
        isTarget: true,
        connector: ["Bezier", { curviness: 150 }],
        connectorStyle: connectorPaintStyle,
        hoverPaintStyle: endpointHoverStyle,
        connectorHoverStyle: connectorHoverStyle,
        maxConnections: -1,
        overlays: [
            ["Label", { location: [0.5, -0.5], label: "Drop", cssClass: "endpointTargetLabel", visible: false }]
        ]
    };
    var initConnection = function (connection) {
        connection.getOverlay("label-overlay").setLabel(connection.sourceId + "-" + connection.targetId);
    };

    // Set endpoints to all input/output sockets
    var addExecutionNode = function (nodeId) {
        var id = null;
        var sockets = null;
        console.log("adding execution node: " , nodeId);

        var node = document.getElementById(nodeId);
        var groupId = "group-" + nodeId; // only for jsPlumb 
        var groupId = "group-" + nodeId; // only for jsPlumb 

        instance.draggable(node);

        // Inputs
        for(let pair of [["in", "LeftMiddle"], ["out", "RightMiddle"]])
        {
            var sockets = $(node).find("."+pair[0]+"puts" + " .socket");
            sockets.each(function (index, socket) {

                id = nodeId + "-" + pair[0] + socket.getAttribute("socketId")
                socket.setAttribute("id", id);

                console.log("adding socket: " , id);
                instance.addEndpoint(socket, targetEndpoint, {
                    anchor: pair[1], uuid: id
                });
            });
        }
    };

    // suspend drawing and initialise.
    instance.batch(function () {

        addExecutionNode("n1");

        // listen for new connections; initialise them the same way we initialise the connections at startup.
        instance.bind("connection", function (connInfo, originalEvent) {
            initConnection(connInfo.connection);
        });

        // // make all the node divs draggable
        // instance.draggable($("#workspace .group"), { grid: [20, 20] });

        // connect a few up
        // instance.connect({ uuids: ["n1-out1", "n2-in1"], editable: true });
        // instance.connect({ uuids: ["n2-out2", "n1-in1"], editable: true });

        // instance.bind("connectionDrag", function (connection) {
        //     console.log("connection " + connection.id + " is being dragged. suspendedElement is ",
        //         connection.suspendedElement, " of type ", connection.suspendedElementType);
        // });

        // instance.bind("connectionDragStop", function (connection) {
        //     console.log("connection " + connection.id + " was dragged");
        // });

        // instance.bind("connectionMoved", function (params) {
        //     console.log("connection " + params.connection.id + " was moved");
        // });
    });
} 