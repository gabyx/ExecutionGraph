import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { MatIconModule, MatToolbarModule, MatMenuModule, MatButtonModule, MatCheckboxModule } from '@angular/material';

import { CefMessageRouterService } from './services/CefMessageRouterService';

import { AppComponent } from './app.component';
import { ToolbarComponent } from './toolbar/toolbar.component';
import { WorkspaceComponent } from './workspace/workspace.component';


@NgModule({
  declarations: [
    AppComponent,
    ToolbarComponent,
    WorkspaceComponent
  ],
  imports: [
    BrowserModule,
    BrowserAnimationsModule,
    MatIconModule, MatToolbarModule, MatMenuModule, MatButtonModule, MatCheckboxModule
  ],
  providers: [
    CefMessageRouterService
  ],
  bootstrap: [AppComponent]
})
export class AppModule { }
