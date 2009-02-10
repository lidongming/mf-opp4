%  This script generates an omnetpp configuration file
%  positioning N nodes on a horizontal line with
%  a fixed interval between nodes.
%  It also generates an omnetpp routes configuration file
%  which tells all nodes to directly send their packet
%  to the sink.
%  It generates N-1 simulations runs (N=nb nodes).
%  In each of these run, one node sends nbPackets to the sink.
%
% And finally it generates a small utility script to execute
% all scenarios. This script is named runall.sh
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Config                                  %%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
clear;

SizeX=500;
SizeY=100;
N=11;
nbPackets = 100;


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Create Omnet position and routing files %%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if 1,
	posY = SizeY / 2;
	file_position = fopen('omnetpp_positions.ini','w');
	fprintf(file_position,'# Node positions \n');
	fprintf(file_position, 'sim.numHosts = %d\n',N);
	fprintf(file_position, 'sim.playgroundSizeX = %d\n', SizeX); 
	fprintf(file_position, 'sim.playgroundSizeY = %d\n', SizeY);
	
	%%% Positions
	for i=0:N-1,
		positions(i+1, 1) = SizeX*i/(N-1);
		positions(i+1, 2) = posY;
    	fprintf(file_position, 'sim.host[%d].mobility.x = %d\n',i,positions(i+1, 1));
    	fprintf(file_position, 'sim.host[%d].mobility.y = %d\n',i,positions(i+1, 2));
    	fprintf(file_position, 'sim.host[%d].nic.mac.ipaddress = %d\n', i, i);
    	fprintf(file_position, 'sim.host[%d].nic.mac.macaddress = "%012X"\n', i, i);
	end
	fclose(file_position);
	
	%%% Routes
	file_routes = fopen('omnetpp_routes.ini','w');
	for j=1:N-1,
	    fprintf(file_routes, '%d %d %d\n', j, 0, 0);
	end
	fclose(file_routes);
	
	%%% Runs
	file_runs = fopen('omnetpp_runs.ini', 'w');
	for j=1:N-1,
		fprintf(file_runs, '\n[Run %d]\n', j);
		fprintf(file_runs, 'description="Node %d sends %d packets to sink."\n', j, nbPackets);
		fprintf(file_runs, 'sim.host[%d].appl.nbPackets = %d\n', j, nbPackets);
		fprintf(file_runs, 'sim.host[*].appl.nbPackets = 0\n\n');
		
	end
	fclose(file_runs);
	
end

%%%
% Save positions to a .dat file
%%%

save('positions', 'positions', '-V4');

%%%%%%%%%%%%%%%%%%%%
%% Generate script
%%%%%%%%%%%%%%%%%%%%

file_runall = fopen('runall.sh', 'w');
fprintf(file_runall, '#! /bin/sh\n\n');
fprintf(file_runall, 'mv scalar.clean scalar.clean.back\n');
fprintf(file_runall, 'mv omnetpp.sca omnetpp.sca.back\n');
fprintf(file_runall, 'for i in ');
for j=1:N-1,
	fprintf(file_runall, ' %d', j);
end	
fprintf(file_runall, ' ; do\n');
fprintf(file_runall, './csma -f omnetpp.ini -r $i\n');
fprintf(file_runall, 'done\n');
fprintf(file_runall, './cleanupall.sh\n');
fclose(file_runall);
