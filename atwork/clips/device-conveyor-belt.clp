;---------------------------------------------------------------------------
;  device-conveyor-belt.clp - RoCKIn RefBox CLIPS - Conveyor belt interface
;
;  Licensed under BSD license, cf. LICENSE file
;---------------------------------------------------------------------------

(defrule net-recv-ConveyorBeltCommand-client
  ?mf <- (protobuf-msg (type "atwork_pb_msgs.ConveyorBeltCommand") (ptr ?p)
         (rcvd-via ?via) (rcvd-from ?host ?port))
  (network-client (id ?client-id) (host ?client-host) (port ?port))
  =>
  (retract ?mf) ; message will be destroyed after rule completes

  ; Get the command from the message
  (bind ?pb-command (pb-field-value ?p "command"))

  (switch ?pb-command
    (case CB_STOPPED then
      (printout t "Client " ?client-id " (" ?client-host ":" ?port ") commands CB to stop" crlf)
      (conveyor-belt-stop)
    )

    (case CB_RUNNING then
      (printout t "Client " ?client-id " (" ?client-host ":" ?port ") commands CB to start" crlf)
      (conveyor-belt-start)
    )
  )
)

(deffunction net-create-ConveyorBeltStatus ()
  ; Instantiate a new status message
  (bind ?pb-status (pb-create "atwork_pb_msgs.ConveyorBeltStatus"))

    (if (conveyor-belt-is-connected)
    then
    (pb-set-field ?pb-status "connection_state" CB_CONNECTED)
    else
    (pb-set-field ?pb-status "connection_state" CB_NOT_CONNECTED)
    )
    (if (conveyor-belt-is-running)
     then
      (pb-set-field ?pb-status "state" CB_RUNNING)
     else
      (pb-set-field ?pb-status "state" CB_STOPPED)
    )

  (return ?pb-status)
)

(defrule net-send-ConveyorBeltStatus
  (time $?now)
  ?f <- (signal (type conveyor-belt) (time $?t&:(timeout ?now ?t ?*CONVEYOR-BELT-PERIOD*)) (seq ?seq))
  (network-peer (group "PUBLIC") (id ?peer-id-public))
  =>
  (modify ?f (time ?now) (seq (+ ?seq 1)))


  (bind ?status (net-create-ConveyorBeltStatus))

  ; Send to all clients
  (do-for-all-facts ((?client network-client)) TRUE
    (pb-send ?client:id ?status)
  )

  (pb-destroy ?status)
)
