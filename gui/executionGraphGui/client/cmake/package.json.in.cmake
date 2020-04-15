{
  "name": "eg",
  "version": "1.0.0",
  "license": "MPL 2.0",
  "author": "Gabriel Nützi, Simon Spörri",
  "description": "Build and execute data flow graphs",
  "private": true,
  "build": {
    "appId": "executionGraphGui",
    "productName": "ExecutionGraphGui",
    "copyright": "Copyright © 2018 ${author}",
    "directories": {
      "output": "package"
    },
    "files": [
      "build"
    ],
    "extraResources": [],
    "extraFiles" : [
      {
        "from": "build/server",
        "to": "server"
      }
    ],
    "linux": {
      "target": "tar.gz",
      "icon": "@ExecutionGraph_CLIENT_DIR@/apps/electron/resources/icon.png"
    },
    "mac": {
      "target": "dmg",
      "category": "com.executiongraph.app",
      "icon": "@ExecutionGraph_CLIENT_DIR@/apps/electron/resources/icon.png"
    },
    "win": {
      "target": "zip",
      "icon": "@ExecutionGraph_CLIENT_DIR@/apps/electron/resources/icon.png"
    }
  },
  "main": "build/main.js"
}
