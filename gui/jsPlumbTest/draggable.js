//Make the node elements draggagle:
document.addEventListener("DOMContentLoaded", function () {
    $(".draggable").each(function (index, el) {
        dragElement(el);
    });
});


function dragElement(elmnt) {
    var posX = 0, posY = 0, cursorX = 0, cursorY = 0;

    dragTarget = $(elmnt).find(".dragTarget")

    if (dragTarget) {
        /* if present, the header is where you move the DIV from:*/
        dragTarget.on("mousedown", dragMouseDown);
    } else {
        /* otherwise, move the DIV from anywhere inside the DIV:*/
        dragTarget.on("mousedown", dragMouseDown);
    }

    function dragMouseDown(e) {
        e = e || window.event;
        // get the mouse cursor position at startup:
        cursorX = e.clientX;
        cursorY = e.clientY;
        console.log("start dragging")
        document.onmouseup = closeDragElement;
        // call a function whenever the cursor moves:
        document.onmousemove = elementDrag;
    }

    function elementDrag(e) {
        e = e || window.event;
        // calculate the new cursor position:
        posX = cursorX - e.clientX;
        posY = cursorY - e.clientY;
        cursorX = e.clientX;
        cursorY = e.clientY;
        console.log("dragging: ", cursorX, cursorY)
        // set the element's new position:
        elmnt.style.top = (elmnt.offsetTop - posY) + "px";
        elmnt.style.left = (elmnt.offsetLeft - posX) + "px";
    }

    function closeDragElement() {
        /* stop moving when mouse button is released:*/
        document.onmouseup = null;
        document.onmousemove = null;
        console.log("stop dragging")
    }
}