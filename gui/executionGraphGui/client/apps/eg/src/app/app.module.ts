// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sun Jul 29 2018
//  @author Simon Spoerri, simon (døt) spoerri (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { Route, RouterModule } from '@angular/router';
import { HttpClientModule } from '@angular/common/http';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import {
  MatIconModule,
  MatToolbarModule,
  MatMenuModule,
  MatButtonModule,
  MatDividerModule,
  MatExpansionModule,
  MatCheckboxModule,
  MatButtonToggleModule,
  MatSidenavModule,
  MatFormFieldModule,
  MatCardModule,
  MatGridListModule,
  MatSnackBarModule,
  MatTabsModule,
  MatListModule,
  MatSliderModule,
  MatProgressSpinnerModule,
  MAT_DIALOG_DEFAULT_OPTIONS,
  MatDialogModule,
  MatInputModule
} from '@angular/material';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';

import { NxModule } from '@nrwl/nx';
import { RouterStateSerializer, StoreRouterConnectingModule } from '@ngrx/router-store';
import { StoreDevtoolsModule } from '@ngrx/store-devtools';
import { StoreModule } from '@ngrx/store';
import { EffectsModule } from '@ngrx/effects';
import { storeFreeze } from 'ngrx-store-freeze';

import { VERBOSE_LOG_TOKEN, BINARY_HTTP_ROUTER_BASE_URL } from './tokens';

import { GraphModule } from '@eg/graph';
import { CommonModule } from '@eg/common';

import { SimpleConsoleLoggerFactory, LoggerFactory } from '@eg/logger';

import { BinaryHttpRouterService } from './services/BinaryHttpRouterService';

import { ExecutionService } from './services/ExecutionService';
import { ExecutionServiceBinaryHttp } from './services/ExecutionServiceBinaryHttp';
import { ExecutionServiceDummy } from './services/ExecutionServiceDummy';

import { GeneralInfoService } from './services/GeneralInfoService';
import { GeneralInfoServiceDummy } from './services/GeneralInfoServiceDummy';
import { GeneralInfoServiceBinaryHttp } from './services/GeneralInfoServiceBinaryHttp';

import { GraphManipulationService } from './services/GraphManipulationService';
import { GraphManipulationServiceBinaryHttp } from './services/GraphManipulationServiceBinaryHttp';
import { GraphManipulationServiceDummy } from './services/GraphManipulationServiceDummy';

import { GraphSerializationService } from './services/GraphSerializationService';
import { GraphSerializationServiceBinaryHttp } from './services/GraphSerializationServiceBinaryHttp';
import { GraphSerializationServiceDummy } from './services/GraphSerializationServiceDummy';

import { GraphManagementService } from './services/GraphManagementService';
import { GraphManagementServiceBinaryHttp } from './services/GraphManagementServiceBinaryHttp';
import { GraphManagementServiceDummy } from './services/GraphManagementServiceDummy';

import { FileBrowserService } from './services/FileBrowserService';
import { FileBrowserServiceDummy } from './services/FileBrowserServiceDummy';
import { FileBrowserServiceBinaryHttp } from './services/FileBrowserServiceBinaryHttp';

import { TestService } from './services/TestService';
import { ITestBackend, TestBackend } from './services/TestBackend';

import { AppComponent } from './app.component';
import { ToolbarComponent } from './components/toolbar/toolbar.component';
import { WorkspaceComponent } from './components/workspace/workspace.component';
import { InspectorComponent } from './components/inspector/inspector.component';

import { ConnectionStyleOptionsComponent } from './components/connection-style-options/connection-style-options.component';
import { GraphCreateComponent } from './components/graph-create/graph-create.component';

import { reducers, RouterStateUrlSerializer } from './+state/reducers/app.reducers';
import { effects } from './+state/effects';

import { environment } from '../environments/environment';
import { GraphLoadedGuard } from './guards/graphLoaded.guard';
import { BackendTestComponent } from './components/backend-test/backend-test.component';
import { FileBrowserComponent } from './components/file-browser/file-browser.component';
import { AddNodeComponent } from './components/add-node/add-node.component';
import { SelectionToolComponent } from './components/tools/selection-tool/selection-tool.component';
import { NavigationToolComponent } from './components/tools/navigation-tool/navigation-tool.component';
import { SocketConnectionToolComponent } from './components/tools/socket-connection-tool/socket-connection-tool.component';
import { ConnectionLayerComponent } from './components/connection-layer/connection-layer.component';
import { MoveToolComponent } from './components/tools/move-tool/move-tool.component';
import { DeleteToolComponent } from './components/tools/delete-tool/delete-tool.component';
import { TypeToolTipToolComponent } from './components/tools/type-tooltip-tool/type-tooltip-tool.component';
import { GraphOpenComponent } from './components/graph-open/graph-open.component';
import { ConfirmationDialogComponent } from './components/confirmation-dialog/confirmation-dialog.component';
import { AutoLayoutService } from './services/AutoLayoutService';
import { GraphSaveComponent } from './components/graph-save/graph-save.component';

