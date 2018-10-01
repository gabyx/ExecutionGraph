import { Component, OnInit } from '@angular/core';
import { dispatch } from 'rxjs/internal/observable/range';
import { Store } from '@ngrx/store';
import { AppState, fromAppActions } from '../../+state';

@Component({
  selector: 'eg-inspector',
  templateUrl: './inspector.component.html',
  styleUrls: ['./inspector.component.scss']
})
export class InspectorComponent implements OnInit {
  constructor(private store: Store<AppState>) {}

  ngOnInit() {}

  public closeInspector() {
    this.store.dispatch(new fromAppActions.UpdateUIProperties({ inspector: { isVisible: false } }));
  }
}
