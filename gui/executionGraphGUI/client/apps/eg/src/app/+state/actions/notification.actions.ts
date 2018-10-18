import { Action } from "@ngrx/store";

export const SHOW_NOTIFICATION = '[Notification] Show';

export class ShowNotification implements Action {
    readonly type = SHOW_NOTIFICATION;

    constructor(public text: string, public showTimeMs: number = 2000) {}
}

export type NotificationActions = ShowNotification;