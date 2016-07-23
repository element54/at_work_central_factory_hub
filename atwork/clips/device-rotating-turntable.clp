;---------------------------------------------------------------------------
;  device-conveyor-belt.clp - RoCKIn RefBox CLIPS - Conveyor belt interface
;
;  Licensed under BSD license, cf. LICENSE file
;---------------------------------------------------------------------------

(defrule net-recv-RotatingTurntableCommand-client
  ?mf <- (protobuf-msg (type "atwork_pb_msgs.RotatingTurntableCommand") (ptr ?p)
         (rcvd-via ?via) (rcvd-from ?host ?port))
  (network-client (id ?client-id) (host ?client-host) (port ?port))
  =>
  (retract ?mf) ; message will be destroyed after rule completes

  ; Get the command from the message
  (bind ?pb-command (pb-field-value ?p "command"))

  (switch ?pb-command
    (case RTT_STOP then
      (printout t "Client " ?client-id " (" ?client-host ":" ?port ") commands RTT to stop" crlf)
      (rotating-turntable-stop)
    )

    (case RTT_START then
      (printout t "Client " ?client-id " (" ?client-host ":" ?port ") commands RTT to start" crlf)
      (rotating-turntable-start)
    )
  )
)


