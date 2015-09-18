-module(uart4).
-export([start/0, stop/0]).
-export([check/0]).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%		API			%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%start the UART interface. 
start() ->
		PID = self(),
		spawn(fun()->init_uart_port(PID) end).
		
stop() -> io:format("stooping uart connection~n"), erl_port!stop.

check()-> erl_port!check.

init_uart_port(User_Pid) ->
	  PID = self(),
	  register(erl_port, PID),
    ExtPrg = "/home/ISG/uart/init_UART4",
    process_flag(trap_exit, true),
    Port = open_port({spawn, ExtPrg}, [{packet, 2}]),
    L = erlang:port_info(Port),
    F = fun({ATOM, VAL}) -> ATOM=:=os_pid end,
    [{os_pid, OS_PID}] = lists:filter(F,L),
    io:format("os pid is:~p~n",[OS_PID]), 
    loop(Port, User_Pid, OS_PID).
    
loop(Port, User_Pid, OS_PID)->
	%io:format("loooooooooping~n"),
	%sendByte(Port, 0, ?ASK_FOR_INCOMING),
	receive
	  	{_,{data, Data}} -> 
	  			io:format("got data:~p~n",[Data]), loop(Port, User_Pid, OS_PID);%, 
	  			%User_Pid!Data, loop(Port, User_Pid);
	  			
	  	{Port, closed} ->
		   		io:format("C prog closed normaly(no reason)~n"),
		   		close_c_port_program(OS_PID);
		   		
		  {'EXIT', Port, Reason} when Reason =/= normal ->
	    		io:format("error while initiating uart.Reason is:~p~n",[Reason]),
	    		close_c_port_program(OS_PID);
	    		
	    {'EXIT', Port, Reason} -> 
	    		io:format("c program closed. reason is:~p~n", [Reason]),
	    		close_c_port_program(OS_PID);
	    		
	    check -> 
	    		io:format("uart4.erl ok!~n"), loop(Port, User_Pid, OS_PID);
			stop ->
	   	 		Port ! {self(), close},
	   			receive
							{Port, closed} ->
		   					 io:format("C prog closed normaly~n"),
		   					 close_c_port_program(OS_PID),
		    					exit(normal)
	    		end;
	    
		 	Else -> io:format("at uart4: got somthing else: ~p~n",[Else]), loop(Port, User_Pid, OS_PID)	
	%after 10 -> loop(Port, User_Pid)
	end. 
	
		
close_c_port_program(OS_PID)->
		os:cmd("kill "++integer_to_list(OS_PID)),
		io:format("closed c program for real~n").
	
