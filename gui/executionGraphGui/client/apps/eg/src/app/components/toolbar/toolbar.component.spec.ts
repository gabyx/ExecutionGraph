import { async, ComponentFixture, TestBed } from '@angular/core/testing';
import { MatToolbarModule, MatMenuModule, MatIconModule } from '@angular/material';

import { ToolbarComponent } from './toolbar.component';
import { ExecutionService } from '../../services/ExecutionService';
import { DummyExecutionService } from '../../services/DummyExecutionService';

describe('ToolbarComponent', () => {
  let component: ToolbarComponent;
  let fixture: ComponentFixture<ToolbarComponent>;

  beforeEach(
    async(() => {
      TestBed.configureTestingModule({
        imports: [MatToolbarModule, MatMenuModule, MatIconModule],
        declarations: [ToolbarComponent],
        providers: [{ provide: ExecutionService, useClass: DummyExecutionService}]
      }).compileComponents();
    })
  );

  beforeEach(() => {
    fixture = TestBed.createComponent(ToolbarComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
