import { HttpClientModule } from '@angular/common/http';

import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { MatIconModule, MatToolbarModule, MatMenuModule, MatButtonModule, MatCheckboxModule, MatButtonToggleModule } from '@angular/material';

import { GraphModule } from '@eg/graph';

import { CefMessageRouterService } from './services/CefMessageRouterService';

import { ExecutionService } from './services/ExecutionService';
import { BinaryHttpExecutionService } from './services/BinaryHttpExecutionService';
import { DummyExecutionService } from './services/DummyExecutionService';

import { AppComponent } from './app.component';
import { ToolbarComponent } from './components/toolbar/toolbar.component';
import { WorkspaceComponent } from './components/workspace/workspace.component';

import { environment } from '../environments/environment';
import { ConnectionStyleOptionsComponent } from './components/connection-style-options/connection-style-options.component';

@NgModule({
  declarations: [AppComponent, ToolbarComponent, WorkspaceComponent, ConnectionStyleOptionsComponent],
  imports: [
    HttpClientModule,
    BrowserModule,
    BrowserAnimationsModule,
    MatIconModule,
    MatToolbarModule,
    MatMenuModule,
    MatButtonModule,
    MatButtonToggleModule,
    MatCheckboxModule,
    GraphModule
  ],
  providers: [
    CefMessageRouterService,
    { provide: ExecutionService, useClass: environment.production ? BinaryHttpExecutionService : DummyExecutionService }
  ],
  bootstrap: [AppComponent]
})
export class AppModule {}