const routes: Route[] = [
  {
    path: 'graph',
    children: [
      { path: 'open', component: GraphOpenComponent, outlet: 'primary' },
      { path: 'save', component: GraphSaveComponent, outlet: 'primary' },
      { path: 'new', component: GraphCreateComponent, outlet: 'primary' },
      {
        path: ':graphId',
        component: WorkspaceComponent,
        canActivate: [GraphLoadedGuard],
        children: [],
        outlet: 'primary'
      }
    ]
  },
  { path: 'inspector', component: InspectorComponent, outlet: 'drawer' },
  { path: 'lines', component: ConnectionStyleOptionsComponent, outlet: 'drawer' },
  { path: 'nodes', component: AddNodeComponent, outlet: 'drawer' },
  { path: 'backend-test', component: BackendTestComponent, outlet: 'drawer' },

  // { path: 'file-browser', component: FileBrowserComponent, outlet: 'bottom-dialog' },

  { path: '**', redirectTo: 'graph/new' }
];

@NgModule({
  declarations: [
    AppComponent,
    ConfirmationDialogComponent,
    ToolbarComponent,
    WorkspaceComponent,
    ConnectionStyleOptionsComponent,
    InspectorComponent,
    GraphCreateComponent,
    FileBrowserComponent,
    BackendTestComponent,
    AddNodeComponent,
    SelectionToolComponent,
    NavigationToolComponent,
    MoveToolComponent,
    SocketConnectionToolComponent,
    TypeToolTipToolComponent,
    DeleteToolComponent,
    ConnectionLayerComponent,
    GraphOpenComponent,
    GraphSaveComponent
  ],
  entryComponents: [ConfirmationDialogComponent],
  imports: [
    HttpClientModule,
    BrowserModule,
    BrowserAnimationsModule,
    MatButtonModule,
    MatListModule,
    MatDividerModule,
    MatExpansionModule,
    MatButtonToggleModule,
    MatCheckboxModule,
    MatCardModule,
    MatGridListModule,
    MatIconModule,
    MatListModule,
    MatMenuModule,
    MatFormFieldModule,
    MatProgressSpinnerModule,
    MatSidenavModule,
    MatSliderModule,
    MatSnackBarModule,
    MatTabsModule,
    MatToolbarModule,
    MatDialogModule,
    MatInputModule,
    FormsModule,
    ReactiveFormsModule,
    GraphModule,
    CommonModule,
    NxModule.forRoot(),
    RouterModule.forRoot(routes, { enableTracing: false }),
    StoreRouterConnectingModule.forRoot({}),
    StoreModule.forRoot(reducers, {
      initialState: {}
      //metaReducers : !environment.production ? [storeFreeze] : []
    }),
    EffectsModule.forRoot(effects),
    environment.production ? [] : StoreDevtoolsModule.instrument({ serialize: false })
  ],
  providers: [
    { provide: VERBOSE_LOG_TOKEN, useValue: environment.logReponsesVerbose },
    { provide: BINARY_HTTP_ROUTER_BASE_URL, useValue: environment.backendUrl },
    { provide: MAT_DIALOG_DEFAULT_OPTIONS, useValue: { hasBackdrop: false } },
    { provide: LoggerFactory, useClass: SimpleConsoleLoggerFactory },
    { provide: ITestBackend, useClass: TestBackend },
    BinaryHttpRouterService,
    TestService,
    GraphLoadedGuard,
    {
      provide: ExecutionService,
      useClass: !environment.useServiceDummys ? ExecutionServiceBinaryHttp : ExecutionServiceDummy
    },
    {
      provide: GeneralInfoService,
      useClass: !environment.useServiceDummys ? GeneralInfoServiceBinaryHttp : GeneralInfoServiceDummy
    },
    {
      provide: GraphManipulationService,
      useClass: !environment.useServiceDummys ? GraphManipulationServiceBinaryHttp : GraphManipulationServiceDummy
    },
    {
      provide: GraphManagementService,
      useClass: !environment.useServiceDummys ? GraphManagementServiceBinaryHttp : GraphManagementServiceDummy
    },
    {
      provide: GraphSerializationService,
      useClass: !environment.useServiceDummys ? GraphSerializationServiceBinaryHttp : GraphSerializationServiceDummy
    },
    {
      provide: FileBrowserService,
      useClass: !environment.useServiceDummys ? FileBrowserServiceBinaryHttp : FileBrowserServiceDummy
    },
    { provide: AutoLayoutService, useClass: AutoLayoutService },
    { provide: RouterStateSerializer, useClass: RouterStateUrlSerializer }
  ],
  bootstrap: [AppComponent]
})
export class AppModule {}
