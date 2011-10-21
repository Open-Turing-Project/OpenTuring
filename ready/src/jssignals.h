// Signals from JavaSup to Ready
#define READY_NOTIFY_JAVA_PROGRAM_PAUSED	"ReadyNotifyJavaProgramPaused"
#define READY_NOTIFY_JAVA_PROGRAM_RESUMED	"ReadyNotifyJavaProgramResumed"
#define READY_NOTIFY_JAVA_PROGRAM_FINISHED	"ReadyNotifyJavaProgramFinished"
#define READY_NOTIFY_JAVA_PROGRAM_HALTED	"ReadyNotifyJavaProgramHalted"

#define READY_NOTIFY_TERMINATE			"ReadyNotifyTerminate"

#define READY_NOTIFY_SEND_OUTPUT		"ReadyNotifySendOutput"
#define READY_NOTIFY_PRINT_OUTPUT		"ReadyNotifyPrintOutput"
#define READY_NOTIFY_EXCEPTION_OUTPUT		"ReadyNotifyExceptionOutput"
#define READY_NOTIFY_FAIL_MESSAGE		"ReadyNotifyFailMessage"

// Signals from Ready to JavaSup
#define READY_MAIN_SAYS_PAUSE			"ReadyMainSaysPause"
#define READY_MAIN_SAYS_RESUME			"ReadyMainSaysResume"
#define READY_MAIN_SAYS_TERMINATE		"ReadyMainSaysTerminate"