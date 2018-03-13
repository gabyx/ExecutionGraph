import { HttpClientModule } from '@angular/common/http';

import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { MatIconModule, MatToolbarModule, MatMenuModule, MatButtonModule, MatCheckboxModule } from '@angular/material';

import { GraphModule } from '@eg/graph';

import { CefMessageRouterService } from './services/CefMessageRouterService';
import { CefBinaryRouterService } from './services/CefBinaryRouterService';

import { ExecutionService } from './services/ExecutionService';
import { CefExecutionService } from './services/CefExecutionService';
import { DummyExecutionService } from './services/DummyExecutionService';

import { AppComponent } from './app.component';
import { ToolbarComponent } from './components/toolbar/toolbar.component';
import { WorkspaceComponent } from './components/workspace/workspace.component';

import { environment } from '../environments/environment';

@NgModule({
  declarations: [AppComponent, ToolbarComponent, WorkspaceComponent],
  imports: [
    HttpClientModule,
    BrowserModule,
    BrowserAnimationsModule,
    MatIconModule,
    MatToolbarModule,
    MatMenuModule,
    MatButtonModule,
    MatCheckboxModule,
    GraphModule
  ],
  providers: [
    CefMessageRouterService,
    CefBinaryRouterService,
    { provide: ExecutionService, useClass: environment.production ? CefExecutionService : DummyExecutionService }
  ],
  bootstrap: [AppComponent]
})
export class AppModule {}
