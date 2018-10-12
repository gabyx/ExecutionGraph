export class PositionService {

    public static getPositionRelativeToParent(element: HTMLElement, referenceParent?: HTMLElement) {
        let offsetLeft = element.offsetLeft; //+ element.offsetWidth / 2;
        let offsetTop = element.offsetTop; // + element.offsetHeight / 2;

        while (element.offsetParent && element.offsetParent !== referenceParent) {
            element = element.offsetParent as HTMLElement;
            offsetLeft += element.offsetLeft;
            offsetTop += element.offsetTop;
        }

        return {
            x: offsetLeft,
            y: offsetTop
        };
    }

}