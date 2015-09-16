-module(receiver2).
-export([start/2]).

-define(START_BYTE, 0).
-define(END_BYTE, 255).
-define(REG_BYTE, 1).
-define(OK, 1).
-define(ERROR, -1).

start(To, Port) ->
    init_receiver_porter( To, Port).


%%initialize the process that actiaaly sends & receive data to/from C program
init_receiver_porter( User_Pid, Port) ->
    register(receiver, self()),
    Msg_Buff = [],    
    loop(Port, User_Pid, Msg_Buff).
    
%%main loop of port procs. sends/receive data to/from C progrm. by receiving data, sends it to receiving pocess for further analyze 
loop(Port, User_Pid, Msg_Buff) ->
    receive
		{Port, {data, Data}} ->
				%io:format("at porter loop, got data: ~p~n", [Data]),
				Msg_Buff_new = decode(Data, Msg_Buff, 
User_Pid),
				loop(Port, User_Pid, Msg_Buff_new);
	stop ->
	    Port ! {self(), close},
	    receive
		{Port, closed} ->
		    io:format("C prog closed normaly~n"),
		    exit(normal)
	    end;
	{'EXIT', Port, Reason} ->
	    io:format("c process killed and notifies to erlang port(receiver). reason is:~p~n",[Reason]),
	    exit(port_terminated)
    end.
    

%case last byte of sent buffer, sent the whole buffer to receiver, and return a new empty buffer
decode([Type, Byte], Buff, User_Pid) when Type =:= 255 ->  L = Buff++[Byte], io:format("at decode, finished list:~p~n", [L]), User_Pid ! (L), [];
%else, add new byte to buffer and returne new buffer
decode([Type, Byte], Buff, Receiver) -> Buff ++ [Byte].
