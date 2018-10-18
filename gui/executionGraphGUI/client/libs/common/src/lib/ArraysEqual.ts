import { isDefaultChangeDetectionStrategy } from "@angular/core/src/change_detection/constants";
import { isDefined } from "@eg/common/src/lib/IsDefined";

export function arraysEqual<T>(a: T[], b: T[]): boolean {
    if(a === b) {
        return true;
    }
    if(b===null || !isDefined(b)) {
        return false;
    }
    if(a.length !== b.length) {
        return false;
    }

    for(let i=0;i<a.length;i++) {
        if(a[i]!==b[i]) {
            return false;
        }
    }

    return true;
}