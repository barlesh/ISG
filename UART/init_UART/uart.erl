-module(uart).
-export([start/0, stop/0]).
-export([send/2]).

-define(OK, 1).
-define(ERROR, -1).


%start the UART interface. spawn 2 processes.
%1) receiver - a process whice will get th UART incoming transmition and will send it to caller's inbox
%2) transmitter - will sent outcoming transmition, using uart:send/2 
start() ->
		PID = self(),
   % ExtPrg = "/home/ISG/uart/init_UART",
    %process_flag(trap_exit, true),
    %Port = open_port({spawn, ExtPrg}, [{packet, 2}]),
    %receive			%%wait for c uart init program to send ack about initiation
    	%	{Port, closed} ->
		   % 		io:format("initiated as expected~n");
		    %{'EXIT', Port, Reason} when Reason =/= normal ->
	    		%	io:format("error while initiating 
%uart.Reason is:~p~n",[Reason]), exit(-1);
	%	 {'EXIT', Port, Reason} -> io:format("initiated~n")
	 % end,
    REC_PID = spawn(fun() -> receiver:start(PID) end),	%%registered itself as 'receiver'
    TRAN_PID = spawn(fun() -> transmitter:start() end), %%registered itself as 'transmitter'
    REC_REF = monitor(process, REC_PID), TRAN_REF = monitor(process, TRAN_PID),
    PID_MON = spawn(fun()-> monitor_processes(REC_REF, TRAN_REF, PID) end),
    register(monitor_process, PID_MON).
    
    
%this process monitor the uart's transmitting & receiving services.
%in case thay crash, it get a massage and try to run them again
monitor_processes(REC_REF, TRAN_REF, CALLER_PID) ->
		receive
			{'DOWN', REC_REF, process, Pid, Reason} when Reason =/= normal -> 
					io:format("Process Pid ~p terminated unexpected. re-spawn it~n",[Pid]),
					REC_PID = spawn(fun() -> receiver:start(CALLER_PID) end), NEW_REC_REF = monitor(process, REC_PID),
					monitor_processes(NEW_REC_REF, TRAN_REF, CALLER_PID);
			{'DOWN', TRAN_REF, process, Pid, Reason} when Reason =/= normal -> 
					io:format("Process Pid ~p terminated unexpected. re-spawn it~n",[Pid]),
					TRAN_PID = spawn(fun() -> transmitter:start() end), NEW_TRAN_REF = monitor(process, TRAN_PID),
					monitor_processes(REC_REF, NEW_TRAN_REF, CALLER_PID);
			stop -> stop
			end.


stop() ->
		monitor_process ! stop,
    transmitter ! stop,
    receiver ! stop.

send(Channel, Payload) ->
		Msg = "a" ++ "b" ++ Payload,		%%TODO - change "a" to Channel and "b" to 0
		transmitter ! {call, self(), Msg},
		io:format("at uart:send Msg sent is:~p~n", [Msg]),
		receive 
			ok -> ok;
			error -> error
		end.
