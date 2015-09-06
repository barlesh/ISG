-module(transmitter).
-export([start/0, stop/0]).
-export([sendMsg/1]).

-define(START_BYTE, 0).
-define(END_BYTE, 255).
-define(REG_BYTE, 1).
-define(OK, 1).
-define(ERROR, -1).

start() ->
    ExtPrg = "/home/barlesh/ISG/erlang_to_c/c_transmitter",
    spawn(fun() -> init_receiver() end),
    spawn(fun() -> init_porter(ExtPrg) end).
     
stop() ->
    complex ! stop,
    receiver!stop.

%%this function is part of the module API. it sent the contant of MSG list (of bytes) to C program
sendMsg(MSG) ->
	[H|T] = MSG,
	sendByte(H, ?START_BYTE),
	sendMSG2(T).
sendMSG2([H|T]) when T =:= [] -> 
	sendByte(H, ?END_BYTE);
sendMSG2([H|T]) -> 
	sendByte(H, ?REG_BYTE),
	sendMSG2(T).
	
sendByte(Byte, Type) -> 
	io:format("sending Type:~p , Byte: ~p .~n", [Type, Byte]), 
	call_port([Type,Byte]).

%%send byte at correct format to the port process, for further sending
call_port(Msg) ->
    complex ! {call, Msg}.
  
%%initialize the process that receive incoming data from C program and handels it
init_receiver()->
		PID2 = self(),
	  register(receiver, PID2),
	  receive_loop().
%%main loop of receiving procs. handle all incoming data  
receive_loop()->
    receive
	{complex, Result} ->
	    io:format("~p~n",[Result]) , receive_loop();
	stop -> io:format("reciever stopped~n")
    end.

%%initialize the process that actiaaly sends & receive data to/from C program
init_porter(ExtPrg) ->
    register(complex, self()),
    process_flag(trap_exit, true),
    Port = open_port({spawn, ExtPrg}, [{packet, 2}]),
    loop(Port).
    
%%main loop of port procs. sends/receive data to/from C progrm. by receiving data, sends it to receiving pocess for further analyze 
loop(Port) ->
    receive
	  {call, Msg} ->
	    Port ! {self(), {command, Msg}}, loop(Port);
		{Port, {data, Data}} ->
		    receiver ! {complex, decode(Data)}, loop(Port);
	stop ->
	    Port ! {self(), close},
	    receive
		{Port, closed} ->
		    io:format("C prog closed normaly~n"),
		    exit(normal)
	    end;
	{'EXIT', Port, Reason} ->
	    io:format("c process killed and notifies to erlang port~n"),
	    exit(port_terminated)
    end.

encode({Type, X}) -> [1, X].

decode([Int]) -> Int.
