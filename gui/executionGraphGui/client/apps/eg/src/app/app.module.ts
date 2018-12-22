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
import { HttpClientModule, HttpClient } from '@angular/common/http';
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
  MatCardModule,
  MatGridListModule,
  MatSnackBarModule,
  MatTabsModule,
  MatListModule,
  MatSliderModule
} from '@angular/material';
import { NxModule } from '@nrwl/nx';
import { RouterStateSerializer, StoreRouterConnectingModule } from '@ngrx/router-store';
import { StoreDevtoolsModule } from '@ngrx/store-devtools';
import { StoreModule } from '@ngrx/store';
import { EffectsModule } from '@ngrx/effects';
import { storeFreeze } from 'ngrx-store-freeze';

import { VERBOSE_LOG_TOKEN, BINARY_HTTP_ROUTER_BASE_URL} from './tokens';

import { GraphModule } from '@eg/graph';
import { SimpleConsoleLoggerFactory, LoggerFactory } from '@eg/logger';

import { BinaryHttpRouterService } from './services/BinaryHttpRouterService';
import { CefMessageRouterService } from './services/CefMessageRouterService';

import { ExecutionService } from './services/ExecutionService';
import { ExecutionServiceBinaryHttp } from './services/ExecutionServiceBinaryHttp';
import { ExecutionServiceDummy } from './services/ExecutionServiceDummy';

import { GeneralInfoService } from './services/GeneralInfoService';
import { GeneralInfoServiceDummy } from './services/GeneralInfoServiceDummy';
import { GeneralInfoServiceBinaryHttp } from './services/GeneralInfoServiceBinaryHttp';

import { GraphManipulationService } from './services/GraphManipulationService';
import { GraphManipulationServiceBinaryHttp } from './services/GraphManipulationServiceBinaryHttp';
import { GraphManipulationServiceDummy } from './services/GraphManipulationServiceDummy';

import { GraphManagementService } from './services/GraphManagementService';
import { GraphManagementServiceBinaryHttp } from './services/GraphManagementServiceBinaryHttp';
import { GraphManagementServiceDummy } from './services/GraphManagementServiceDummy';

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
import { AddNodeComponent } from './components/add-node/add-node.component'
import { SelectionToolComponent } from './components/tools/selection-tool/selection-tool.component';
import { NavigationToolComponent } from './components/tools/navigation-tool/navigation-tool.component';
import { SocketConnectionToolComponent } from './components/tools/socket-connection-tool/socket-connection-tool.component';
import { ConnectionLayerComponent } from './components/connection-layer/connection-layer.component';
import { MoveToolComponent } from './components/tools/move-tool/move-tool.component';
import { DeleteToolComponent } from './components/tools/delete-tool/delete-tool.component';

//environment.production = true;

const routes: Route[] = [
  {
    path: 'graph',
    children: [
      { path: 'new', component: GraphCreateComponent },
      {
        path: ':graphId',
        component: WorkspaceComponent,
        canActivate: [GraphLoadedGuard],
        children: [
        ]
      },
    ],
  },
  { path: 'inspector', component: InspectorComponent, outlet: 'drawer' },
  { path: 'lines', component: ConnectionStyleOptionsComponent, outlet: 'drawer' },
  { path: 'nodes', component: AddNodeComponent, outlet: 'drawer' },
  { path: 'backend-test', component: BackendTestComponent, outlet: 'drawer' },
  { path: '**', redirectTo: 'graph/new' },
];

@NgModule({
  declarations: [
    AppComponent,
    ToolbarComponent,
    WorkspaceComponent,
    ConnectionStyleOptionsComponent,
    InspectorComponent,
    GraphCreateComponent,
    BackendTestComponent,
    AddNodeComponent,
    SelectionToolComponent,
    NavigationToolComponent,
    MoveToolComponent,
    SocketConnectionToolComponent,
    DeleteToolComponent,
    ConnectionLayerComponent
  ],
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
    MatSidenavModule,
    MatSliderModule,
    MatSnackBarModule,
    MatTabsModule,
    MatToolbarModule,
    GraphModule,
    NxModule.forRoot(),
    RouterModule.forRoot(routes, { enableTracing: false}),
    StoreRouterConnectingModule.forRoot({}),
    StoreModule.forRoot(
      reducers,
      {
        initialState: { }
        //metaReducers : !environment.production ? [storeFreeze] : []
      }
    ),
    EffectsModule.forRoot(effects),
    environment.production ? [] : StoreDevtoolsModule.instrument()
  ],
  providers: [
    { provide: VERBOSE_LOG_TOKEN, useValue: environment.logReponsesVerbose },
    { provide: BINARY_HTTP_ROUTER_BASE_URL, useValue: environment.backendUrl },
    { provide: LoggerFactory, useClass: SimpleConsoleLoggerFactory },
    { provide: ITestBackend, useClass: TestBackend },
    BinaryHttpRouterService,
    CefMessageRouterService,
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
    { provide: RouterStateSerializer, useClass: RouterStateUrlSerializer }
  ],
  bootstrap: [AppComponent]
})
export class AppModule {}
