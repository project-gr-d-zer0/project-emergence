# Project startup: ensure the Remote Control HTTP (:30010) + WebSocket (:30020) servers are running,
# so the autonomous harness can always reach the editor. Idempotent; safe if already started.
import unreal
try:
    unreal.SystemLibrary.execute_console_command(None, "WebControl.StartServer")
    unreal.log("EMERGE_INIT: WebControl.StartServer issued (Remote Control :30010)")
except Exception as e:
    unreal.log_error("EMERGE_INIT: RC start failed " + repr(e))
