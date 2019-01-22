import { async, ComponentFixture, TestBed } from '@angular/core/testing';
import { MatProgressSpinnerModule, MatIconModule, MatListModule, MatMenuModule, MatDialogModule } from '@angular/material';
import { StoreModule } from '@ngrx/store';
import { LoggerFactory, SimpleConsoleLoggerFactory } from '@eg/logger';

import { GraphOpenComponent } from './graph-open.component';
import { FileBrowserComponent } from '../file-browser/file-browser.component';
import { reducers } from '../../+state/reducers/app.reducers';
import { FileBrowserService } from '../../services';
import { FileBrowserServiceDummy } from '../../services/FileBrowserServiceDummy';
import { VERBOSE_LOG_TOKEN } from '../../tokens';

describe('GraphOpenComponent', () => {
  let component: GraphOpenComponent;
  let fixture: ComponentFixture<GraphOpenComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      imports: [
        MatProgressSpinnerModule, MatIconModule, MatListModule, MatMenuModule, MatDialogModule,
        StoreModule.forRoot(reducers, { initialState: {} })
      ],
      declarations: [GraphOpenComponent, FileBrowserComponent],
      providers: [
        { provide: FileBrowserService, useClass: FileBrowserServiceDummy },
        { provide: LoggerFactory, useClass: SimpleConsoleLoggerFactory },
        { provide: VERBOSE_LOG_TOKEN, useValue: false }
      ]
    }).compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(GraphOpenComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
