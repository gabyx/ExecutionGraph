
import { Appender, BaseAppender, LogEvent } from "ts-log-debug";
const consoleLog = console.log.bind(console);

@Appender({ name: "consoleAppender" })
export class ConsoleAppender extends BaseAppender {
  write(logEvent: LogEvent) {
    consoleLog(this.layout(logEvent, this.config.timezoneOffset));
  }
}
