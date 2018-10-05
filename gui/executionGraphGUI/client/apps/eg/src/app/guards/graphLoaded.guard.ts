import { Injectable } from '@angular/core';
import { CanActivate } from '@angular/router';
import { Store } from '@ngrx/store';
import { Observable } from 'rxjs';
import { map, take } from 'rxjs/operators';

import { GraphsState } from '../+state/reducers';
import { getSelectedGraphId, getSelectedGraph } from '../+state/selectors';
import { isDefined } from '@eg/common/src';

@Injectable()
export class GraphLoadedGuard implements CanActivate {
    constructor(private store: Store<GraphsState>){}

    canActivate(): Observable<boolean> {
        return this.store.select(getSelectedGraph)
            .pipe(map(graph => isDefined(graph)), take(1));
    }


}