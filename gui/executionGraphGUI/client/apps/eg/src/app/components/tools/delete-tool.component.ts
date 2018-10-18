import { Component, HostListener, OnInit, EventEmitter } from "@angular/core";
import { Store } from "@ngrx/store";
import { ToolComponent, KEY_CODE } from "./tool-component";
import { GraphsState } from "../../+state/reducers";
import { Observable } from "rxjs";
import { Selection } from "../../+state/reducers/ui.reducers";
import { withLatestFrom, map } from "rxjs/operators";
import { RemoveNode } from "../../+state/actions";
import { getSelection } from "../../+state/selectors/ui.selectors";
import { Graph } from "../../model";
import { getSelectedGraph } from "../../+state/selectors";

@Component({
    selector: 'eg-delete-tool',
    template: ``
})
export class DeleteToolComponent extends ToolComponent implements OnInit {

    private readonly deletePressed = new EventEmitter<void>();
    private readonly selection: Observable<Selection>;
    private readonly selectedGraph: Observable<Graph>;

    constructor(private store: Store<GraphsState>) {
        super();
        this.selection = store.select(getSelection);
        this.selectedGraph = store.select(getSelectedGraph);
    }

    ngOnInit() {
        this.deletePressed.pipe(withLatestFrom(this.selection, this.selectedGraph))
            .subscribe(([_, selection, graph]) => {
                for(const nodeId of selection.nodes) {
                    this.store.dispatch(new RemoveNode(graph.id, nodeId))
                }
            });
    }

    @HostListener('window:keydown', ['$event'])
    onKeyDown(keyEvent: KeyboardEvent) {
        if (keyEvent.keyCode === KEY_CODE.DELETE) {
            keyEvent.preventDefault();
            this.deletePressed.emit();
        }
    }
}
