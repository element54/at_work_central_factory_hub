;---------------------------------------------------------------------------
;  device-rotating-turntable.clp - RoCKIn RefBox CLIPS - Conveyor belt interface
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
    (case RTT_STOPPED then
      (printout t "Client " ?client-id " (" ?client-host ":" ?port ") commands RTT to stop" crlf)
      (rotating-turntable-stop)
    )

    (case RTT_RUNNING then
      (printout t "Client " ?client-id " (" ?client-host ":" ?port ") commands RTT to start" crlf)
      (rotating-turntable-start)
    )
  )
)

(deffunction net-create-RotatingTurntableStatus ()
  ; Instantiate a new status message
  (bind ?pb-status (pb-create "atwork_pb_msgs.RotatingTurntableStatus"))

    (if (rotating-turntable-is-connected)
    then
    (pb-set-field ?pb-status "connection_state" RTT_CONNECTED)
    else
    (pb-set-field ?pb-status "connection_state" RTT_NOT_CONNECTED)
    )
    (if (rotating-turntable-is-running)
     then
      (pb-set-field ?pb-status "state" RTT_RUNNING)
     else
      (pb-set-field ?pb-status "state" RTT_STOPPED)
    )

  (return ?pb-status)
)

(defrule net-send-RotatingTurntableStatus
  (time $?now)
  ?f <- (signal (type rotating-turntable) (time $?t&:(timeout ?now ?t ?*ROTATING-TURNTABLE-PERIOD*)) (seq ?seq))
  (network-peer (group "PUBLIC") (id ?peer-id-public))
  =>
  (modify ?f (time ?now) (seq (+ ?seq 1)))


  (bind ?status (net-create-RotatingTurntableStatus))

  ; Send to all clients
  (do-for-all-facts ((?client network-client)) TRUE
    (pb-send ?client:id ?status)
  )

  (pb-destroy ?status)
)
