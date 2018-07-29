import { TestBed, async } from '@angular/core/testing';
import { MatIconModule, MatToolbarModule, MatMenuModule, MatCardModule } from '@angular/material';
import { GraphModule } from '@eg/graph';

import { AppComponent } from './app.component';
import { ToolbarComponent } from './components/toolbar/toolbar.component';
import { WorkspaceComponent } from './components/workspace/workspace.component';
import { ExecutionService } from './services/ExecutionService';
import { ExecutionServiceDummy } from './services/ExecutionServiceDummy';

describe('AppComponent', () => {
  beforeEach(
    async(() => {
      TestBed.configureTestingModule({
        imports: [GraphModule, MatIconModule, MatToolbarModule, MatMenuModule, MatCardModule],
        declarations: [AppComponent, ToolbarComponent, WorkspaceComponent],
        providers: [{ provide: ExecutionService, useClass: ExecutionServiceDummy }]
      }).compileComponents();
    })
  );
  it(
    'should create the app',
    async(() => {
      const fixture = TestBed.createComponent(AppComponent);
      const app = fixture.debugElement.componentInstance;
      expect(app).toBeTruthy();
    })
  );
  it(
    `should have as title 'app'`,
    async(() => {
      const fixture = TestBed.createComponent(AppComponent);
      const app = fixture.debugElement.componentInstance;
      expect(app.title).toEqual('app');
    })
  );
  it(
    'should render title in a h1 tag',
    async(() => {
      const fixture = TestBed.createComponent(AppComponent);
      fixture.detectChanges();
      const compiled = fixture.debugElement.nativeElement;
      expect(compiled.querySelector('h1').textContent).toContain('Execution Graph');
    })
  );
});
